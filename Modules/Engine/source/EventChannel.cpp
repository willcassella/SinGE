// EventChannel.cpp

#include <algorithm>
#include <cstring>
#include "../include/Engine/EventChannel.h"

namespace sge
{
    EventChannel::EventChannel(std::size_t event_object_size, int32 capacity)
        : _buffer(nullptr),
        _end_index(0)
    {
        capacity = std::max(capacity, 1);
        _buffer = (byte*)std::malloc(capacity * event_object_size);
        _capacity = capacity;

        std::memset(_subscriber_indices, 0xFF, sizeof(_subscriber_indices));
        std::memset(_subscribers_active, 0, sizeof(_subscribers_active));
    }

    EventChannel::~EventChannel()
    {
        std::free(_buffer);
    }

    EventChannel::SubscriberId EventChannel::subscribe()
    {
        const auto end_index = _end_index;

        for (SubscriberId id = 0; id < MAX_SUBSCRIBERS; ++id)
        {
            if (!_subscribers_active[id])
            {
                _subscribers_active[id] = 0xFF;
                _subscriber_indices[id] = end_index;
                return id;
            }
        }

        assert(false);
        return 0;
    }

    void EventChannel::unsubscribe(SubscriberId subscriber)
    {
        assert(subscriber < MAX_SUBSCRIBERS);
        _subscribers_active[subscriber] = 0;
        _subscriber_indices[subscriber] = 0xFFFFFFFF;
    }

    void EventChannel::append(const void* events, std::size_t event_object_size, int32 num_events)
    {
        // Cache members
        auto* buffer = _buffer;
        auto capacity = _capacity;
        auto end_index = _end_index;
        auto start_index = std::numeric_limits<int32>::max();

        // Get the lowest start index
        for (auto index : _subscriber_indices)
        {
            start_index = std::min(start_index, index);
        }

        // Check start index
        if (start_index == std::numeric_limits<int32>::max())
        {
            // In the case of no subscribers, we don't have to do anything (new subscribers don't see old events)
            return;
        }

        // Compute size
        const auto size = end_index - start_index;

        // Get start_index and end_index in the [0, capacity) range
        auto mod_start_index = start_index % capacity;
        auto mod_end_index = end_index % capacity;

        // Make sure there's enough room for these events
        if (size + num_events > capacity)
        {
            // Create a new buffer
            const auto new_capacity = std::max(capacity * 2, num_events);
            auto* new_buff = (byte*)std::malloc(new_capacity * event_object_size);

            // Copy the old buffer into the new buffer
            std::memcpy(new_buff, buffer + mod_start_index * event_object_size, (capacity - mod_start_index) * event_object_size);
            std::memcpy(new_buff + (capacity - mod_start_index) * event_object_size, buffer, mod_end_index * event_object_size);
            std::free(buffer);

            // Move indices backwards
            for (SubscriberId id = 0; id < MAX_SUBSCRIBERS; ++id)
            {
                if (_subscribers_active[id])
                {
                    _subscriber_indices[id] -= start_index;
                }
            }

            // Reassign values
            buffer = new_buff;
            capacity = new_capacity;
            mod_start_index = 0;
            mod_end_index = size;
            end_index -= start_index;
        }

        // Add in new values
        const auto copy_1_num = std::min(capacity - mod_end_index, num_events);
        const auto copy_2_num = num_events - copy_1_num;
        std::memcpy(buffer + mod_end_index * event_object_size, events, copy_1_num * event_object_size);
        mod_end_index = (mod_end_index + copy_1_num) % capacity;
        std::memcpy(buffer + mod_end_index * event_object_size, (const byte*)events + copy_1_num * event_object_size, copy_2_num * event_object_size);

        // Reassign values
        _buffer = buffer;
        _capacity = capacity;
        _end_index = end_index + num_events;
    }

    int32 EventChannel::consume(SubscriberId subscriber, std::size_t event_object_size, int32 max_events, void* out_events, int32* out_num_events)
    {
        assert(subscriber < MAX_SUBSCRIBERS);
        const auto buffer = _buffer;
        const auto capacity = _capacity;
        const auto end_index = _end_index;
        const auto index = _subscriber_indices[subscriber];
        const auto size = end_index - index;
        const auto mod_index = index % capacity;

        // Figure out how much to copy
        const auto copy_1_num = std::min({ capacity - mod_index, size, max_events });
        const auto copy_2_num = std::min({ max_events - copy_1_num, size - copy_1_num });

        // Perform actual copy
        std::memcpy(out_events, buffer + mod_index * event_object_size, copy_1_num * event_object_size);
        std::memcpy((byte*)out_events + copy_1_num * event_object_size, buffer, copy_2_num * event_object_size);

        const auto num_copied = copy_1_num + copy_2_num;
        _subscriber_indices[subscriber] = index + num_copied;
        if (out_num_events)
        {
            *out_num_events = num_copied;
        }

        return num_copied;
    }

    void EventChannel::acknowledge_unconsumed(SubscriberId subscriber)
    {
        _subscriber_indices[subscriber] = _end_index;
    }

    void EventChannel::clear()
    {
        _end_index = 0;

        for (SubscriberId id = 0; id < MAX_SUBSCRIBERS; ++id)
        {
            if (_subscribers_active[id])
            {
                _subscriber_indices[id] = 0;
            }
        }
    }
}
