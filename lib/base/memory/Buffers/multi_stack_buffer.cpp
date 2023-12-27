#include <cstdlib>

#include "lib/base/memory/buffers/multi_stack_buffer.h"

namespace sge
{
    MultiStackBuffer::MultiStackBuffer()
        : _num_elems(0)
    {
    }

    MultiStackBuffer::~MultiStackBuffer()
    {
        for (auto *stack : _stacks)
        {
            std::free(stack);
        }
    }

    void MultiStackBuffer::clear()
    {
        _num_elems = 0;
        _stacks.clear();
    }

    std::size_t MultiStackBuffer::num_elems()
    {
        return _num_elems;
    }

    void MultiStackBuffer::set_num_elems(std::size_t num_elems)
    {
        _num_elems = num_elems;
    }

    std::size_t MultiStackBuffer::num_stack_buffers()
    {
        return _stacks.size();
    }

    byte *const *MultiStackBuffer::stack_buffers()
    {
        return _stacks.data();
    }

    const byte *const *MultiStackBuffer::stack_buffers() const
    {
        return _stacks.data();
    }

    void *MultiStackBuffer::alloc(std::size_t obj_size)
    {
        // Determine where to place object
        const auto num_elems = _num_elems;
        const auto stack_index = num_elems / STACK_SIZE;
        const auto stack_offset = num_elems % STACK_SIZE;

        // Make sure there's a spot for the new object
        if (stack_index >= _stacks.size())
        {
            auto *buffer = (byte *)std::malloc(obj_size * STACK_SIZE);
            _stacks.push_back(buffer);
            _num_elems = num_elems + 1;
            return buffer;
        }

        _num_elems = num_elems + 1;
        return _stacks[stack_index] + (stack_offset * obj_size);
    }

    void MultiStackBuffer::compact()
    {
        const auto num_elems = _num_elems;
        const auto last_stack = num_elems / STACK_SIZE;
        const auto num_stacks = _stacks.size();
        auto *const stacks = _stacks.data();

        // Free memory associated with the stacks
        for (auto i = last_stack; i < num_stacks; ++i)
        {
            std::free(stacks[i]);
        }

        // Remove them from the array
        _stacks.erase(_stacks.begin() + last_stack, _stacks.end());
    }
}
