#pragma once

#include <stdint.h>

#include "lib/engine/build.h"

namespace sge {
struct SGE_ENGINE_API EventChannel {
  using SubscriberId = uint8_t;
  static constexpr SubscriberId INVALID_SID = 255;
  static constexpr size_t MAX_SUBSCRIBERS = 12;

  EventChannel(size_t event_object_size, int32_t capacity);
  ~EventChannel();

  SubscriberId subscribe();

  void unsubscribe(SubscriberId subscriber);

  /**
   * \brief Puts new events into this channel.
   * \param events The events to put into the channel.
   * \param event_object_size The size of each event object.
   * \param num_events The number of events to put into the channel.
   */
  void append(const void* events, size_t event_object_size, int32_t num_events);

  /**
   * \brief Puts new events into this channel.
   * \tparam EventT The type of event to put into the channel.
   * \param events The events to put into the channel.
   * \param num_events The number of events to put into the channel.
   */
  template <typename EventT>
  void append(const EventT* events, int32_t num_events) {
    this->append(events, sizeof(EventT), num_events);
  }

  /**
   * \brief Consumes events for the given subscriber.
   * \param subscriber The ID of the subscriber consuming the events.
   * \param event_object_size The size of each event object.
   * \param max_events The maximum number of events that may be compied into the buffer.
   * \param out_events The buffer to store the events.
   * \param out_num_events The number of events copied into the given buffer.
   * \return The number of events copied into the buffer.
   */
  int32_t consume(
      SubscriberId subscriber,
      size_t event_object_size,
      int32_t max_events,
      void* out_events,
      int32_t* out_num_events
  );

  /**
   * \brief Consumes events for the given subscriber.
   * \tparam EventT The type of event to consume.
   * \param subscriber The ID of the subscriber consuming the events.
   * \param max_events The maximum number of events that may be consumed.
   * \param out_events The buffer to store the events.
   * \param out_num_events The number of events copied into the given buffer.
   * \return The number of events copied into the buffer.
   */
  template <typename EventT>
  int32_t consume(SubscriberId subscriber, int32_t max_events, EventT* out_events, int32_t* out_num_events) {
    return this->consume(subscriber, sizeof(EventT), max_events, out_events, out_num_events);
  }

  /**
   * \brief Consumes events for the given subscriber.
   * \tparam EventT The type of event to consume.
   * \tparam MaxEvents The maximum number of events that may be consumed.
   * \param subscriber The ID of the subscriber consuming the events.
   * \param out_events The buffer to store the events.
   * \param out_num_events The number of events copied into the given buffer.
   * \return The number of events copied into the buffer.
   */
  template <typename EventT, int32_t MaxEvents>
  int32_t consume(SubscriberId subscriber, EventT (&out_events)[MaxEvents], int32_t* out_num_events) {
    return this->consume(subscriber, sizeof(EventT), MaxEvents, out_events, out_num_events);
  }

  /**
   * \brief Acknowledges all unconsumed events for the given subscriber.
   * \param subscriber The subscriber acknowledging the events.
   */
  void acknowledge_unconsumed(SubscriberId subscriber);

  /**
   * \brief Clears all events from this channel, and resets subscriber indices.
   */
  void clear();

 private:
  uint8_t* _buffer;
  int32_t _capacity;
  int32_t _end_index;
  int32_t _subscriber_indices[MAX_SUBSCRIBERS];
  uint8_t _subscribers_active[MAX_SUBSCRIBERS];
};
}  // namespace sge
