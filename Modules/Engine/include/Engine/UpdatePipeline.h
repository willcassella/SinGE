// UpdatePipeline.h
#pragma once

#include <memory>
#include <Core/Functional/UFunction.h>
#include "TagBuffer.h"

namespace sge
{
    struct Scene;
    struct SceneData;
    struct SystemFrame;
    struct SystemInfo;
    struct TagCallbackInfo;

    using TagCallbackOptions_t = uint32;
    enum : TagCallbackOptions_t
    {
        /**
         * \brief There are no special options associated with this tag callback.
         */
        TCO_NONE,

        /**
         * \brief The tag callback may be called asynchronously with respect to itself.
         */
        TCO_SELF_ASYNC = (1 << 0)
    };

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

        /**
         * \brief Function signature used for tag callback functions.
         * \param frame The system frame the callback may use.
         * \param tag_type The type of tag this callback is being called for.
         * \param component_type The type of component the tag was applied to.
         * \param tags An array of tag objects that were applied to the components.
         * \param component_instances An ordered array of entities the components are attached to that the tags were applied to.
         * \param num_tags The number of tags in the array.
         */
        using TagCallbackFn = void(
            SystemFrame& frame,
            const TypeInfo& tag_type,
            const TypeInfo& component_type,
            const EntityId* entities,
            const TagCount_t* tag_counts,
            std::size_t num_ents,
            const void* tag_buffer);

        /**
         * \brief Uniquely identifies a System.
         */
        using SystemToken = uint16;

        /**
         * \brief Used for tag callbacks to indicate that they should not be associated with any particular system.
         */
        static constexpr SystemToken NO_SYSTEM = 0;

        /**
         * \brief Tag callbacks with the same AsyncToken will be run synchronously.
         */
        using AsyncToken = uint16;

        /**
         * \brief Async token used to define tag callbacks that have no syncrhonous dependencies.
         */
        static constexpr AsyncToken FULLY_ASYNC = 0;

        using PipelineStep = std::vector<SystemInfo*>;
        using Pipeline = std::vector<PipelineStep>;

        /////////////////////////
        ///   Constructors    ///
    public:

        UpdatePipeline();
        ~UpdatePipeline();
        UpdatePipeline(const UpdatePipeline& copy) = delete;
        UpdatePipeline& operator=(const UpdatePipeline& copy) = delete;
        UpdatePipeline(UpdatePipeline&& move) = delete;
        UpdatePipeline& operator=(UpdatePipeline&& move) = delete;

        ///////////////////
        ///   Methods   ///
    public:

        void configure_pipeline(ArchiveReader& reader);

        const Pipeline& get_pipeline() const;

        AsyncToken new_async_token();

        /**
        * \brief Registers a system function to be called during the read phase.
        * This design may change in the future.
        * \param system_fn The system function to call during the read phase.
        * \return The token for the system function, which may be used to unregister it.
        */
        SystemToken register_system_fn(
            std::string name,
            AsyncToken async_token,
            UFunction<SystemFn> system_fn);

        /**
        * \brief Registers a system member function to be called during the read phase.
        * This design may change in the future.
        * \param outer The system object that the member function is to be called on.
        * \param system_fn The system member function to call during the read phase.
        * \return The token for the system function, which may be used to unregister it.
        */
        template <typename T>
        SystemToken register_system_fn(
            std::string name,
            AsyncToken async_token,
            T* outer,
            void(T::*system_fn)(SystemFrame& frame, float current_time, float dt))
        {
            return this->register_system_fn(std::move(name), async_token, [outer, system_fn]
                (SystemFrame& frame, float current_time, float dt)
            {
                (outer->*system_fn)(frame, current_time, dt);
            });
        }

        void register_tag_callback(
            SystemToken system_token,
            AsyncToken async_token,
            TagCallbackOptions_t options,
            const TypeInfo& tag_type,
            UFunction<TagCallbackFn> callback);

        void register_tag_callback(
            SystemToken system_token,
            AsyncToken async_token,
            TagCallbackOptions_t options,
            const TypeInfo& tag_type,
            const TypeInfo& component_type,
            UFunction<TagCallbackFn> callback);

        template <typename ComponentT, typename TagT>
        void register_tag_callback(
            SystemToken system_token,
            AsyncToken async_token,
            TagCallbackOptions_t options,
            void(*callback)(
                SystemFrame& frame,
                const EntityId* entities,
                const TagCount_t* tag_counts,
                std::size_t num_ents,
                const TagT* tags))
        {
            this->register_tag_callback(
                system_token,
                async_token,
                options,
                sge::get_type<TagT>(),
                sge::get_type<ComponentT>(),
                [callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& /*component_type*/,
                    const EntityId* entities,
                    const TagCount_t* tag_counts,
                    std::size_t num_ents,
                    const void* tag_buffer) -> void
            {
                callback(frame, entities, tag_counts, num_ents, static_cast<const TagT*>(tag_buffer));
            });
        }

        /**
         * \brief Registers a callback for when the deduced tag type is applied to the given component type.
         * \tparam ComponentT The type of component to be called for. If none is provided, this will be called for any component type.
         * \param outer The object to call this callback  member function on.
         * \param callback The callback member function to call.
         */
        template <typename ComponentT, typename TagT, class OuterT>
        void register_tag_callback(
            SystemToken system_token,
            AsyncToken async_token,
            TagCallbackOptions_t options,
            OuterT* outer,
            void(OuterT::*callback)(
                SystemFrame& frame,
                const EntityId* entities,
                const TagCount_t* tag_counts,
                std::size_t num_ents,
                const TagT* tags))
        {
            this->register_tag_callback(
                system_token,
                async_token,
                options,
                sge::get_type<TagT>(),
                sge::get_type<ComponentT>(),
                [outer, callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& /*component_type*/,
                    const EntityId* entities,
                    const TagCount_t* tag_counts,
                    std::size_t num_ents,
                    const void* tags) -> void
            {
                (outer->*callback)(frame, entities, tag_counts, num_ents, static_cast<const TagT*>(tags));
            });
        }

        template <typename ComponentT, typename TagT, class OuterT>
        void register_tag_callback(
            SystemToken system_token,
            AsyncToken async_token,
            TagCallbackOptions_t options,
            OuterT* outer,
            void (OuterT::*callback)(
                SystemFrame& frame,
                const EntityId* entities,
                std::size_t num_ents))
        {
            this->register_tag_callback(
                system_token,
                async_token,
                options,
                sge::get_type<TagT>(),
                sge::get_type<ComponentT>(),
                [outer, callback](
                    SystemFrame& frame,
                    const TypeInfo& /*tag_type*/,
                    const TypeInfo& /*component_type*/,
                    const EntityId* entities,
                    const TagCount_t* /*tag_counts*/,
                    std::size_t num_ents,
                    const void* /*tags*/) -> void
            {
                (outer->*callback)(frame, entities, num_ents);
            });
        }

        //////////////////
        ///   Fields   ///
    private:

        SystemToken _next_system_token;
        AsyncToken _next_async_token;

        std::vector<PipelineStep> _pipeline;

        /* System functions */
        std::unordered_map<std::string, std::unique_ptr<SystemInfo>> _systems;

        /* Tag callbacks */
        std::unordered_map<const TypeInfo*, std::map<AsyncToken, std::vector<TagCallbackInfo>>> _tag_callbacks;
    };
}
