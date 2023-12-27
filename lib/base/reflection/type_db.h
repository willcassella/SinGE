#pragma once

#include <functional>
#include <queue>
#include <unordered_map>

#include "lib/base/reflection/reflection.h"

namespace sge
{
    struct TypeInfo;

    struct SGE_BASE_EXPORT TypeDB
    {
        using TypeEntryCallbackFn = void(TypeDB &database, const TypeInfo &type);

        /**
         * \brief Pushes a callback for newly registered types into the callback queue.
         * This callback is only called once, it must be re-registered if subsequent calls are required.
         * \param callback The callback to add to the queue.
         */
        void push_new_type_callback(std::function<TypeEntryCallbackFn> callback);

        /**
         * \brief Pushes a callback for removed registered types into the callback queue.
         * This callback is only called once, it must be re-registred if subsequent calls are required.
         * \param callback The callback to add to the queue.
         */
        void push_remove_type_callback(std::function<TypeEntryCallbackFn> callback);

        /**
         * \brief Registers a new type with the database.
         * \param type The type to register.
         */
        void new_type(const TypeInfo &type);

        template <typename T>
        void new_type()
        {
            new_type(sge::get_type<T>());
        }

        /**
         * \brief Removes a Type from the database.
         * \param type The type to remove.
         */
        void remove_type(const TypeInfo &type);

        /**
         * \brief Searches for a type entry in the database, given its name.
         * \param name The name of the type.
         * \return A pointer to the type with the given name, if one is found.
         */
        const TypeInfo *find_type(const char *name) const;

        /**
         * \brief Returns whether this TypeDB contains an entry for the given type.
         * \param type The type to search for.
         */
        bool has_type(const TypeInfo &type) const;

        /**
         * \brief Enumerates all registerd types in this database.
         * \param enumerator The function to call at each iteration.
         */
        template <typename EnumeratorFn>
        void enumerate_types(EnumeratorFn &&enumerator) const
        {
            for (const auto &type : _registered_types)
            {
                enumerator(type.second);
            }
        }

    private:
        std::queue<std::function<TypeEntryCallbackFn>> _new_type_callbacks;
        std::queue<std::function<TypeEntryCallbackFn>> _remove_type_callbacks;
        std::unordered_map<std::string, const TypeInfo *> _registered_types;
    };
}
