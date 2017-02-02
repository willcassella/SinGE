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
    static ComponentContainer::InstanceIterator instance_union(
        ComponentContainer::InstanceIterator primary_iter,
        const ComponentContainer::InstanceIterator primary_iter_end,
        ComponentContainer::InstanceIterator* const iters,
        const ComponentContainer::InstanceIterator* const end_iters,
        const std::size_t num_secondary)
    {
        // Make sure we haven't reached the end
        if (primary_iter == primary_iter_end)
        {
            return primary_iter_end;
        }

        // For each secondary iterator
        for (std::size_t i = 0; i < num_secondary;)
        {
            while (true)
            {
                // If we've reached the end of this iterator
                if (iters[i] == end_iters[i])
                {
                    return primary_iter_end;
                }

                // If the iterator is less than the primary iterator (most likely case)
                if (*iters[i] < *primary_iter)
                {
                    // Keep incrementing
                    ++iters[i];
                    continue;
                }

                // If the iter is equal to the primary
                if (*iters[i] == *primary_iter)
                {
                    // Go on to the next iterator
                    ++i;
                    break;
                }

                // The iterator must be greater than the target
                // Start over
                i = 0;
                ++primary_iter;

                // If we've reached the end of the primary iterator
                if (primary_iter == primary_iter_end)
                {
                    return primary_iter_end;
                }

                break;
            }
        }

        return primary_iter;
    }

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

        // Make sure the children are valid
        for (std::size_t i = 0; i < num_children; ++i)
        {
            if (ord_children[i] == NULL_ENTITY || ord_children[i] >= _scene_data->next_entity_id)
            {
                return;
            }
        }

        // Set the parents
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
        impl_process_entities(nullptr, 0, types, num_types, process_fn);
	}

    void SystemFrame::process_entities_mut(
        const TypeInfo* const types[],
        std::size_t num_types,
        FunctionView<ProcessMutFn> process_fn)
    {
        impl_process_entities(nullptr, 0, types, num_types, process_fn);
    }

	void SystemFrame::process_entities(
        const EntityId* ord_entities,
        std::size_t num_entities,
        const TypeInfo* const types[],
        std::size_t num_types,
        FunctionView<ProcessFn> process_fn)
	{
        if (num_entities == 0)
        {
            return;
        }

		impl_process_entities(ord_entities, num_entities, types, num_types, process_fn);
	}

    void SystemFrame::process_entities_mut(
        const EntityId* ord_entities,
        std::size_t num_entities,
        const TypeInfo*
        const types[],
        std::size_t num_types,
        FunctionView<ProcessMutFn> process_fn)
    {
        if (num_entities == 0)
        {
            return;
        }

        impl_process_entities(ord_entities, num_entities, types, num_types, process_fn);
    }

    void SystemFrame::append_tags(const TypeInfo& tag_type, TagBuffer tag_buffer)
    {
        _has_tags = true;
        _tags[&tag_type].push_back(std::move(tag_buffer));
    }

	template <typename ProcessFnT>
	void SystemFrame::impl_process_entities(
		const EntityId* ord_entities,
        const std::size_t num_entities,
		const TypeInfo* const types[],
		const std::size_t num_types,
		ProcessFnT& process_fn)
	{
        if (num_types == 0)
        {
            return;
        }

        // Create the array of component interfaces
        auto** const comp_interfaces = SGE_STACK_ALLOC(ComponentInterface*, num_types);

        // Create arrays for component iterators
        auto* const instance_iters = SGE_STACK_ALLOC(ComponentContainer::InstanceIterator, num_types);
        auto* const end_iters = SGE_STACK_ALLOC(ComponentContainer::InstanceIterator, num_types);

        // Create an array of component containers
        auto* const containers = SGE_STACK_ALLOC(ComponentContainer*, num_types);

        // Fill the interface array
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

        // Create a processing frame
        ProcessingFrame pframe;

        // Create the iterators
        const EntityId** primary_iter;
        const EntityId* primary_iter_end;
        const EntityId** secondary_iters;
        const EntityId** secondary_end_iters;
        std::size_t num_secondary_iters;

        // If we were given an ordered array of entities
        if (ord_entities != nullptr)
        {
            primary_iter = &ord_entities;
            primary_iter_end = ord_entities + num_entities;
            secondary_iters = instance_iters;
            secondary_end_iters = end_iters;
            num_secondary_iters = num_types;
        }
        else
        {
            primary_iter = &instance_iters[0];
            primary_iter_end = end_iters[0];
            secondary_iters = instance_iters + 1;
            secondary_end_iters = end_iters + 1;
            num_secondary_iters = num_types - 1;
        }

        // Search for matches
        for (std::size_t frame_index = 0, iteration_index = 0; true; ++frame_index)
        {
            // Find the next match
            auto advanced_iter = instance_union(*primary_iter, primary_iter_end, secondary_iters, secondary_end_iters, num_secondary_iters);
            iteration_index += advanced_iter - *primary_iter;
            *primary_iter = advanced_iter;

            // If there are no more, quit searching
            if (*primary_iter == primary_iter_end)
            {
                break;
            }

            // Advance the interfaces
            for (std::size_t i = 0; i < num_types; ++i)
            {
                comp_interfaces[i]->reset(instance_iters[i]);
                containers[i]->reset_interface(instance_iters[i], comp_interfaces[i]);
            }

            // Set up the processing frame
            pframe._entity = **primary_iter;
            pframe._frame_index = frame_index;
            pframe._iteration_index = iteration_index;

            // Call the processing function
            auto control = process_fn(pframe, comp_interfaces);
            if (control == ProcessControl::BREAK)
            {
                break;
            }

            // Increment the primary iterator (to prevent matching the same entity again)
            ++*primary_iter;
            ++iteration_index;
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
