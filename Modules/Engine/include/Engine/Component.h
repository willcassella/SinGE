// Component.h
#pragma once

#include <Core/Containers/FixedString.h>
#include <Core/Interfaces/IToArchive.h>
#include <Core/Interfaces/IFromArchive.h>
#include "Node.h"

namespace sge
{
	struct Scene;
	struct SceneData;
    struct SystemFrame;

    class SGE_ENGINE_API ComponentContainer
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        virtual ~ComponentContainer() = default;

        ///////////////////
        ///   Methods   ///
    public:

        virtual const TypeInfo& get_component_type() const = 0;

        virtual void reset() = 0;

        virtual void to_archive(ArchiveWriter& writer) const = 0;

        virtual void from_archive(ArchiveReader& reader) = 0;

        virtual void on_end_system_frame() = 0;

        virtual void on_end_update_frame() = 0;

        virtual void create_instances(const Node* const* nodes, std::size_t num_instances, void** out_instances) = 0;

        virtual void remove_instances(const NodeId* nodes, std::size_t num_instances) = 0;

		virtual void get_instances(const NodeId* nodes, std::size_t num_instances, void** out_instances) = 0;

		virtual std::size_t num_instance_nodes() const = 0;

		virtual std::size_t get_instance_nodes(std::size_t start_index, std::size_t num_instances, std::size_t* out_num_instances, NodeId* out_instance_nodes) const = 0;

        virtual EventChannel* get_event_channel(const char* name) = 0;

		template <class T>
		void create_instances(const NodeId* nodes, std::size_t num_instances, T** out_instances)
		{
			return this->create_instances(nodes, num_instances, reinterpret_cast<void**>(out_instances));
		}

		template <class T>
		void get_instances(const NodeId* nodes, std::size_t num_instances, T** out_instances)
		{
			return this->get_instances(nodes, num_instances, reinterpret_cast<void**>(out_instances));
		}
    };

	/**
	 * \brief Event generated for new component objects.
	 */
	struct ENewComponent
	{
        NodeId node;
        void* instance = nullptr;
	};

	/**
	 * \brief Event generated for destroyed component objects.
	 */
	struct EDestroyedComponent
	{
        NodeId node;
        void* instance = nullptr;
	};

	/**
	 * \brief Event generated for modified component objects.
	 */
	struct EModifiedComponent
	{
        NodeId node;
        void* instance = nullptr;
		FixedString<16> property;
	};

	/**
	 * \brief Event generated when a specific component property is modified.
	 */
	struct EModifiedComponentProperty
	{
		NodeId node;
		void* instance;
	};

	/**
	 * \brief Registers all builtin engine component types.
	 * \param scene The scene to register the component types with.
	 */
	SGE_ENGINE_API void register_builtin_components(Scene& scene);
}
