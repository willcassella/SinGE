// CInput.h
#pragma once

#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CInput
	{
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
	};
}
