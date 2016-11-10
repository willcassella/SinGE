// ProcessingFrame.h
#pragma once

#include <atomic>
#include <Core/Reflection/Reflection.h>
#include "Component.h"

namespace sge
{
	struct Scene;

	struct ProcessingFrame
	{
		static constexpr std::size_t TAG_BUFFER_SIZE = 1024;

		////////////////////////
		///   Constructors   ///
	public:

		ProcessingFrame(const Scene& scene);
		~ProcessingFrame();

		///////////////////
		///   Methods   ///
	public:

		void* create_tag(ComponentId component, const TypeInfo& tagType);

		template <typename T>
		T* create_tag(ComponentId component)
		{
			return static_cast<T*>(create_tag(component, sge::get_type<T>()));
		}

		//////////////////
		///   Fields   ///
	private:

		std::size_t _free_offset;
		byte _tag_buffer[TAG_BUFFER_SIZE];
	};

	struct ProcessingFrameMut : ProcessingFrame
	{
		static constexpr std::size_t COMPONENT_BUFFER_SIZE = 256;

		////////////////////////
		///   Constructors   ///
	public:

		ProcessingFrameMut(Scene& scene);

		///////////////////
		///   Methods   ///
	public:

		ComponentInterface* new_component(EntityId entity, const TypeInfo& type);

		template <class C>
		C* new_component(EntityId entity)
		{
			return static_cast<C*>(new_component(entity, sge::get_type<C>()));
		}

		//////////////////
		///   Fields   ///
	public:

		Scene* _scene;

		std::size_t _component_interface_free_offset;
		byte _new_component_interfaces[COMPONENT_BUFFER_SIZE];

		std::size_t _new_component_next;
		ComponentId _new_components[COMPONENT_BUFFER_SIZE];
	};
}
