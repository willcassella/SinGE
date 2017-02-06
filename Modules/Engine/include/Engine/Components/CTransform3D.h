// CTransform3D.h
#pragma once

#include <Core/Math/Mat4.h>
#include "../Util/TagStorage.h"

namespace sge
{
	class SGE_ENGINE_API CTransform3D final : public TComponentInterface<CTransform3D>
	{
	public:

		SGE_REFLECTED_TYPE;
		struct Data;

        struct SGE_ENGINE_API FParentChanged
        {
            SGE_REFLECTED_TYPE;
        };

		///////////////////
		///   Methods   ///
	public:

		static void register_type(Scene& scene);

        void reset(Data& data, std::map<EntityId, Data>& cont_data);

		bool has_parent() const;

		TComponentId<CTransform3D> get_parent() const;

		void set_parent(TComponentId<CTransform3D> parent);

        std::vector<EntityId> get_children() const;

		Vec3 get_local_position() const;

		void set_local_position(Vec3 pos);

		Vec3 get_local_scale() const;

		void set_local_scale(Vec3 scale);

		Quat get_local_rotation() const;

		void set_local_rotation(Quat rot);

		Mat4 get_local_matrix() const;

		Vec3 get_world_position() const;

		Vec3 get_world_scale() const;

		Quat get_world_rotation() const;

		Mat4 get_world_matrix() const;

	private:

        void generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags) override;

		Mat4 get_parent_matrix() const;

		//////////////////
		///   Fields   ///
	private:

		Data* _data = nullptr;
        const std::map<EntityId, Data>* _data_map = nullptr;
        TagStorage<FParentChanged> _parent_changed_tags;
	};
}
