// CInput.h
#pragma once

#include "../../Component.h"

namespace sge
{
	class SGE_ENGINE_API CInput : public TComponentInterface<CInput>
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

			std::string name;
		};

		/* Input event dispatched when an input axis changes. */
		struct SGE_ENGINE_API FAxisEvent
		{
			SGE_REFLECTED_TYPE;

			//////////////////
			///   Fields   ///
		public:

			std::string name;
			float value;
		};

        ////////////////////////
        ///   Constructors   ///
	public:

        CInput(ProcessingFrame& pframe, EntityId entity);

        ///////////////////
        ///   Methods   ///
	public:

        static void register_type(Scene& scene);
	};
}
