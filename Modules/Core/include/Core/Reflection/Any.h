// Any.h
#pragma once

#include <utility>
#include "../Util/InterfaceUtils.h"

namespace sge
{
    struct TypeInfo;

    template <typename T>
    const auto& get_type(const T&);

    template <typename I>
    struct AnyImpl
    {
        ////////////////////////
        ///   Constructors   ///
    protected:

        AnyImpl(const I& impl)
            : _impl(&impl)
        {
        }

        ///////////////////
        ///   Methods   ///
    protected:

        const I& impl() const
        {
            return *_impl;
        }

        //////////////////
        ///   Fields   ///
    private:

        const I* _impl;
    };

    template <typename ... Is>
    struct AnyMut : AnyImpl<Is>...
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        AnyMut(const TypeInfo& type, void* object, const Is& ... impls)
            : AnyImpl<Is>(impls)...,
            _type(&type),
            _object(object)
        {
        }

        template <typename T>
        AnyMut(T& value)
            : AnyImpl<Is>(Is::template get_impl<T>())...,
            _type(&get_type(value)),
            _object(&value)
        {
        }

        template <typename ... OtherIs>
        AnyMut(AnyMut<OtherIs...> other)
            : AnyImpl<Is>(other)...,
            _type(&other.type()),
            _object(other.object())
        {
        }

        AnyMut(AnyMut& copy)
            : AnyImpl<Is>(copy)...,
            _type(copy._type),
            _object(copy._object)
        {
        }
        AnyMut(const AnyMut& copy)
            : AnyImpl<Is>(copy)...,
            _type(copy._type),
            _object(copy._object)
        {
        }
        AnyMut(AnyMut&& move)
            : AnyImpl<Is>(move)...,
            _type(move._type),
            _object(move._object)
        {
        }
        AnyMut(const AnyMut&& move)
            : AnyImpl<Is>(move)...,
            _type(move._type),
            _object(move._object)
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        const TypeInfo& type() const
        {
            return *_type;
        }

        void* object() const
        {
            return _object;
        }

        template <typename I>
        const I& impl() const
        {
            return this->AnyImpl<I>::impl();
        }

        template <typename Ret, typename I, typename ... ArgTs, typename ... ActualArgTs>
        Ret call(Ret(*I::*member)(SelfMut, ArgTs...), ActualArgTs&& ... args) const
        {
            return (this->AnyImpl<I>::impl().*member)(SelfMut{ _object }, std::forward<ActualArgTs>(args)...);
        }

        template <typename Ret, typename I, typename ... ArgTs, typename ... ActualArgTs>
        Ret call(Ret(*I::*member)(Self, ArgTs...), ActualArgTs&& ... args) const
        {
            return (this->AnyImpl<I>::impl().*member)(Self{ _object }, std::forward<ActualArgTs>(args)...);
        }

        template <typename Ret, typename I, typename ... ArgTs, typename ... ActualArgTs>
        Ret call(Ret(*I::*member)(ArgTs...), ActualArgTs&& ... args) const
        {
            return (this->AnyImpl<I>::impl().*member)(std::forward<ActualArgTs>(args)...);
        }

        //////////////////
        ///   Fields   ///
    private:

        const TypeInfo* _type;
        void* _object;
    };

    template <typename ... Is>
    struct Any : AnyImpl<Is>...
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        Any(const TypeInfo& type, const void* object, const Is& ... impls)
            : AnyImpl<Is>(impls)...,
            _type(&type),
            _object(object)
        {
        }

        template <typename T>
        Any(const T& value)
            : AnyImpl<Is>(Is::template get_impl<T>())...,
            _type(&get_type(value)),
            _object(&value)
        {
        }

        template <typename ... OtherIs>
        Any(AnyMut<OtherIs...> other)
            : AnyImpl<Is>(other)...,
            _type(&other.type()),
            _object(other.object())
        {
        }

        template <typename ... OtherIs>
        Any(Any<OtherIs...> other)
            : AnyImpl<Is>(other)...,
            _type(&other.type()),
            _object(other.object())
        {
        }

        Any(Any& copy)
            : AnyImpl<Is>(copy)...,
            _type(copy._type),
            _object(copy._object)
        {
        }
        Any(const Any& copy)
            : AnyImpl<Is>(copy)...,
            _type(copy._type),
            _object(copy._object)
        {
        }
        Any(Any&& move)
            : AnyImpl<Is>(move)...,
            _type(move._type),
            _object(move._object)
        {
        }
        Any(const Any&& move)
            : AnyImpl<Is>(move)...,
            _type(move._type),
            _object(move._object)
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        const TypeInfo& type() const
        {
            return *_type;
        }

        const void* object() const
        {
            return _object;
        }

        template <typename Ret, typename I, typename ... ArgTs, typename ... ActualArgTs>
        Ret call(Ret(*I::*member)(Self, ArgTs...), ActualArgTs&& ... args) const
        {
            return (this->AnyImpl<I>::impl().*member)(Self{ _object }, std::forward<ActualArgTs>(args)...);
        }

        template <typename Ret, typename I, typename ... ArgTs, typename ... ActualArgTs>
        Ret call(Ret(*I::*member)(ArgTs...), ActualArgTs&& ... args) const
        {
            return (this->AnyImpl<I>::impl().*member)(std::forward<ActualArgTs>(args)...);
        }

        //////////////////
        ///   Fields   ///
    private:

        const TypeInfo* _type;
        const void* _object;
    };
}
