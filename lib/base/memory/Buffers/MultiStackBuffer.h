// MultiStackBuffer.h
#pragma once

#include <vector>
#include "base/config.h"

namespace sge
{
    struct SGE_BASE_EXPORT MultiStackBuffer
    {
        static constexpr std::size_t STACK_SIZE = 32;

        ////////////////////////
        ///   Constructors   ///
    public:
        MultiStackBuffer();
        ~MultiStackBuffer();
        MultiStackBuffer(const MultiStackBuffer &copy) = delete;
        MultiStackBuffer(MultiStackBuffer &&move) = default;
        MultiStackBuffer &operator=(const MultiStackBuffer &copy) = delete;
        MultiStackBuffer &operator=(MultiStackBuffer &&move) = delete;

        ///////////////////
        ///   Methods   ///
    public:
        void clear();

        std::size_t num_elems();

        void set_num_elems(std::size_t num_elems);

        std::size_t num_stack_buffers();

        byte *const *stack_buffers();

        const byte *const *stack_buffers() const;

        void *alloc(std::size_t obj_size);

        void compact();

        //////////////////
        ///   Fields   ///
    private:
        std::size_t _num_elems;
        std::vector<byte *> _stacks;
    };
}
