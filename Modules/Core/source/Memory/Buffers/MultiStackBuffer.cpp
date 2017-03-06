// MultiStackBuffer.cpp

#include "../../../include/Core/Memory/Buffers/MultiStackBuffer.h"

namespace sge
{
    MultiStackBuffer::MultiStackBuffer()
        : _num_elems(0)
    {
    }

    MultiStackBuffer::~MultiStackBuffer()
    {
        for (auto* stack : _stacks)
        {
            std::free(stack);
        }
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

    byte* const* MultiStackBuffer::stack_buffers()
    {
        return _stacks.data();
    }

    const byte* const* MultiStackBuffer::stack_buffers() const
    {
        return _stacks.data();
    }

    void* MultiStackBuffer::alloc(std::size_t obj_size)
    {
        // Determine where to place object
        const auto stack_index = _num_elems / STACK_SIZE;
        const auto stack_offset = _num_elems % STACK_SIZE;
        ++_num_elems;

        // Make sure there's a spot for the new object
        if (stack_index >= _stacks.size())
        {
            auto* buffer = (byte*)std::malloc(obj_size * STACK_SIZE);
            _stacks.push_back(buffer);
            return buffer;
        }

        return _stacks[stack_index] + (stack_offset * obj_size);
    }
}
