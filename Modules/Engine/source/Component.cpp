// Component.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/ProcessingFrame.h"
#include "../include/Engine/Components/CTransform3D.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"
#include "../include/Engine/Components/Display/CSpotlight.h"
#include "../include/Engine/Components/Display/CLightColor.h"
#include "../include/Engine/Components/Logic/CInput.h"
#include "../include/Engine/Components/Physics/CBoxCollider.h"
#include "../include/Engine/Components/Physics/CVelocity.h"
#include "../include/Engine/Components/Physics/CRigidBody.h"
#include "../include/Engine/Components/Physics/CSensor.h"
#include "../include/Engine/Components/Gameplay/CCharacterController.h"
#include "../include/Engine/Components/Physics/CCapsuleCollider.h"

SGE_REFLECT_TYPE(sge::ComponentId);
SGE_REFLECT_TYPE(sge::FNewComponent);
SGE_REFLECT_TYPE(sge::FDestroyedComponent);
SGE_REFLECT_TYPE(sge::FModifiedComponent);

namespace sge
{
	ComponentInterface::ComponentInterface(ProcessingFrame& pframe, EntityId entity)
		: _pframe(&pframe),
	    _entity(entity),
	    _applied_modified_tag(false)
	{
	}

	void ComponentInterface::from_property_archive(ArchiveReader& reader)
	{
		const auto& type = this->get_type();

		reader.enumerate_object_members([this, &type, &reader](const char* propName)
		{
			const auto* prop = type.find_property(propName);
			if (prop)
			{
				prop->mutate(this, [&reader](AnyMut<> value)
				{
					const auto* fromArchive = sge::get_vtable<IFromArchive>(value.type());
					if (fromArchive)
					{
						fromArchive->from_archive(value.object(), reader);
					}
				});
			}
		});
	}

    void ComponentInterface::create_tag(const TypeInfo& tag_type, void* tag) const
    {
        processing_frame().create_tag(id(), tag_type, tag);
    }

    void ComponentInterface::apply_component_modified_tag()
	{
		if (!_applied_modified_tag)
		{
			_pframe->create_tag(id(), FModifiedComponent{});
			_applied_modified_tag = true;
		}
	}

	void register_builtin_components(Scene& scene)
	{
		CTransform3D::register_type(scene);
		CStaticMesh::register_type(scene);
		CPerspectiveCamera::register_type(scene);
		CSpotlight::register_type(scene);
		CLightColor::register_type(scene);
        CInput::register_type(scene);
        CBoxCollider::register_type(scene);
        CVelocity::register_type(scene);
        CRigidBody::register_type(scene);
        CSensor::register_type(scene);
        CCharacterController::register_type(scene);
        CCapsuleCollider::register_type(scene);
	}
}
