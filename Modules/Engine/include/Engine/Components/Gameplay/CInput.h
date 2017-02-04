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

		/* Input event dispatched when an 'action' occurs. */
		struct SGE_ENGINE_API FActionEvent
		{
			SGE_REFLECTED_TYPE;

			//////////////////
			///   Fields   ///
		public:

			FixedString<32> name;
		};

		/* Input event dispatched when an input axis changes. */
		struct SGE_ENGINE_API FAxisEvent
		{
			SGE_REFLECTED_TYPE;

			//////////////////
			///   Fields   ///
		public:

			FixedString<32> name;
			float value;
            float max;
		};

        /* Tag to set the value of the specified axis. */
        struct SGE_ENGINE_API FSetAxis
        {
            SGE_REFLECTED_TYPE;

            //////////////////
            ///   Fields   ///
        public:

            FixedString<32> name;
            float value;
        };

        ///////////////////
        ///   Methods   ///
	public:

        static void register_type(Scene& scene);

        void reset();

        void add_action_event(const FActionEvent& action_event) const;

        void add_axis_event(const FAxisEvent& axis_event) const;

        void set_axis(const FSetAxis& set_axis) const;

	private:

        void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags) override;

        //////////////////
        ///   Fields   ///
	private:

        mutable TagStorage<FActionEvent> _action_tags;
        mutable TagStorage<FAxisEvent> _axis_tags;
        mutable TagStorage<FSetAxis> _set_axis_tags;
	};
}
