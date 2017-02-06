// Scene.cpp

#include <Core/Reflection/TypeDB.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Core/Util/StringUtils.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/SystemFrame.h"
#include "../include/Engine/UpdatePipeline.h"
#include "../include/Engine/TagCallbackInfo.h"
#include "../include/Engine/SystemInfo.h"
#include "../include/Engine/Util/VectorUtils.h"

SGE_REFLECT_TYPE(sge::Scene)
.implements<IToArchive>()
.implements<IFromArchive>();

namespace sge
{
	////////////////////////
	///   Constructors   ///

	Scene::Scene(TypeDB& typedb)
		: _type_db(&typedb)
	{
		_current_time = 0;
		_scene_data.next_entity_id = 2; // '1' is reserved for WORLD_ENTITY
	}

	Scene::~Scene()
	{
	}

	///////////////////
	///   Methods   ///

	void Scene::reset_scene()
	{
		_current_time = 0;
		_scene_data.next_entity_id = 2;
		_scene_data.entity_names = {};
		_scene_data.entity_parents = {};
        _scene_data.entity_children = {};

		for (auto& component_type : _scene_data.components)
		{
			component_type.second->reset();
		}
	}

	const SceneData& Scene::get_raw_scene_data() const
	{
		return _scene_data;
	}

	void Scene::to_archive(ArchiveWriter& writer) const
	{
		// Serialize next entity id
		writer.object_member("next_entity_id", _scene_data.next_entity_id);

		// Serialize all entities
		writer.push_object_member("entities");
		for (auto entity : _scene_data.entity_parents)
		{
			writer.push_object_member(to_string(entity.first).c_str());

			// Write the entity id and parent id
			writer.object_member("parent", entity.second);

			// See if the entity has a name
			auto nameIter = _scene_data.entity_names.find(entity.first);
			if (nameIter != _scene_data.entity_names.end())
			{
				writer.object_member("name", nameIter->second);
			}

			writer.pop();
		}
		writer.pop();

		// Serialize all components
		writer.push_object_member("components");
		for (const auto& componentType : _scene_data.components)
		{
			// Add the component type name as a field
			writer.push_object_member(componentType.first->name().c_str());
            componentType.second->to_archive(writer);
            writer.pop();
		}
		writer.pop();
	}

	void Scene::from_archive(ArchiveReader& reader)
	{
		reset_scene();

		// Deserialize entities
		reader.object_member("next_entity_id", _scene_data.next_entity_id);
		reader.pull_object_member("entities");

		// Get the number of entities
		std::size_t numEntities = 0;
		reader.object_size(numEntities);
		_scene_data.entity_parents.reserve(numEntities);

		// Load all entities
		reader.enumerate_object_members([&reader, &data=_scene_data](const char* id)
		{
			// Get the entities ID and Parent
			EntityId entity = NULL_ENTITY, parent = WORLD_ENTITY;
			entity = std::strtoull(id, nullptr, 10);
			reader.object_member("parent", parent);

			// Make sure the entity fields are valid
			if (entity == NULL_ENTITY || entity == WORLD_ENTITY || parent == NULL_ENTITY)
			{
				return;
			}

			// Add the entity to the world
			data.entity_parents.insert(std::make_pair(entity, parent));

			// Get the entity's name
			std::string name;
			if (reader.object_member("name", name))
			{
				data.entity_names.insert(std::make_pair(entity, std::move(name)));
			}
		});
		reader.pop();

		// Deserialize all components
		reader.pull_object_member("components");
		reader.enumerate_object_members([&](const char* name)
		{
			// Try to get the component type
			auto type = get_component_type(name);
			if (!type)
			{
				return;
			}

			// Clear the entities
			auto storageIter = _scene_data.components.find(type);

			// Deserialize the storage object
			storageIter->second->from_archive(reader);
		});

	    reader.pop();
	}

	TypeDB& Scene::get_type_db()
	{
		return *_type_db;
	}

	const TypeDB& Scene::get_type_db() const
	{
		return *_type_db;
	}

	const TypeInfo* Scene::get_component_type(const char* typeName) const
	{
		auto type = _type_db->find_type(typeName);
		if (type && _scene_data.components.find(type) != _scene_data.components.end())
		{
			return type;
		}

		return nullptr;
	}

	void Scene::register_component_type(const TypeInfo& type, std::unique_ptr<ComponentContainer> container)
	{
		// Insert the type
		_scene_data.components.insert(std::make_pair(&type, std::move(container)));
		_type_db->new_type(type);
	}

	void Scene::update(UpdatePipeline& pipeline, float dt)
	{
		// Update the time
		_current_time += dt;

		// For each step in the pipeline
		for (const auto& pipeline_step : pipeline.get_pipeline())
		{
            std::vector<SystemFrame> tag_frames;

            // For each system in the step TODO: Make this asynchronous
            for (const auto& system : pipeline_step)
            {
                // Create a frame for the system
                SystemFrame frame{ system->system_token, *this, _scene_data };

                // Call the system
                system->system_fn(frame, _current_time, dt);

                // If it has tags, add it to the tag frames array
                if (frame._has_tags)
                {
                    tag_frames.push_back(std::move(frame));
                }
            }

            // Keep running the tag frames until there are no tags left
            while (!tag_frames.empty())
            {
                tag_frames = apply_changes(pipeline, tag_frames.data(), tag_frames.size());
            }
		}
	}

    std::vector<SystemFrame> Scene::apply_changes(
        UpdatePipeline& pipeline,
        SystemFrame* frames,
        std::size_t num_frames)
    {
        std::vector<SystemFrame> result;

        auto* instance_buff = SGE_STACK_ALLOC(const EntityId*, num_frames + 1);
        auto* instance_count_buff = SGE_STACK_ALLOC(std::size_t, num_frames + 1);
        std::size_t buff_index = 0;

        // For each registered component type
        for (auto& comp_type : _scene_data.components)
        {
            instance_buff[0] = comp_type.second->get_instance_set();
            instance_count_buff[0] = comp_type.second->get_num_instances();
            buff_index = 1;

            std::size_t total_new = 0;

            // See if any frames have new components for this type
            for (std::size_t i = 0; i < num_frames; ++i)
            {
                const auto iter = frames[i]._ord_new_components.find(comp_type.first);
                if (iter == frames[i]._ord_new_components.end())
                {
                    continue;
                }

                instance_buff[buff_index] = iter->second.data();
                instance_count_buff[buff_index] = iter->second.size();
                ++buff_index;
                total_new += iter->second.size();
            }

            // If there are no new entities, just skip the next part
            if (total_new == 0)
            {
                continue;
            }

            // Count up the number of duplicates among these arrays
            const auto num_dups = rev_count_dups(instance_buff, instance_count_buff, buff_index);

            // Reserve a vector to hold the new instances
            std::vector<EntityId> new_instance_set;
            new_instance_set.assign(total_new - num_dups, NULL_ENTITY);

            // Add the new entities
            comp_type.second->create_instances(
                instance_buff + 1,
                instance_count_buff + 1,
                buff_index - 1,
                total_new - num_dups,
                new_instance_set.data());

            // For each callback for new components
            auto tag_iter = pipeline._tag_callbacks.find(&FNewComponent::type_info);
            if (tag_iter == pipeline._tag_callbacks.end())
            {
                continue;
            }

            // For each async token
            for (const auto& async_token : tag_iter->second)
            {
                // For each callback
                for (const auto& tag_cb : async_token.second)
                {
                    // Make sure this callback is compatible with this new component event
                    if (tag_cb.component_type != nullptr && tag_cb.component_type != comp_type.first)
                    {
                        continue;
                    }

                    // Create a system frame for the callback
                    SystemFrame tag_frame{ tag_cb.system, *this, _scene_data };

                    // Call the tag callback
                    tag_cb.callback(
                        tag_frame,
                        FNewComponent::type_info,
                        *comp_type.first,
                        new_instance_set.data(),
                        nullptr,
                        new_instance_set.size(),
                        nullptr);

                    // If the frame has changes, add it to the list
                    if (tag_frame._has_tags)
                    {
                        result.push_back(std::move(tag_frame));
                    }
                }
            }
        }

        // For each frame
        for (std::size_t i = 0; i < num_frames; ++i)
        {
            // For each type of tag in the frame
            for (const auto& tag_type : frames[i]._tags)
            {
                // Get the callbacks registered for this type of tag
                auto iter = pipeline._tag_callbacks.find(tag_type.first);
                if (iter == pipeline._tag_callbacks.end())
                {
                    continue;
                }

                // TODO: MAKE THIS ASYNCHRONOUS (AND NOT SHITTY)
                // For each tag callback set
                for (auto& tag_cb_set : iter->second)
                {
                    // For each tag callback
                    for (auto& tag_cb : tag_cb_set.second)
                    {
                        // Make sure this tag callback is compatible with this frame
                        if (tag_cb.system != UpdatePipeline::NO_SYSTEM && tag_cb.system == frames[i].system_token())
                        {
                            continue;
                        }

                        // For each tag set
                        for (const auto& tag_set : tag_type.second)
                        {
                            // Make sure this callback is compatible with the set
                            if (tag_cb.component_type != nullptr && tag_cb.component_type != &tag_set.component_type())
                            {
                                continue;
                            }

                            // Create a system frame for the callback
                            SystemFrame tag_frame{ tag_cb.system, *this, _scene_data };

                            // Call the tag callback
                            tag_cb.callback(
                                tag_frame,
                                *tag_type.first,
                                tag_set.component_type(),
                                tag_set.get_ord_entities(),
                                tag_set.get_tag_counts(),
                                tag_set.get_num_entities(),
                                tag_set.get_buffer());

                            // If the frame has changes, add it to the list
                            if (tag_frame._has_tags)
                            {
                                result.push_back(std::move(tag_frame));
                            }
                        }
                    }
                }
            }
        }

        return result;
    }
}
