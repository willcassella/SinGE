// Lightmapper.cpp

#include <array>
#include <future>
#include <algorithm>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <Core/Math/Mat4.h>
#include "../include/Lightmapper/Lightmapper.h"

namespace sge
{
    static Vec3 get_barycentric(float u, float v)
    {
        return Vec3{ u, v, 1.f - u - v };
    }

    static Vec3 get_barycentric(const IVec2<int32>* tri, IVec2<int32> point)
    {
        // Calculate the Barycentric coordinates of the point
        const float u = float((tri[1].y() - tri[2].y()) * (point.x() - tri[2].x()) + (tri[2].x() - tri[1].x()) * (point.y() - tri[2].y())) /
            ((tri[1].y() - tri[2].y()) * (tri[0].x() - tri[2].x()) + (tri[2].x() - tri[1].x()) * (tri[0].y() - tri[2].y()));

        const float v = float((tri[2].y() - tri[0].y()) * (point.x() - tri[2].x()) + (tri[0].x() - tri[2].x()) * (point.y() - tri[2].y())) /
            ((tri[1].y() - tri[2].y()) * (tri[0].x() - tri[2].x()) + (tri[2].x() - tri[1].x()) * (tri[0].y() - tri[2].y()));

        return get_barycentric(u, v);
    }

    static bool within_triangle(const Vec3& bary)
    {
        // Return if the point is within the triangle (with a margin)
        return bary.x() >= -0.0001f && bary.x() <= 1.0001f && bary.y() >= -0.0001f && bary.y() <= 1.0001f && bary.z() >= -0.0001f && bary.z() <= 1.0001f;
    }

    template <typename T>
    T bary_interpolate(const Vec3& bary_coords, const T* values)
    {
        return bary_coords.x() * values[0] + bary_coords.y() * values[1] + bary_coords.z() * values[2];
    }

    float randf(float min, float max)
    {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
    }

    struct EmbreeGeometry
    {
        //////////////////
        ///   Fields   ///
    public:

        const StaticMesh::SubMesh* sub_mesh;
        const Image* albedo;
        const color::RGBF32* lightmap;
    };

    static std::vector<EmbreeGeometry> fill_scene(
        RTCScene scene,
        const LightmapOccluder* occluders,
        std::size_t num_occluders)
    {
        std::vector<EmbreeGeometry> result;

        // For each occluder object given
        for (std::size_t occluder_i = 0; occluder_i < num_occluders; ++occluder_i)
        {
            const auto* const sub_meshes = occluders[occluder_i].mesh->sub_meshes();
            const auto num_sub_meshes = occluders[occluder_i].mesh->num_sub_meshes();
            const auto* const occluder_albedo = occluders[occluder_i].albedo;
            const auto* const occluder_lightmap = occluders[occluder_i].lightmap;
            const auto occluder_transform = occluders[occluder_i].transform;

            // For each submesh in the occluder
            for (std::size_t mesh_i = 0; mesh_i < num_sub_meshes; ++mesh_i)
            {
                // Create the mesh
                auto rtc_mesh = rtcNewTriangleMesh(
                    scene,
                    RTC_GEOMETRY_STATIC,
                    sub_meshes[mesh_i].num_triangles(),
                    sub_meshes[mesh_i].num_verts());

                // Add it to the vector
                result.push_back({ sub_meshes + mesh_i, occluder_albedo, occluder_lightmap });

                // Load in vertex positions
                const auto num_verts = sub_meshes[mesh_i].num_verts();
                const auto* const sub_mesh_positions = sub_meshes[mesh_i].vertex_positions();
                auto* const pos_buffer = (Vec4*)rtcMapBuffer(scene, rtc_mesh, RTC_VERTEX_BUFFER);
                for (std::size_t vert_i = 0; vert_i < num_verts; ++vert_i)
                {
                    const auto pos = occluder_transform * sub_mesh_positions[vert_i];
                    pos_buffer[vert_i] = Vec4{ pos, 0.f };
                }
                rtcUnmapBuffer(scene, rtc_mesh, RTC_VERTEX_BUFFER);

                // Load in triangle indices
                const auto num_triangle_elements = sub_meshes[mesh_i].num_triangle_elements();
                const auto* const sub_mesh_triangle_elements = sub_meshes[mesh_i].triangle_elements();
                auto* index_buffer = (int*)rtcMapBuffer(scene, rtc_mesh, RTC_INDEX_BUFFER);
                for (std::size_t index_i = 0; index_i < num_triangle_elements; ++index_i)
                {
                    index_buffer[index_i] = static_cast<int>(sub_mesh_triangle_elements[index_i]);
                }
                rtcUnmapBuffer(scene, rtc_mesh, RTC_INDEX_BUFFER);
            }
        }

        rtcCommit(scene);

        // Return the created geometries
        return result;
    }

    void generate_lightmap_pack(
        LightmapObject* objects,
        std::size_t num_objects)
    {
        // Only support one object right now
        objects[0].lightmap_pos = Vec2::zero();
        objects[0].lightmap_exts = 1.0f;
    }

    void generate_lightmap_texels(
        const LightmapObject* objects,
        std::size_t num_objects,
        int32 width,
        int32 height,
        LightmapTexel* out_texels,
        byte* out_texel_mask)
    {
        // Initialize the mask to 0
        std::memset(out_texel_mask, 0, width * height);

        // Get the submeshes in the mesh
        const auto* const sub_meshes = objects[0].mesh->sub_meshes();
        const auto num_sub_meshes = objects[0].mesh->num_sub_meshes();

        // For each submesh
        for (std::size_t mesh_i = 0; mesh_i < num_sub_meshes; ++mesh_i)
        {
            // Get the properties of the submesh
            const auto num_triangles = sub_meshes[mesh_i].num_triangles();
            const auto* const triangle_elements = sub_meshes[mesh_i].triangle_elements();
            const auto* const material_uv = sub_meshes[mesh_i].material_uv();
            const auto* const triangle_pos = sub_meshes[mesh_i].vertex_positions();
            const auto* const triangle_norms = sub_meshes[mesh_i].vertex_normals();
            const auto* const triangle_tangents = sub_meshes[mesh_i].vertex_tangents();
            const auto* const triangle_bitangent_signs = sub_meshes[mesh_i].bitangent_signs();

            // For each triangle
            for (std::size_t tri_i = 0; tri_i < num_triangles; ++tri_i)
            {
                // Get vertex indices for this triangle
                uint32 vert_indices[3];
                std::memcpy(vert_indices, triangle_elements + tri_i * 3, sizeof(uint32) * 3);

                // The properties of each vertex on the triangle
                IVec2<int32> vert_uv_coords[3];
                Vec3 vert_pos[3];
                Vec3 vert_norms[3];
                Vec3 vert_tans[3];
                Vec3 vert_btans[3];

                // The min and max UV coordinates of the triangle
                int32 min_x = INT32_MAX, min_y = INT32_MAX, max_x = 0, max_y = 0;

                // For each vert in the triangle
                for (int vert_i = 0; vert_i < 3; ++vert_i)
                {
                    // Convert the normalized UV coordinates to image coordinates
                    UHalfVec2 coord = material_uv[vert_indices[vert_i]];
                    const int32 x = static_cast<int32>(coord.norm_f32_x() * width);
                    const int32 y = static_cast<int32>(coord.norm_f32_y() * height);

                    // Set the coordinates
                    vert_uv_coords[vert_i].x(x);
                    vert_uv_coords[vert_i].y(y);

                    // Set the max and min coordinates
                    min_x = std::min(min_x, x);
                    min_y = std::min(min_y, y);
                    max_x = std::max(max_x, x);
                    max_y = std::max(max_y, y);

                    // Set the position
                    vert_pos[vert_i] = triangle_pos[vert_indices[vert_i]];

                    // Set the normal
                    vert_norms[vert_i] = Vec3{
                        triangle_norms[vert_indices[vert_i]].norm_f32_x(),
                        triangle_norms[vert_indices[vert_i]].norm_f32_y(),
                        triangle_norms[vert_indices[vert_i]].norm_f32_z()
                    };

                    // Set the tangent
                    vert_tans[vert_i] = Vec3{
                        triangle_tangents[vert_indices[vert_i]].norm_f32_x(),
                        triangle_tangents[vert_indices[vert_i]].norm_f32_y(),
                        triangle_tangents[vert_indices[vert_i]].norm_f32_z()
                    };

                    // Re-orthogonalize tangent with respect to normal
                    vert_tans[vert_i] = (vert_tans[vert_i] - Vec3::dot(vert_tans[vert_i], vert_norms[vert_i]) * vert_norms[vert_i]).normalized();

                    // Calculate bitangent
                    vert_btans[vert_i] = Vec3::cross(vert_norms[vert_i], vert_tans[vert_i]) * triangle_bitangent_signs[vert_indices[vert_i]];
                }

                // Iterate from min to max
                for (int32 y = min_y; y < max_y; ++y)
                {
                    for (int32 x = min_x; x <= max_x; ++x)
                    {
                        // Get the barycentric coordinates for this point
                        const auto bary_coords = get_barycentric(vert_uv_coords, { x, y });

                        // Make sure this pixel is within the triangle
                        if (!within_triangle(bary_coords))
                        {
                            continue;
                        }

                        // Get the surface normal, tangent, and bitangent at this pixel
                        const Vec3 pixel_pos = bary_interpolate(bary_coords, vert_pos);
                        const Vec3 pixel_norm = bary_interpolate(bary_coords, vert_norms).normalized();
                        const Vec3 pixel_tangent = bary_interpolate(bary_coords, vert_tans).normalized();
                        const Vec3 pixel_bitangent = bary_interpolate(bary_coords, vert_btans).normalized();

                        // Construct TBN matrix
                        const Mat4 TBN{
                            pixel_tangent.x(), pixel_bitangent.x(), pixel_norm.x(), 0,
                            pixel_tangent.y(), pixel_bitangent.y(), pixel_norm.y(), 0,
                            pixel_tangent.z(), pixel_bitangent.z(), pixel_norm.z(), 0,
                            0, 0, 0, 1
                        };

                        // Assign to texel element
                        const std::size_t index = x + y * width;
                        out_texels[index].world_pos = Vec4{ pixel_pos, 0.f };
                        out_texels[index].TBN = TBN;

                        // Set the mask for this texel
                        out_texel_mask[index] = 0xFF;
                    }
                }
            }
        }
    }

    void compute_direct_irradiance(
        LightmapLight light,
        const LightmapOccluder* occluders,
        std::size_t num_occluders,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* out_irradiance)
    {
        // Create and fill the scene
        RTCDevice rtc_device = rtcNewDevice();
        RTCScene rtc_scene = rtcDeviceNewScene(rtc_device, RTC_SCENE_STATIC, RTC_INTERSECT1);
        fill_scene(rtc_scene, occluders, num_occluders);

        // Determine what direction each ray should point
        const Vec3 ray_dir = (light.direction * -1).normalized();

        // Determine how long the ray should be
        const float ray_len = std::numeric_limits<float>::max();

        for (int32 y = 0; y < height; ++y)
        {
            for (int32 x = 0; x < width; ++x)
            {
                // Get the index of this texel
                const int32 index = x + y * width;

                // Make sure this texel is enabled
                if (!texel_mask[index])
                {
                    continue;
                }

                // Get the texel position and normal
                const auto texel_pos = texels[index].world_pos;
                const Vec3 texel_norm = { texels[index].TBN[2][0], texels[index].TBN[2][1], texels[index].TBN[2][2] };

                // Create the ray
                RTCRay light_ray;
                std::memcpy(light_ray.org, texel_pos.vec(), sizeof(float) * 3);
                std::memcpy(light_ray.dir, ray_dir.vec(), sizeof(float) * 3);
                light_ray.tnear = 0.001f;
                light_ray.tfar = ray_len;
                light_ray.geomID = 1;
                light_ray.primID = 0;
                light_ray.mask = RTC_INVALID_GEOMETRY_ID;
                light_ray.time = 0;

                // Cast the ray
                rtcOccluded(rtc_scene, light_ray);

                // Test the ray
                if (light_ray.geomID)
                {
                    const auto dot = std::max(Vec3::dot(texel_norm, ray_dir), 0.f);
                    out_irradiance[index] = light.color * dot;
                }
            }
        }

        // Clean up
        rtcDeleteScene(rtc_scene);
        rtcDeleteDevice(rtc_device);
    }

    void compute_indirect_irradiance(
        const LightmapOccluder* occluders,
        std::size_t num_occluders,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBF32* out_irradiance)
    {
        // Create and fill the RTC scene
        RTCDevice rtc_device = rtcNewDevice();
        RTCScene rtc_scene = rtcDeviceNewScene(rtc_device, RTC_SCENE_STATIC, RTC_INTERSECT8);
        const auto geometries = fill_scene(rtc_scene, occluders, num_occluders);

        // Iterate from min to max
        for (int32 y = 0; y < height; ++y)
        {
            for (int32 x = 0; x < height; ++x)
            {
                // Get the index of this texel
                const int32 texel_index = x + y * width;

                // Make sure this texel isn't masked out
                if (!texel_mask[texel_index])
                {
                    continue;
                }

                // Get the texel position, TBN matrix, and normal vector
                const auto texel_pos = texels[texel_index].world_pos;
                const auto texel_TBN = texels[texel_index].TBN;
                const auto texel_norm = Vec3{ texel_TBN[2][0], texel_TBN[2][1], texel_TBN[2][2] };

                // Create the ray valid mask
                std::array<uint32, 8> valid_mask;
                valid_mask.fill(0xFFFFFFFF);

                // For each raycasting iteration
                for (int iter_i = 0; iter_i < 64; ++iter_i)
                {
                    RTCRay8 indirect_rays;
                    for (int i = 0; i < 8; ++i)
                    {
                        // Set origin
                        indirect_rays.orgx[i] = texel_pos.x();
                        indirect_rays.orgy[i] = texel_pos.y();
                        indirect_rays.orgz[i] = texel_pos.z();

                        // Set direction
                        const Vec3 dir = texel_TBN * Vec3{
                            randf(-1.0f, 1.0f),
                            randf(-1.0f, 1.0f),
                            randf(0.15f, 1.0f) }.normalized();
                        indirect_rays.dirx[i] = dir.x();
                        indirect_rays.diry[i] = dir.y();
                        indirect_rays.dirz[i] = dir.z();

                        // Set near and far distances
                        indirect_rays.tnear[i] = 0.001f;
                        indirect_rays.tfar[i] = std::numeric_limits<float>::max();
                        indirect_rays.time[i] = 0;
                        indirect_rays.mask[i] = RTC_INVALID_GEOMETRY_ID;
                        indirect_rays.geomID[i] = RTC_INVALID_GEOMETRY_ID;
                        indirect_rays.instID[i] = RTC_INVALID_GEOMETRY_ID;
                    }

                    // Cast the rays
                    rtcIntersect8(valid_mask.data(), rtc_scene, indirect_rays);

                    // Test rays
                    color::RGBF32 result;
                    for (int i = 0; i < 8; ++i)
                    {
                        // If the ray didn't hit anything
                        if (indirect_rays.geomID[i] == RTC_INVALID_GEOMETRY_ID)
                        {
                            continue;
                        }

                        // Get the hit triangle
                        const auto hit_mesh = geometries[indirect_rays.geomID[i]];
                        const auto* const hit_elems = hit_mesh.sub_mesh->triangle_elements() + indirect_rays.primID[i] * 3;

                        // Calculate the uv coordinates for the hit triangle
                        IVec2<int32> hit_tri_uvs[3];
                        for (int vert_i = 0; vert_i < 3; ++vert_i)
                        {
                            const auto uv = hit_mesh.sub_mesh->material_uv()[hit_elems[vert_i]];
                            hit_tri_uvs[vert_i].x(static_cast<int32>(uv.norm_f32_x() * INT32_MAX));
                            hit_tri_uvs[vert_i].y(static_cast<int32>(uv.norm_f32_y() * INT32_MAX));
                        }

                        // Calculate the hit uv coordinate
                        const auto hit_u = indirect_rays.u[i];
                        const auto hit_v = indirect_rays.v[i];
                        const Vec3 hit_bary = { 1.f - hit_u - hit_v, hit_u, hit_v };
                        const auto hit_uv = bary_interpolate(hit_bary, hit_tri_uvs);

                        // Calculate the hit albedo UV coordinates
                        const auto hit_albedo_uv = IVec2<int32>{
                            static_cast<int32>(hit_uv.norm_f32_x() * hit_mesh.albedo->get_width()),
                            static_cast<int32>(hit_uv.norm_f32_y() * hit_mesh.albedo->get_height())
                        };

                        // Sample the albedo value at the hit point
                        const Vec3 hit_albedo{
                            (float)hit_mesh.albedo->get_bitmap()[(hit_albedo_uv.x() + hit_albedo_uv.y() * width) * 4 + 0] / UINT8_MAX,
                            (float)hit_mesh.albedo->get_bitmap()[(hit_albedo_uv.x() + hit_albedo_uv.y() * width) * 4 + 1] / UINT8_MAX,
                            (float)hit_mesh.albedo->get_bitmap()[(hit_albedo_uv.x() + hit_albedo_uv.y() * width) * 4 + 2] / UINT8_MAX
                        };

                        // Calculate the hit irradiance uv coordinates
                        const auto hit_irradiance_uv = IVec2<int32>{
                            static_cast<int32>(hit_uv.norm_f32_x() * width),
                            static_cast<int32>(hit_uv.norm_f32_y() * height) };

                        // Sample the irradiance at the hit location
                        const color::RGBF32 hit_irradiance = hit_mesh.lightmap[hit_irradiance_uv.x() + hit_irradiance_uv.y() * width];

                        // Calculate the influence from this point
                        const auto dir = Vec3{ indirect_rays.dirx[i], indirect_rays.diry[i], indirect_rays.dirz[i] };
                        const auto dot = Vec3::dot(texel_norm, dir);

                        // Write out the irradiance
                        result.red(result.red() + dot * hit_irradiance.red() * hit_albedo.x());
                        result.green(result.green() + dot * hit_irradiance.green() * hit_albedo.y());
                        result.blue(result.blue() + dot * hit_irradiance.blue() * hit_albedo.z());
                    }

                    // Divide by number of samples and probability (Monte carlo integration)
                    result = result / (64 * 8) * 2 * 3.1415926539f;

                    // Write out irradiance
                    out_irradiance[texel_index] += result;
                }
            }
        }

        // Clean up
        rtcDeleteScene(rtc_scene);
        rtcDeleteDevice(rtc_device);
    }
}
