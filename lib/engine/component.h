#pragma once

#include "lib/base/containers/fixed_string.h"
#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/engine/node.h"

namespace sge
{
    struct Scene;
    struct SceneData;
    struct SystemFrame;

    class SGE_ENGINE_API ComponentContainer
    {
    public:
        virtual ~ComponentContainer() = default;

        virtual const TypeInfo& get_component_type() const = 0;

        virtual void reset() = 0;

        virtual void to_archive(ArchiveWriter& writer) const = 0;

        virtual void from_archive(ArchiveReader& reader) = 0;

        virtual void on_end_system_frame() = 0;

        virtual void on_end_update_frame() = 0;

        virtual void create_instances(const Node* const* nodes, size_t num_instances, void** out_instances) = 0;

        virtual void remove_instances(const NodeId* nodes, size_t num_instances) = 0;

        virtual void get_instances(const NodeId* nodes, size_t num_instances, void** out_instances) = 0;

        virtual size_t num_instance_nodes() const = 0;

        virtual size_t get_instance_nodes(size_t start_index, size_t num_instances, size_t* out_num_instances, NodeId* out_instance_nodes) const = 0;

        virtual EventChannel* get_event_channel(const char* name) = 0;

        template <class T>
        void create_instances(const NodeId* nodes, size_t num_instances, T** out_instances)
        {
            return this->create_instances(nodes, num_instances, reinterpret_cast<void**>(out_instances));
        }

        template <class T>
        void get_instances(const NodeId* nodes, size_t num_instances, T** out_instances)
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
        FixedString<24> property;
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
