// CCubeCollider.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../../Component.h"

namespace sge
{
	struct SGE_ENGINE_API CCubeCollider : TComponentInterface<CCubeCollider>
	{
		SGE_REFLECTED_TYPE;
		struct Data;

		////////////////
		///   Tags   ///
	public:

		struct SGE_ENGINE_API FShapeChanged
		{
			SGE_REFLECTED_TYPE;
		};

		////////////////////
		////   Methods   ///
	public:

		Vec2 shape() const;

		void shape(Vec2 shape);

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
	};
}
