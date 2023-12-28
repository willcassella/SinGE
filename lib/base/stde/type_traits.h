#pragma once

#include <type_traits>

#include "tmp.h"

namespace stde
{
    /* Copies the const qualifier from 'FromT' to 'ToT' */
    template <typename FromT, typename ToT>
    struct copy_const
    {
        using type = ToT;
    };

    /* Copies the const qualifier from 'FromT' to 'ToT' */
    template <typename FromT, typename ToT>
    struct copy_const <const FromT, ToT>
    {
        using type = const ToT;
    };

    /* Copies the const qualifier from 'FromT' to 'ToT' */
    template <typename FromT, typename ToT>
    using copy_const_t = typename copy_const<FromT, ToT>::type;

    template <typename FuncT>
    struct function_traits : function_traits<decltype(&FuncT::operator())>
    {
    };

    template <typename FuncT>
    struct function_traits< FuncT& > : function_traits<FuncT>
    {
    };

    template <typename FuncT>
    struct function_traits< FuncT* > : function_traits<FuncT>
    {
    };

    template <typename Ret, typename ... Args>
    struct function_traits< Ret(Args...) >
    {
        using return_type = Ret;
        using self_type = void;
        using arg_types = sge::tmp::list<Args...>;
        static constexpr size_t arity = sizeof...(Args);
    };

    template <class T, typename Ret, typename ... Args>
    struct function_traits< Ret(T::*)(Args...) >
    {
        using return_type = Ret;
        using self_type = T;
        using arg_types = sge::tmp::list<Args...>;
        static constexpr size_t arity = sizeof...(Args);
    };

    template <typename T, typename Ret, typename ... Args>
    struct function_traits< Ret(T::*)(Args...)const >
    {
        using return_type = Ret;
        using self_type = const T;
        using arg_types = sge::tmp::list<Args...>;
        static constexpr size_t arity = sizeof...(Args);
    };

    /* Handles case when 'nullptr' was passed a function pointer argument, allows you to perform SFINAE instead of erroring out. */
    template <>
    struct function_traits< std::nullptr_t >
    {};
}
