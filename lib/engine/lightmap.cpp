#include "lib/base/reflection/reflection_builder.h"
#include "lib/engine/lightmap.h"
#include "lib/resource/misc/image_ops.h"

SGE_REFLECT_TYPE(sge::SceneLightmap)
.implements<IToArchive>()
.implements<IFromArchive>();

namespace sge
{
    void SceneLightmap::to_archive(ArchiveWriter& writer) const
    {
        writer.as_object();
        writer.object_member("ldir", light_direction);
        writer.object_member("lint", light_intensity);

        writer.push_object_member("nodes");
        for (const auto& element : lightmap_elements)
        {
            char node_id_str[20];
            element.first.to_string(node_id_str, 20);

            writer.push_object_member(node_id_str);
            writer.as_object();

            const auto width = element.second.width;
            const auto height = element.second.height;
            const auto size = width * height;

            writer.push_object_member("w");
            writer.number(width);
            writer.pop(); // "w"

            writer.push_object_member("h");
            writer.number(height);
            writer.pop(); // "h"

            // Save individual components, compressed
            byte* image_buff;
            std::size_t image_buff_size;

            image_ops::save_rgbf_to_memory(element.second.basis_x_radiance.data()->vec(), width, height, 3, &image_buff, &image_buff_size);
            writer.push_object_member("x");
            writer.typed_array(image_buff, image_buff_size);
            writer.pop(); // "x"
            sge::free(image_buff);

            image_ops::save_rgbf_to_memory(element.second.basis_y_radiance.data()->vec(), width, height, 3, &image_buff, &image_buff_size);
            writer.push_object_member("y");
            writer.typed_array(image_buff, image_buff_size);
            writer.pop(); // "y"
            sge::free(image_buff);

            image_ops::save_rgbf_to_memory(element.second.basis_z_radiance.data()->vec(), width, height, 3, &image_buff, &image_buff_size);
            writer.push_object_member("z");
            writer.typed_array(image_buff, image_buff_size);
            writer.pop(); // "z"
            sge::free(image_buff);

            writer.push_object_member("d");
            writer.typed_array(element.second.direct_mask.data(), size);
            writer.pop(); // "n"

            writer.pop(); // node_id_str
        }
        writer.pop(); // "nodes"
    }

    void SceneLightmap::from_archive(ArchiveReader& reader)
    {
        reader.object_member("ldir", light_direction);
        reader.object_member("lint", light_intensity);

        lightmap_elements.clear();
        reader.pull_object_member("nodes");
        reader.enumerate_object_members([this, &reader](const char* node_id_str)
        {
            NodeId node_id;
            node_id.from_string(node_id_str);

            LightmapElement element;
            reader.object_member("w", element.width);
            reader.object_member("h", element.height);

            // Preallocate data
            const auto size = element.width * element.height;
            element.basis_x_radiance.assign(size, color::RGBF32::black());
            element.basis_y_radiance.assign(size, color::RGBF32::black());
            element.basis_z_radiance.assign(size, color::RGBF32::black());
            element.direct_mask.assign(size, 0);

            // Load individual components, from compressed memory
            std::vector<byte> compressed_buff;
            std::size_t compressed_buff_size;
            float* image_buff;
            int32 image_width;
            int32 image_height;
            byte image_num_channels;

            // Load data for x component
            reader.pull_object_member("x");
            reader.array_size(compressed_buff_size);
            compressed_buff.assign(compressed_buff_size, 0);
            reader.typed_array(compressed_buff.data(), compressed_buff_size);
            reader.pop(); // "x"

            // Construct image
            image_ops::load_rgbf_from_memory(compressed_buff.data(), compressed_buff_size, &image_buff, &image_width, &image_height, &image_num_channels);
            std::memcpy(element.basis_x_radiance.data(), image_buff, size * 3 * sizeof(float));
            sge::free(image_buff);

            // Load data for y component
            reader.pull_object_member("y");
            reader.array_size(compressed_buff_size);
            compressed_buff.assign(compressed_buff_size, 0);
            reader.typed_array(compressed_buff.data(), compressed_buff_size);
            reader.pop(); // "y"

            // Construct image
            image_ops::load_rgbf_from_memory(compressed_buff.data(), compressed_buff_size, &image_buff, &image_width, &image_height, &image_num_channels);
            std::memcpy(element.basis_y_radiance.data(), image_buff, size * 3 * sizeof(float));
            sge::free(image_buff);

            // Load data for z component
            reader.pull_object_member("z");
            reader.array_size(compressed_buff_size);
            compressed_buff.assign(compressed_buff_size, 0);
            reader.typed_array(compressed_buff.data(), compressed_buff_size);
            reader.pop(); // "z"

            // Construct image
            image_ops::load_rgbf_from_memory(compressed_buff.data(), compressed_buff_size, &image_buff, &image_width, &image_height, &image_num_channels);
            std::memcpy(element.basis_z_radiance.data(), image_buff, size * 3 * sizeof(float));
            sge::free(image_buff);

            reader.pull_object_member("d");
            reader.typed_array(element.direct_mask.data(), size);
            reader.pop(); // "d"

            // Add the element to the node table
            this->lightmap_elements.insert(std::make_pair(node_id, std::move(element)));
        });
        reader.pop(); // "nodes"
    }
}
