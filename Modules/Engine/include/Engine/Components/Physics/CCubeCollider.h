// CCubeCollider.h
#pragma once

#include <Core/Math/Vec2.h>
#include "../../Component.h"

namespace sge
{
	class SGE_ENGINE_API CCubeCollider final : public TComponentInterface<CCubeCollider>
	{
	public:

		SGE_REFLECTED_TYPE;
		struct Data;

        ////////////////////////
        ///   Constructors   ///
	public:

        CCubeCollider(ProcessingFrame& pframe, EntityId entity, Data& data);

		////////////////////
		////   Methods   ///
	public:

        static void register_type(Scene& scene);

		float width() const;

        void width(float value);

        float height() const;

        void height(float value);

		//////////////////
		///   Fields   ///
	private:

		Data* _data;
	};
}
