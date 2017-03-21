// CInput.h
#pragma once

#include <Core/Containers/FixedString.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CInput
	{
		SGE_REFLECTED_TYPE;
		struct SharedData;

		////////////////////////
		///   Constructors   ///
	public:

		explicit CInput(NodeId node, SharedData& shared_data);

		////////////////
		///   Tags   ///
	public:

        using EventName_t = FixedString<32>;

		/* Input event dispatched when an 'action' occurs. */
		struct EAction
		{
			NodeId input_node;
			EventName_t name;
		};

		/* Input event dispatched when an input axis changes. */
		struct EAxis
		{
			///////////////////
			///   Methods   ///
		public:

            float axis_half() const
            {
                return min + (max - min) * 0.5f;
            }

            float norm_axis_half() const
            {
                return axis_half() / (max - axis_half());
            }

            float distance_from_half() const
            {
                return value - axis_half();
            }

            float norm_distance_from_half() const
            {
                return distance_from_half() / (max - axis_half());
            }

			//////////////////
			///   Fields   ///
		public:

			NodeId input_node;
			EventName_t name;
			float value;
            float min;
            float max;
		};

        ///////////////////
        ///   Methods   ///
	public:

        static void register_type(Scene& scene);

		void to_archive(ArchiveWriter& writer) const;

		void from_archive(ArchiveReader& reader);

		NodeId node() const;

        void add_action_event(EventName_t action_name) const;

        void add_axis_event(EventName_t axis_name, float value, float min, float max) const;

        //////////////////
        ///   Fields   ///
	private:

		NodeId _node;
		SharedData* _shared_data = nullptr;
	};
}
