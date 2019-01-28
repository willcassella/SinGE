// ResourceManager.h
#pragma once

#include <mutex>
#include <atomic>
#include <unordered_map>
#include <functional>
#include <Core/Reflection/Any.h>
#include "build.h"
#include "ResourceId.h"

namespace sge
{
    struct SGE_RESOURCE_API ResourceManager
    {
        struct Resource;

        using AsyncToken = uint32;

        /* Result code for an attempt to create a new resource. This is returned for both synchronous and asyncrhonous operations. */
        enum class NewResourceResult
        {
            /* The new resource was created successfully. */
            SUCCESS,

            /* The resource already exist. Use 'write' if you wish to overwite the existing resource. */
            ERROR_RESOURCE_EXISTS
        };

        enum class AccessResourceResult
        {
            SUCCESS,

            ERROR_NONEXISTANT_RESOURCE,

            ERROR_TYPE_MISMATCH
        };

        enum class MutateCallbackResult
        {
            RESOURCE_MUTATED,
            RESOURCE_NOT_MUTATED
        };

        using WriteCallbackFn = void(void* addr);
        using ReadCallbackFn = void(const void* resource);
        using MutateCallbackFn = MutateCallbackResult(void* resource);

        struct SGE_RESOURCE_API CallbackQueue
        {
            struct ReadCallback
            {
                //////////////////
                ///   Fields   ///
            public:

                Resource* resource;
                std::function<ReadCallbackFn> callback;
            };

            //////////////////
            ///   Fields   ///
        private:


        };

        ///////////////////
        ///   Methods   ///
    public:

        static std::string fake_uri(const std::string& base);

        NewResourceResult new_resource(
            std::string uri,
            const TypeInfo& type,
            void* init);

        template <typename T>
        NewResourceResult new_resource(
            std::string uri,
            T init)
        {
            return this->new_resource(
                std::move(uri),
                sge::get_type<T>(),
                &init);
        }

        NewResourceResult async_new_resource(
            const std::string& uri,
            const TypeInfo& type,
            CallbackQueue& cbQueue,
            std::function<WriteCallbackFn> writeFn);

        template <typename T, typename WriteFnT>
        NewResourceResult async_new_resource(
            const ResourceId<T>& id,
            AsyncToken token,
            WriteFnT writeFn)
        {
            return this->async_new_resource(id.uri(), sge::get_type<T>(), token,
                [fn = std::move(writeFn)](void* addr) -> void {
                new (addr) T{ fn() };
            });
        }

        AccessResourceResult async_write_resource(
            const std::string& uri,
            const TypeInfo& type,
            AsyncToken token,
            std::function<WriteCallbackFn> writeFn);

        template <typename T, typename WriteFnT>
        AccessResourceResult async_write_resource(
            const ResourceId<T>& id,
            AsyncToken token,
            WriteFnT writeFn)
        {
            return this->async_write_resource(id.uri(), sge::get_type<T>(), token,
                [fn = std::move(writeFn)](void* resource) -> void {
                fn(*static_cast<T*>(resource));
            });
        }

        AccessResourceResult async_mutate_resource(
            const std::string& uri,
            const TypeInfo& type,
            AsyncToken token,
            std::function<MutateCallbackFn> mutateFn);

        template <typename T, typename MutateFnT>
        AccessResourceResult async_mutate_resource(
            const ResourceId<T>& id,
            AsyncToken token,
            MutateFnT mutateFn)
        {
            return this->async_mutate_resource(id.uri(), sge::get_type<T>(), token,
                [fn = std::move(mutateFn)](void* resource) -> MutateCallbackResult {
                return fn(*static_cast<T*>(resource));
            });
        }

        AccessResourceResult async_read_resource(
            const std::string& uri,
            const TypeInfo& type,
            AsyncToken token,
            std::function<ReadCallbackFn> readFn);

        template <typename T, typename ReadFnT>
        AccessResourceResult async_read_resource(
            const ResourceId<T>& id,
            AsyncToken token,
            ReadFnT readFn)
        {
            return this->async_read_resource(id.uri(), sge::get_type<T>(), token,
                [fn = std::move(readFn)](const void* resource) {
                fn(*static_cast<const T*>(resource));
            });
        }

        AccessResourceResult async_watch_resource(
            const std::string& uri,
            const TypeInfo& type,
            AsyncToken token,
            std::function<ReadCallbackFn> readFn);

        template <typename T, typename ReadFnT>
        AccessResourceResult async_watch_resource(
            const ResourceId<T>& id,
            AsyncToken token,
            ReadFnT readFn)
        {
            return this->async_watch_resource(id.uri(), sge::get_type<T>(), token,
                [fn = std::move(readFn)](const void* resource) -> void {
                fn(*static_cast<const T*>(resource));
            });
        }

    private:

        Resource* get_resource(const std::string& uri);

        //////////////////
        ///   Fields   ///
    private:

        std::atomic<AsyncToken> _next_async_token;
        std::mutex _resources_lock;
        std::unordered_map<std::string, Resource*> _resources;
    };
}
