#pragma once

#include <vector>

#include "lib/base/build.h"

namespace sge
{
    struct SGE_BASE_EXPORT MultiStackBuffer
    {
        static constexpr size_t STACK_SIZE = 32;

        MultiStackBuffer();
        ~MultiStackBuffer();
        MultiStackBuffer(const MultiStackBuffer &copy) = delete;
        MultiStackBuffer(MultiStackBuffer &&move) = default;
        MultiStackBuffer &operator=(const MultiStackBuffer &copy) = delete;
        MultiStackBuffer &operator=(MultiStackBuffer &&move) = delete;

        void clear();

        size_t num_elems();

        void set_num_elems(size_t num_elems);

        size_t num_stack_buffers();

        uint8_t* const* stack_buffers();

        const uint8_t* const* stack_buffers() const;

        void* alloc(size_t obj_size);

        void compact();

    private:
        size_t _num_elems;
        std::vector<uint8_t*> _stacks;
    };
}
