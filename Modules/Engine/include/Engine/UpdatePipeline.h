// UpdatePipeline.h
#pragma once

#include <Core/Functional/UFunction.h>
#include "Component.h"

namespace sge
{
    struct Scene;
    struct SceneData;
    struct SystemFrame;
    struct TagCallback;

    struct SGE_ENGINE_API UpdatePipeline
    {
        SGE_REFLECTED_TYPE;

        friend Scene;

        /**
        * \brief Function signature used for system funtions during the read phase. This design may change in the future.
        * \param frame The frame to be used by the system function.
        * \param current_time The current game time.
        * \param dt The time delta since the last frame.
        */
        using SystemFn = void(SystemFrame& frame, float current_time, float dt);

        using TagCallbackFn = void(
            SystemFrame& frame,
            const TypeInfo& tag_type,
            const TypeInfo& component_type,
            const void* tags,
            const EntityId* component_instances,
            std::size_t num_tags);

        using SystemToken = uint16;

        using PipelineStep = std::vector<UFunction<SystemFn>>;

        using Pipeline = std::vector<PipelineStep>;

        /////////////////////////
        ///   Constructors    ///
    public:

        UpdatePipeline() = default;
        UpdatePipeline(const UpdatePipeline& copy) = delete;
        UpdatePipeline& operator=(const UpdatePipeline& copy) = delete;
        UpdatePipeline(UpdatePipeline&& move) = default;
        UpdatePipeline& operator=(UpdatePipeline&& move) = default;

        ///////////////////
        ///   Methods   ///
    public:

        void configure_pipeline(ArchiveReader& reader);

        const Pipeline& get_pipeline() const;

        /**
        * \brief Registers a system function to be called during the read phase.
        * This design may change in the future.
        * \param system_fn The system function to call during the read phase.
        * \return The token for the system function, which may be used to unregister it.
        */
        void register_system_fn(std::string name, UFunction<SystemFn> system_fn);

        /**
        * \brief Registers a system member function to be called during the read phase.
        * This design may change in the future.
        * \param outer The system object that the member function is to be called on.
        * \param system_fn The system member function to call during the read phase.
        * \return The token for the system function, which may be used to unregister it.
        */
        template <typename T>
        void register_system_fn(
            std::string name,
            T* outer,
            void(T::*system_fn)(SystemFrame& frame, float current_time, float dt))
        {
            this->register_system_fn(std::move(name), [outer, system_fn]
                (SystemFrame& frame, float current_time, float dt)
            {
                (outer->*system_fn)(frame, current_time, dt);
            });
        }

        /**
        * \brief Registers a callback for when the given tag type is applied to any component.
        * \param tag_type The type of tag to be called for.
        * \param callback The callback to call.
        */
        void register_tag_callback(
            const TypeInfo& tag_type,
            UFunction<TagCallbackFn> callback);

        /**
        * \brief Registers a callback for when the given tag type is applied to any component of the given type.
        * \param tag_type The type of tag to be called for.
        * \param component_type The type of component to be called for.
        * \param callback The callback to call.
        */
        void register_tag_callback(
            const TypeInfo& tag_type,
            const TypeInfo& component_type,
            UFunction<TagCallbackFn> callback);

        /**
        * \brief Registers a callback for when the given tag type is applied to the given component instance.
        * \param tag_type The type of tag to be called for.
        * \param component The component instance to be called for.
        * \param callback The callback to call.
        */
        void register_tag_callback(
            const TypeInfo& tag_type,
            ComponentId component,
            UFunction<TagCallbackFn> callback);

        template <typename TagT, class OuterT>
        void register_tag_callback(
            OuterT* outer,
            void(OuterT::*callback)(
                SystemFrame& frame,
                const TypeInfo& component_type,
                const TagT* tags,
                const EntityId* component_instances,
                std::size_t num_tags))
        {
            this->register_tag_callback(
                sge::get_type<TagT>(),
                [outer, callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& component_type,
                    const void* tags,
                    const EntityId* component_instances,
                    std::size_t num_tags) -> void
            {
                (outer->*callback)(frame, component_type, static_cast<const TagT*>(tags), component_instances, num_tags);
            });
        }

        /**
         * \brief Registers a callback for when the deduced tag type is applied to the given component type.
         * \tparam ComponentT The type of component to be called for. If none is provided, this will be called for any component type.
         * \param outer The object to call this callback  member function on.
         * \param callback The callback member function to call.
         */
        template <typename TagT, typename ComponentT, class OuterT>
        void register_tag_callback(
            OuterT* outer,
            void(OuterT::*callback)(
                SystemFrame& frame,
                const TagT* tags,
                const EntityId* component_instances,
                std::size_t num_tags))
        {
            this->register_tag_callback(
                sge::get_type<TagT>(),
                sge::get_type<ComponentT>(),
                [outer, callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& /*component_type*/,
                    const TagT* tags,
                    const EntityId* component_instances,
                    std::size_t num_tags) -> void
            {
                (outer->*callback)(frame, static_cast<TagT*>(tags), component_instances, num_tags);
            });
        }

        template <typename TagT, typename ComponentT, class OuterT>
        void register_tag_callback(
            EntityId entity,
            OuterT* outer,
            void(OuterT::*callback)(
                SystemFrame& frame,
                const TagT* tags,
                const ComponentT* component_instances,
                std::size_t num_tags))
        {
            this->register_tag_callback(
                sge::get_type<TagT>(),
                ComponentId{ entity, sge::get_type<ComponentT>() },
                [outer, callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& /*component_type*/,
                    const void* tags,
                    const EntityId* component_instances,
                    std::size_t num_tags)
            {
                (outer->*callback)(frame, static_cast<const TagT*>(tags), component_instances, num_tags);
            });
        }

        //////////////////
        ///   Fields   ///
    private:

        std::vector<PipelineStep> _pipeline;

        /* System functions */
        std::unordered_map<std::string, UFunction<SystemFn>> _system_fns;

        /* Tag callbacks */
        std::unordered_map<const TypeInfo*, std::vector<TagCallback>> _tag_callbacks;
    };
}
