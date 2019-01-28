// InterfaceUtils.h
#pragma once

namespace sge
{
    /* Use for the first argument of an interface function to represent the 'this' argument in a non-const method. */
    struct SelfMut
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        SelfMut(void* self)
            : _self(self)
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        template <typename T>
        T* as() const
        {
            return static_cast<T*>(_self);
        }

        void* ptr() const
        {
            return _self;
        }

        bool null() const
        {
            return _self == nullptr;
        }

        //////////////////
        ///   Fields   ///
    private:

        void* _self;
    };

    /* Use for the first argument of an interface function to represent the 'this' argument in a const method. */
    struct Self
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        Self(const void* self)
            : _self(self)
        {
        }

        Self(SelfMut self)
            : _self(self.ptr())
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        template <typename T>
        const T* as() const
        {
            return static_cast<const T*>(_self);
        }

        const void* ptr() const
        {
            return _self;
        }

        bool null() const
        {
            return _self == nullptr;
        }

        //////////////////
        ///   Fields   ///
    private:

        const void* _self;
    };
}
