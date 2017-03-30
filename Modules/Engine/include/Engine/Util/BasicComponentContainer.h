// BasicComponentContainer.h
#pragma once

#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <Core/Interfaces/IFromString.h>
#include <Core/Memory/Buffers/MultiStackBuffer.h>
#include "../Component.h"

namespace sge
{
    template <class ComponentT, typename SharedDataT>
    class BasicComponentContainer final : public ComponentContainer
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        BasicComponentContainer()
            : _new_instance_channel(sizeof(ENewComponent), 8),
            _destroyed_instance_channel(sizeof(EDestroyedComponent), 8)
            {
            }

            ///////////////////
            ///   Methods   ///
    public:

        const TypeInfo& get_component_type() const override
        {
            return sge::get_type<ComponentT>();
        }

        void reset() override
        {
			_shared_data.reset();
            _new_instance_channel.clear();
            _destroyed_instance_channel.clear();
            _instance_map.clear();
            _instance_buffer.clear();
        }

        void to_archive(ArchiveWriter& writer) const override
        {
            char id_str[20];

            for (auto instance : _instance_map)
            {
				instance.first.to_string(id_str, 20);
                writer.push_object_member(id_str);
                instance.second->to_archive(writer);
                writer.pop();
            }
        }

        void from_archive(ArchiveReader& reader) override
        {
            reset();

			std::vector<ENewComponent> new_instances;

            reader.enumerate_object_members([this, &reader, &new_instances](const char* id_str)
            {
                // Get the node id
				NodeId node;
				node.from_string(id_str);

				// Make sure it's valid
				if (node.is_null())
				{
					return;
				}

				// Make sure it doesn't already exist in the map
				const auto node_iter = this->_instance_map.find(node);
				if (node_iter != this->_instance_map.end())
				{
					return;
				}

				// Allocate space for the entity
				auto* const buff = this->_instance_buffer.alloc(sizeof(ComponentT));
				auto* const instance = new(buff) ComponentT(node, _shared_data);

				// Insert it into the map
				this->_instance_map[node] = instance;
				this->_instance_nodes.push_back(node);

				// Deserialize it
				instance->from_archive(reader);

				// Create the new instance event
				ENewComponent event;
				event.node = node;
				event.instance = instance;
				new_instances.push_back(event);
			});

			// Append all new instance events
			_new_instance_channel.append(
				new_instances.data(),
				sizeof(ENewComponent),
				(int32)new_instances.size());
		}

		void on_end_system_frame() override
		{
			_shared_data.on_end_system_frame();
		}

		void on_end_update_frame() override
		{
			// TODO: Destroy actual instances
			_shared_data.on_end_update_frame();
			_new_instance_channel.clear();
			_destroyed_instance_channel.clear();
		}

		void create_instances(const NodeId* nodes, std::size_t num_nodes, void** out_instances) override
		{
			std::vector<ENewComponent> new_instances;
			new_instances.reserve(num_nodes);

			for (std::size_t i = 0; i < num_nodes; ++i)
			{
				const auto node = nodes[i];

				// Make sure the instance doesn't already exist
				const auto iter = _instance_map.find(node);
				if (iter != _instance_map.end())
				{
					out_instances[i] = nullptr;
					continue;
				}

				// Construct the instance
				void* const buff = _instance_buffer.alloc(sizeof(ComponentT));
				auto* const instance = new (buff) ComponentT(node, _shared_data);
				out_instances[i] = instance;

				// Add it to the map
				_instance_map[node] = instance;

				// Add it to the instance node list
				_instance_nodes.push_back(node);

				// Create the new instance event
				ENewComponent event;
				event.node = node;
				event.instance = instance;
				new_instances.push_back(event);
			}

			// Create the new instance events
			_new_instance_channel.append(
				new_instances.data(),
				sizeof(ENewComponent),
				(int32)new_instances.size());
		}

		void remove_instances(const NodeId* nodes, std::size_t num_nodes) override
		{
			std::vector<EDestroyedComponent> destroyed_events;
			destroyed_events.reserve(num_nodes);

			// Figure out which instances of the given nodes actually have these components
			for (std::size_t i = 0; i < num_nodes; ++i)
			{
				const auto node = nodes[i];
				const auto iter = _instance_map.find(node);
				if (iter == _instance_map.end())
				{
					continue;
				}

				// Create the destroyed event
				EDestroyedComponent event;
				event.node = node;
				event.instance = iter->second;

				destroyed_events.push_back(event);
			}

			// Add all events
			_destroyed_instance_channel.append(
				destroyed_events.data(),
				sizeof(EDestroyedComponent),
				(int32)destroyed_events.size());
		}

		void get_instances(const NodeId* nodes, std::size_t num_instances, void** out_instances)
		{
			for (std::size_t i = 0; i < num_instances; ++i)
			{
				const auto node = nodes[i];

				// Search for the id
				const auto iter = _instance_map.find(node);
				if (iter == _instance_map.end())
				{
					out_instances[i] = nullptr;
					continue;
				}

				out_instances[i] = iter->second;
			}
		}

		std::size_t num_instance_nodes() const override
		{
			return _instance_nodes.size();
		}

		std::size_t get_instance_nodes(
			std::size_t start_index,
			std::size_t num_instances,
			std::size_t* out_num_instances,
			NodeId* out_instances) const override
		{
			if (start_index >= _instance_nodes.size())
			{
				*out_num_instances = 0;
				return 0;
			}

			const auto num_copy = std::min(_instance_nodes.size() - start_index, num_instances);
			std::memcpy(out_instances, _instance_nodes.data() + start_index, num_copy * sizeof(NodeId));
			*out_num_instances = num_copy;
			return num_copy;
		}

		EventChannel* get_event_channel(const char* name) override
		{
			if (std::strcmp(name, "new") == 0)
			{
				return &_new_instance_channel;
			}
			else if (std::strcmp(name, "destroy") == 0)
			{
				return &_destroyed_instance_channel;
			}
			else
			{
				return _shared_data.get_event_channel(name);
			}
		}

        //////////////////
        ///   Fields   ///
    private:

		SharedDataT _shared_data;
        EventChannel _new_instance_channel;
        EventChannel _destroyed_instance_channel;
        std::map<NodeId, ComponentT*> _instance_map;
		std::vector<NodeId> _instance_nodes;
		MultiStackBuffer _instance_buffer;
    };
}
