// CSharedData.h
#pragma once

#include <vector>
#include "../Component.h"
#include "../EventChannel.h"

namespace sge
{
	template <class ComponentT>
	struct CSharedData
	{
		////////////////////////
		///   Constructors   ///
	public:

		CSharedData()
			: modified_instance_channel(sizeof(EModifiedComponent<ComponentT>), 8)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void reset()
		{
			modified_instances.clear();
			modified_instance_channel.clear();
		}

		void on_end_system_frame()
		{
			// Add events to the channel
			modified_instance_channel.append(
				modified_instances.data(),
				sizeof(EModifiedComponent<ComponentT>),
				(int32)modified_instances.size());

			modified_instances.clear();
		}

		void on_end_update_frame()
		{
			modified_instance_channel.clear();
		}

		EventChannel* get_event_channel(const char* name)
		{
			if (std::strcmp(name, "prop_mod") == 0)
			{
				return &modified_instance_channel;
			}
			else
			{
				return nullptr;
			}
		}

		void set_modified(NodeId node, ComponentT* instance, const char* prop_name)
		{
			EModifiedComponent<ComponentT> event;
			event.node = node;
			event.instance = instance;
			event.property = prop_name;

			modified_instances.push_back(event);
		}

		//////////////////
		///   Fields   ///
	public:

		std::vector<EModifiedComponent<ComponentT>> modified_instances;
		EventChannel modified_instance_channel;
	};
}
