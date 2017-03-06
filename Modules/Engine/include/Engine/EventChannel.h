// EventChannel.h
#pragma once

#include <Core/Functional/UFunction.h>
#include <Core/Reflection/TypeInfo.h>
#include "config.h"

namespace sge
{
    struct SGE_ENGINE_API EventChannel
    {
        using SubscriberId = uint8;
        static constexpr std::size_t MAX_SUBSCRIBERS = 12;

        ////////////////////////
        ///   Constructors   ///
    public:

        EventChannel(std::size_t event_object_size, int32 capacity);
        ~EventChannel();

        ///////////////////
        ///   Methods   ///
    public:

        SubscriberId subscribe();

        void unsubscribe(SubscriberId subscriber);

        /**
         * \brief Puts new events into this channel.
         * \param events The events to put into the channel.
         * \param event_object_size The size of each event object.
         * \param num_events The number of events to put into the channel.
         */
        void append(const void* events, std::size_t event_object_size, int32 num_events);

        /**
         * \brief Consumes events for the given subscriber.
         * \param subscriber The ID of the subscriber consuming the events.
         * \param event_object_size The size of each event object.
         * \param max_events The maximum number of events that may be compied into the buffer.
         * \param out_events The buffer to store the events.
         * \param out_num_events The number of events copied into the given buffer.
         * \return The number of events copied into the buffer.
         */
        int32 consume(SubscriberId subscriber, std::size_t event_object_size, int32 max_events, void* out_events, int32* out_num_events);

        /**
         * \brief Acknlowedges the given number of events without actually retreiving them.
         * \param subscriber The subscriber acknowledging the events.
         * \param num_events The number of events to acknowledge.
         */
        void acknowledge(SubscriberId subscriber, int32 num_events);

        /**
         * \brief Clears all events from this channel, and resets subscriber indices.
         */
        void clear();

        //////////////////
        ///   Fields   ///
    private:

        byte* _buffer;
        int32 _capacity;
        int32 _end_index;
        int32 _subscriber_indices[MAX_SUBSCRIBERS];
        uint8 _subscribers_active[MAX_SUBSCRIBERS];
    };
}
