// Lightmapper.cpp

#include <array>
#include <algorithm>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <glm/glm.hpp>
#include <Core/Math/Mat4.h>
#include <Resource/Resources/Image.h>
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

    struct LightmapScene
    {
        RTCDevice rtc_device;
        RTCScene rtc_scene;
        const LightmapOccluder* occluders;
        std::size_t num_occluders;
    };

    LightmapScene* new_lightmap_scene(
        const LightmapOccluder* occluders,
        std::size_t num_occluders)
    {
        // Create the scene
        auto* scene = new LightmapScene{};
        scene->rtc_device = rtcNewDevice();
        scene->rtc_scene = rtcDeviceNewScene(scene->rtc_device, RTC_SCENE_STATIC, RTC_INTERSECT1 | RTC_INTERSECT8);
        scene->occluders = occluders;
        scene->num_occluders = num_occluders;

        // Fill it
        auto* const rtc_scene = scene->rtc_scene;
        for (std::size_t occluder_i = 0; occluder_i < num_occluders; ++occluder_i)
        {
            const auto* const occluder_mesh = occluders[occluder_i].mesh;
            const auto occluder_transform = occluders[occluder_i].world_transform;

            // Create the mesh
            auto rtc_mesh = rtcNewTriangleMesh(
                rtc_scene,
                RTC_GEOMETRY_STATIC,
                occluder_mesh->num_triangles(),
                occluder_mesh->num_verts());

            // Load in vertex positions
            const auto num_verts = occluder_mesh->num_verts();
            const auto* const vert_positions = occluder_mesh->vertex_positions();
            auto* const pos_buffer = (Vec4*)rtcMapBuffer(rtc_scene, rtc_mesh, RTC_VERTEX_BUFFER);
            for (std::size_t vert_i = 0; vert_i < num_verts; ++vert_i)
            {
                const auto pos = occluder_transform * vert_positions[vert_i];
                pos_buffer[vert_i] = Vec4{ pos, 0.f };
            }
            rtcUnmapBuffer(rtc_scene, rtc_mesh, RTC_VERTEX_BUFFER);

            // Load in triangle indices
            const auto num_triangle_elements = occluder_mesh->num_triangle_elements();
            const auto* const triangle_elements = occluder_mesh->triangle_elements();
            auto* index_buffer = (int*)rtcMapBuffer(rtc_scene, rtc_mesh, RTC_INDEX_BUFFER);
            for (std::size_t index_i = 0; index_i < num_triangle_elements; ++index_i)
            {
                index_buffer[index_i] = static_cast<int>(triangle_elements[index_i]);
            }
            rtcUnmapBuffer(rtc_scene, rtc_mesh, RTC_INDEX_BUFFER);
        }

        rtcCommit(rtc_scene);

        return scene;
    }

    void free_lightmap_scene(
        LightmapScene* scene)
    {
        // Destroy rtc resources
        rtcDeleteScene(scene->rtc_scene);
        rtcDeleteDevice(scene->rtc_device);

        // Destroy the scene
        delete scene;
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

        // For each object
        for (std::size_t mesh_i = 0; mesh_i < num_objects; ++mesh_i)
        {
            // Get the properties of the submesh
            const auto mesh_transform = objects[mesh_i].world_transform;
            Mat4 mesh_inverse_transpose;

            // Construct inverse transpose matrix
            {
                glm::mat4 inverse_trans;
                std::memcpy(&inverse_trans, &mesh_transform, sizeof(Mat4));

                inverse_trans = glm::transpose(glm::inverse(inverse_trans));

                std::memcpy(&mesh_inverse_transpose, &inverse_trans, sizeof(Mat4));
            }

            const auto num_triangles = objects[mesh_i].mesh->num_triangles();
            const auto* const triangle_elements = objects[mesh_i].mesh->triangle_elements();
            const auto* const lightmap_uv = objects[mesh_i].mesh->lightmap_uv();
            const auto* const triangle_pos = objects[mesh_i].mesh->vertex_positions();
            const auto* const triangle_norms = objects[mesh_i].mesh->vertex_normals();
            const auto* const triangle_tangents = objects[mesh_i].mesh->vertex_tangents();
            const auto* const triangle_bitangent_signs = objects[mesh_i].mesh->bitangent_signs();

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
                    UHalfVec2 coord = lightmap_uv[vert_indices[vert_i]];
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
                    vert_pos[vert_i] = mesh_transform * triangle_pos[vert_indices[vert_i]];

                    // Set the normal
                    vert_norms[vert_i] = (mesh_inverse_transpose * Vec3{
                        triangle_norms[vert_indices[vert_i]].norm_f32_x(),
                        triangle_norms[vert_indices[vert_i]].norm_f32_y(),
                        triangle_norms[vert_indices[vert_i]].norm_f32_z()
                    }).normalized();

                    // Set the tangent
                    vert_tans[vert_i] = (mesh_inverse_transpose * Vec3{
                        triangle_tangents[vert_indices[vert_i]].norm_f32_x(),
                        triangle_tangents[vert_indices[vert_i]].norm_f32_y(),
                        triangle_tangents[vert_indices[vert_i]].norm_f32_z()
                    }).normalized();

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
        const LightmapScene* scene,
        LightmapLight light,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBAF32* out_irradiance)
    {
        // Get the scene
        auto* const rtc_scene = scene->rtc_scene;

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
                const auto texel_norm = Vec3{ texels[index].TBN[2][0], texels[index].TBN[2][1], texels[index].TBN[2][2] };

                // Create the ray
                RTCRay light_ray;
                std::memcpy(light_ray.org, texel_pos.vec(), sizeof(float) * 3);
                std::memcpy(light_ray.dir, ray_dir.vec(), sizeof(float) * 3);
                light_ray.tnear = 0.0001f;
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
                    const auto color_intensity = light.intensity * dot;
                    out_irradiance[index] = color::RGBAF32{ color_intensity.red(), color_intensity.green(), color_intensity.blue(), dot };
                }
            }
        }
    }

    void compute_indirect_irradiance(
        const LightmapScene* scene,
        int32 num_sample_sets,
        int32 width,
        int32 height,
        const LightmapTexel* texels,
        const byte* texel_mask,
        color::RGBAF32* out_irradiance)
    {
        // Get the scene
        auto* const rtc_scene = scene->rtc_scene;
        const auto* const occluders = scene->occluders;
        const float normalization_factor = 1.f / (static_cast<float>(num_sample_sets) * 8) * 2 * 3.1415926539f;

		// Create the ray valid mask
		alignas(32) std::array<uint32, 8> valid_mask;
		valid_mask.fill(0xFFFFFFFF);

        // Iterate from min to max
        for (int32 y = 0; y < height; ++y)
        {
            for (int32 x = 0; x < width; ++x)
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

                // For each raycasting iteration
                for (int iter_i = 0; iter_i < num_sample_sets; ++iter_i)
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
                        indirect_rays.tnear[i] = 0.0001f;
                        indirect_rays.tfar[i] = std::numeric_limits<float>::max();
                        indirect_rays.time[i] = 0;
                        indirect_rays.mask[i] = RTC_INVALID_GEOMETRY_ID;
                        indirect_rays.geomID[i] = RTC_INVALID_GEOMETRY_ID;
                        indirect_rays.instID[i] = RTC_INVALID_GEOMETRY_ID;
                    }

                    // Cast the rays
                    rtcIntersect8(valid_mask.data(), rtc_scene, indirect_rays);

                    // Test rays
                    auto result = color::RGBAF32::zero();
                    for (int i = 0; i < 8; ++i)
                    {
                        // If the ray didn't hit anything
                        if (indirect_rays.geomID[i] == RTC_INVALID_GEOMETRY_ID)
                        {
                            continue;
                        }

						// Get the hit normal direction
						const auto hit_norm = Vec3{ indirect_rays.Ngx[i], indirect_rays.Ngy[i], indirect_rays.Ngz[i] };

						// Make sure we didn't hit the backside
						if (Vec3::dot(hit_norm, texel_norm) <= 0.f)
						{
							continue;
						}

                        // Get the hit triangle
                        const auto hit_mesh = occluders[indirect_rays.geomID[i]];
                        const auto* const hit_elems = hit_mesh.mesh->triangle_elements() + indirect_rays.primID[i] * 3;

                        // Calculate the uv coordinates for the hit triangle
                        IVec2<int32> hit_tri_lm_uvs[3];
                        for (int vert_i = 0; vert_i < 3; ++vert_i)
                        {
                            const auto lm_uv = hit_mesh.mesh->lightmap_uv()[hit_elems[vert_i]];
                            hit_tri_lm_uvs[vert_i].x(static_cast<int32>(lm_uv.norm_f32_x() * INT32_MAX));
                            hit_tri_lm_uvs[vert_i].y(static_cast<int32>(lm_uv.norm_f32_y() * INT32_MAX));
                        }

                        // Calculate the hit uv coordinate
                        const auto hit_u = indirect_rays.u[i];
                        const auto hit_v = indirect_rays.v[i];
                        const auto hit_bary = Vec3{ 1.f - hit_u - hit_v, hit_u, hit_v };
                        const auto hit_lm_uv = bary_interpolate(hit_bary, hit_tri_lm_uvs);

                        // Get the hit color
                        const auto hit_color = hit_mesh.base_color;

                        // Calculate the hit irradiance uv coordinates
                        auto hit_irradiance_uv = IVec2<int32>{
                            static_cast<int32>(hit_lm_uv.norm_f32_x() * hit_mesh.irradiance_width),
                            static_cast<int32>(hit_lm_uv.norm_f32_y() * hit_mesh.irradiance_height) };

                        // Clamp irradiance UV
                        hit_irradiance_uv.x(std::max(std::min(hit_mesh.irradiance_width, hit_irradiance_uv.x()), 0));
                        hit_irradiance_uv.y(std::max(std::min(hit_mesh.irradiance_height, hit_irradiance_uv.y()), 0));

                        // Sample the irradiance at the hit location
                        const color::RGBAF32 hit_irradiance = hit_mesh.irradiance[hit_irradiance_uv.x() + hit_irradiance_uv.y() * hit_mesh.irradiance_width];

                        // Calculate the influence from this point
                        const auto dir = Vec3{ indirect_rays.dirx[i], indirect_rays.diry[i], indirect_rays.dirz[i] };
                        const auto dot = Vec3::dot(texel_norm, dir);

                        // Write out the irradiance
                        result.red(result.red() + dot * hit_irradiance.red() * hit_color.red());
                        result.green(result.green() + dot * hit_irradiance.green() * hit_color.green());
                        result.blue(result.blue() + dot * hit_irradiance.blue() * hit_color.blue());
                    }

                    // Divide by number of samples and probability (Monte carlo integration)
                    result = result * normalization_factor;

                    // Write out irradiance (ignore alpha component)
                    out_irradiance[texel_index] += result;
                }
            }
        }
    }

    void postprocess_irradiance(int32 width, int32 height, int32 num_steps, color::RGBAF32* irradiance)
    {
        // Create a copy of the image
        std::vector<color::RGBAF32> temp;
        temp.assign(width * height, color::RGBAF32::black());

        for (int32 i = 0; i < num_steps; ++i)
        {
            Image::dilate_rgbf(irradiance->vec(), width, height, 4, temp.data()->vec());
            Image::smooth_rgbf(temp.data()->vec(), width, height, 4, irradiance->vec());
        }
    }
}
