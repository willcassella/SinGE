// Component.cpp

#include <Core/Reflection/ReflectionBuilder.h>
#include "../include/Engine/Scene.h"
#include "../include/Engine/ProcessingFrame.h"
#include "../include/Engine/TagBuffer.h"
#include "../include/Engine/Components/CTransform3D.h"
#include "../include/Engine/Components/Display/CCamera.h"
#include "../include/Engine/Components/Display/CStaticMesh.h"
#include "../include/Engine/Components/Display/CSpotlight.h"
#include "../include/Engine/Components/Display/CLightColor.h"
#include "../include/Engine/Components/Display/CLightMaskReceiver.h"
#include "../include/Engine/Components/Display/CLightMaskObstructor.h"
#include "../include/Engine/Components/Gameplay/CInput.h"
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
	ComponentInterface::ComponentInterface()
        : _entity(NULL_ENTITY),
        _modified_current(false),
        _destroyed_current(false)
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

    void ComponentInterface::destroy()
    {
        if (!_destroyed_current)
        {
            _ord_destroyed.push_back(_entity);
            _destroyed_current = true;
        }
    }

    void ComponentInterface::set_modified()
    {
        if (!_modified_current)
        {
            _ord_modified.push_back(_entity);
            _modified_current = true;
        }
    }

    void ComponentInterface::generate_tags(std::map<const TypeInfo*, std::vector<TagBuffer>>& tags)
    {
        if (_ord_modified.empty())
        {
            return;
        }

        // Generate modified tags
        if (!_ord_modified.empty())
        {
            tags[&FModifiedComponent::type_info].push_back(TagBuffer::create_single_empty(
                get_type(),
                _ord_modified.data(),
                _ord_modified.size()));
        }
    }

    void ComponentInterface::reset(EntityId entity)
    {
        _entity = entity;
        _modified_current = false;
        _destroyed_current = false;
    }

    void register_builtin_components(Scene& scene)
	{
		CTransform3D::register_type(scene);
		CStaticMesh::register_type(scene);
		CPerspectiveCamera::register_type(scene);
		CSpotlight::register_type(scene);
		CLightColor::register_type(scene);
        CLightMaskReceiver::register_type(scene);
        CLightMaskObstructor::register_type(scene);
        CInput::register_type(scene);
        CBoxCollider::register_type(scene);
        CVelocity::register_type(scene);
        CRigidBody::register_type(scene);
        CSensor::register_type(scene);
        CCharacterController::register_type(scene);
        CCapsuleCollider::register_type(scene);
	}
}
