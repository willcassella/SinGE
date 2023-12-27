#pragma once

namespace sge
{
    /* Use for the first argument of an interface function to represent the 'this' argument in a non-const method. */
    struct SelfMut
    {
        SelfMut(void* self)
            : _self(self)
        {
        }

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

    private:
        void* _self;
    };

    /* Use for the first argument of an interface function to represent the 'this' argument in a const method. */
    struct Self
    {
        Self(const void* self)
            : _self(self)
        {
        }

        Self(SelfMut self)
            : _self(self.ptr())
        {
        }

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

    private:
        const void* _self;
    };
}
