// SystemFrame.cpp

#include <Core/Memory/Functions.h>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/SystemFrame.h"
#include "../include/Engine/ProcessingFrame.h"
#include "../include/Engine/Scene.h"
#include "../include/Engine/UpdatePipeline.h"
#include "../include/Engine/Util/VectorUtils.h"

SGE_REFLECT_TYPE(sge::SystemFrame);

namespace sge
{
	SystemFrame::SystemFrame(UpdatePipeline::SystemToken system_token, const Scene& scene, SceneData& scene_data)
		: _system_token(system_token),
        _has_tags(false),
        _scene(&scene),
		_scene_data(&scene_data)
	{
	}

    UpdatePipeline::SystemToken SystemFrame::system_token() const
    {
        return _system_token;
    }

    const Scene& SystemFrame::scene() const
    {
        return *_scene;
    }

    void SystemFrame::yield()
	{
	}

    void SystemFrame::create_entities(EntityId* out_entities, std::size_t num) const
    {
        for (std::size_t i = 0; i < num; ++i)
        {
            out_entities[i] = _scene_data->next_entity_id++;
        }
    }

    void SystemFrame::destroy_entities(const EntityId* ordered_entities, std::size_t num_entities)
    {
        _has_tags = true;

        // Add the entities to the destroyed set
        insert_ord_entities(_ord_destroyed_entities, ordered_entities, num_entities);

        // Destroy all children of these entities
        for (std::size_t i = 0; i < num_entities; ++i)
        {
            auto iter = _scene_data->entity_children.find(ordered_entities[i]);
            if (iter != _scene_data->entity_children.end())
            {
                destroy_entities(iter->second.data(), iter->second.size());
            }
        }
    }

	EntityId SystemFrame::get_entity_parent(EntityId entity) const
	{
		if (entity == NULL_ENTITY || entity == WORLD_ENTITY)
		{
			return NULL_ENTITY;
		}

		auto iter = _scene_data->entity_parents.find(entity);
		return iter != _scene_data->entity_parents.end() ? iter->second : WORLD_ENTITY;
	}

    void SystemFrame::set_entities_parent(EntityId parent, const EntityId* ord_children, std::size_t num_children) const
    {
        // Make sure the parent is a valid parent
        if (parent == NULL_ENTITY || parent >= _scene_data->next_entity_id)
        {
            return;
        }

        // Set the parents
        auto& children = _scene_data->entity_children[parent];
        insert_ord_entities(_scene_data->entity_children[parent], ord_children, num_children);

        for (std::size_t i = 0; i < num_children; ++i)
        {

        }
    }

	std::string SystemFrame::get_entity_name(EntityId entity) const
	{
		if (entity == NULL_ENTITY)
		{
			return "null";
		}

		if (entity == WORLD_ENTITY)
		{
			return "World";
		}

		// Search for the name
		auto iter = _scene_data->entity_names.find(entity);
		return iter != _scene_data->entity_names.end() ? iter->second : "";
	}

    void SystemFrame::set_entity_name(EntityId entity, std::string name) const
    {
        // Make sure the entity is a user entity, and the name is valid
        if (entity == NULL_ENTITY || entity == WORLD_ENTITY || name.empty())
        {
            return;
        }

        // Insert the name
        _scene_data->entity_names[entity] = std::move(name);
    }

    void SystemFrame::remove_entity_name(EntityId entity) const
    {
        auto iter = _scene_data->entity_names.find(entity);
        if (iter == _scene_data->entity_names.end())
        {
            return;
        }

        _scene_data->entity_names.erase(iter);
    }

    void SystemFrame::create_components(const TypeInfo& type, const EntityId* ord_entities, std::size_t num_entities)
    {
        _has_tags = true;

        // Make sure all of the entities are valid entities
        for (std::size_t i = 0; i < num_entities; ++i)
        {
            if (ord_entities[i] == NULL_ENTITY || ord_entities[i] >= _scene_data->next_entity_id)
            {
                return;
            }
        }

        // Insert the request
        insert_ord_entities(_ord_new_components[&type], ord_entities, num_entities);
    }

    void SystemFrame::destroy_components(
        const TypeInfo& type,
        const EntityId* ord_entities,
        std::size_t num_entities)
    {
        _has_tags = true;
        insert_ord_entities(_ord_destroyed_components[&type], ord_entities, num_entities);
    }

	void SystemFrame::process_entities(
        const TypeInfo* const types[],
        std::size_t num_types,
        FunctionView<ProcessFn> process_fn)
	{
        if (num_types == 0)
        {
            return;
        }

        impl_process_entities(nullptr, nullptr, 0, types, num_types, process_fn);
	}

    void SystemFrame::process_entities_mut(
        const TypeInfo* const types[],
        std::size_t num_types,
        FunctionView<ProcessMutFn> process_fn)
    {
        if (num_types == 0)
        {
            return;
        }

        impl_process_entities(nullptr, nullptr, 0, types, num_types, process_fn);
    }

	void SystemFrame::process_entities(
        const EntityId* const* SGE_RESTRICT user_start_iters,
        const EntityId* const* SGE_RESTRICT user_end_iters,
        std::size_t num_user_iters,
        const TypeInfo* const types[],
        std::size_t num_types,
        FunctionView<ProcessFn> process_fn)
	{
        if (num_user_iters == 0)
        {
            return;
        }

		impl_process_entities(user_start_iters, user_end_iters, num_user_iters, types, num_types, process_fn);
	}

    void SystemFrame::process_entities_mut(
        const EntityId* const* SGE_RESTRICT user_start_iters,
        const EntityId* const* SGE_RESTRICT user_end_iters,
        std::size_t num_user_iters,
        const TypeInfo* const* types,
        std::size_t num_types,
        FunctionView<ProcessMutFn> process_fn)
    {
        if (num_user_iters == 0)
        {
            return;
        }

        impl_process_entities(user_start_iters, user_end_iters, num_user_iters, types, num_types, process_fn);
    }

    void SystemFrame::append_tags(const TypeInfo& tag_type, TagBuffer tag_buffer)
    {
        _has_tags = true;
        _tags[&tag_type].push_back(std::move(tag_buffer));
    }

	template <typename ProcessFnT>
	void SystemFrame::impl_process_entities(
		const EntityId* const* SGE_RESTRICT user_start_iters,
        const EntityId* const* SGE_RESTRICT user_end_iters,
        const std::size_t num_user_iters,
		const TypeInfo* const types[],
		const std::size_t num_types,
		ProcessFnT& process_fn)
	{
        // Create arrays for entity iterators
        auto* const instance_iters = SGE_STACK_ALLOC(ComponentContainer::InstanceIterator, num_types + num_user_iters);
        auto* const end_iters = SGE_STACK_ALLOC(ComponentContainer::InstanceIterator, num_types + num_user_iters);

        // Create an array of component containers
        auto* const containers = SGE_STACK_ALLOC(ComponentContainer*, num_types);

        // Create the array of component interfaces
        auto** const comp_interfaces = SGE_STACK_ALLOC(ComponentInterface*, num_types);

        // Get all of the component containers, iterators, and interfaces
        for (std::size_t i = 0; i < num_types; ++i)
        {
            // Make sure the type exists in the scene
            auto cont_iter = _scene_data->components.find(types[i]);
            if (cont_iter == _scene_data->components.end())
            {
                // Destroy existing interfaces
                for (; i > 0; --i)
                {
                    comp_interfaces[i - 1]->~ComponentInterface();
                }

                return;
            }

            // Create the interface
            comp_interfaces[i] = reinterpret_cast<ComponentInterface*>(SGE_STACK_ALLOC(byte, types[i]->size()));
            types[i]->init(comp_interfaces[i]);

            // Create the iterators
            instance_iters[i] = cont_iter->second->get_start_iterator();
            end_iters[i] = cont_iter->second->get_end_iterator();

            // Create the component container pointer
            containers[i] = cont_iter->second.get();
        }

	    // Add the user-supplied iterators
        for (std::size_t i = 0; i < num_user_iters; ++i)
        {
            instance_iters[num_types + i] = user_start_iters[i];
            end_iters[num_types + i] = user_end_iters[i];
        }

        // Create a processing frame
        ProcessingFrame pframe;
        pframe._user_iterators = instance_iters + num_types;
        pframe._user_start_iterators = user_start_iters;
        pframe._user_end_iterators = user_end_iters;
        pframe._num_user_iterators = num_user_iters;

        // Search for matches
        for (std::size_t frame_index = 0, iteration_index = 0; true; ++frame_index)
        {
            // Find the next match
            auto entity = ord_entity_union(instance_iters, end_iters, num_types + num_user_iters);

            // If there are no more, quit searching
            if (entity == NULL_ENTITY)
            {
                break;
            }

            // Advance the interfaces
            for (std::size_t i = 0; i < num_types; ++i)
            {
                // Call custom 'reset' function before ComponentInterface one, to allow interfaces to access old entity data
                containers[i]->reset_interface(instance_iters[i], comp_interfaces[i]);
                comp_interfaces[i]->reset(instance_iters[i]);
            }

            // Set up the processing frame
            pframe._entity = entity;
            pframe._frame_index = frame_index;

            // Call the processing function
            auto control = process_fn(pframe, comp_interfaces);
            if (control == ProcessControl::BREAK)
            {
                break;
            }

            // Increment iterators (so we don't hit the same match again)
            for (std::size_t i = 0; i < num_types + num_user_iters; ++i)
            {
                ++instance_iters[i];
            }
        }

        // Generate tags and destroy interfaces
        for (std::size_t i = 0; i < num_types; ++i)
        {
            _has_tags = true;

            // Insert the destroyed components
            insert_ord_entities(
                _ord_destroyed_components[types[i]],
                comp_interfaces[i]->_ord_destroyed.data(),
                comp_interfaces[i]->_ord_destroyed.size());

            // Insert other tags
            comp_interfaces[i]->generate_tags(_tags);

            // Destroy the interface
            comp_interfaces[i]->~ComponentInterface();
        }
	}
}
