// CInput.h
#pragma once

#include <Core/Containers/FixedString.h>
#include "../../Util/TagStorage.h"

namespace sge
{
	class SGE_ENGINE_API CInput final : public TComponentInterface<CInput>
	{
	public:

		SGE_REFLECTED_TYPE;

		////////////////
		///   Tags   ///
	public:

        using Name_t = FixedString<32>;

		/* Input event dispatched when an 'action' occurs. */
		struct SGE_ENGINE_API FActionEvent
		{
			SGE_REFLECTED_TYPE;

			//////////////////
			///   Fields   ///
		public:

			Name_t name;
		};

		/* Input event dispatched when an input axis changes. */
		struct SGE_ENGINE_API FAxisEvent
		{
			SGE_REFLECTED_TYPE;

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

			Name_t name;
			float value;
            float min;
            float max;
		};

        ///////////////////
        ///   Methods   ///
	public:

        static void register_type(Scene& scene);

        void reset();

        void add_action_event(Name_t action_name) const;

        void add_axis_event(Name_t axis_name, float value, float min, float max) const;

	private:

        void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags) override;

        //////////////////
        ///   Fields   ///
	private:

        mutable TagStorage<FActionEvent> _action_tags;
        mutable TagStorage<FAxisEvent> _axis_tags;
	};
}
