// event bus class adapted from here: https://gist.github.com/Jgb14002/44716ad59c9654ad08b59abbf1c45b40
#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>

class EventBus
{
public:
  struct Event { bool handled = false; };

  // implementation details
private:
  class EventDispatcher
  {
  public:
    virtual ~EventDispatcher() = default;
    inline void Dispatch(Event& e) const
    {
      InvokeHandler(e);
    }

  private:
    virtual void InvokeHandler(Event& e) const = 0;
  };

  // type-erased wrapper
  template<typename Receiver, typename EventType>
  class EventHandler : public EventDispatcher
  {
  public:
    using HandlerFn = void(Receiver::*)(EventType&);

    EventHandler(Receiver* receiver, HandlerFn handlerFn)
      : m_Receiver(receiver), m_HandlerFn(handlerFn)
    {
    }

    void InvokeHandler(Event& e) const override
    {
      (m_Receiver->*m_HandlerFn)(static_cast<EventType&>(e));
    }

    bool operator==(const EventHandler& rhs) const
    {
      return (m_Receiver == rhs.m_Receiver) && (m_HandlerFn == rhs.m_HandlerFn);
    }

    bool operator==(const Receiver* rhs) const
    {
      return m_Receiver == rhs;
    }

  private:
    Receiver* m_Receiver;
    HandlerFn m_HandlerFn;
  };

public:
  using Dispatcher = std::unique_ptr<EventDispatcher>;
  using HandlerList = std::vector<Dispatcher>;

  EventBus() = default;
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  template<typename EventType>
  void Publish(EventType&& e)
  {
    auto entry = m_Subscriptions.find(typeid(EventType));
    if (entry != m_Subscriptions.end())
    {
      for (const auto& handler : entry->second)
      {
        if (e.handled) return;
        handler->Dispatch(e);
      }
    }
  }

  template<typename Receiver, typename EventType>
  void Subscribe(Receiver* receiver, void(Receiver::* handlerFn)(EventType&))
  {
    HandlerList& handlers = m_Subscriptions[typeid(EventType)];
    handlers.emplace_back(std::make_unique<EventHandler<Receiver, EventType>>(receiver, handlerFn));
  }

  // unsubscribes a receiver from a single event
  template<typename Receiver, typename EventType>
  void Unsubscribe(Receiver* receiver, void(Receiver::* handlerFn)(EventType&))
  {
    auto entry = m_Subscriptions.find(typeid(EventType));
    if (entry != m_Subscriptions.end())
    {
      const EventHandler<Receiver, EventType> tmp = { receiver, handlerFn };
      HandlerList& handlers = entry->second;
      std::erase_if(handlers, [&](const auto& handler)
        {
          using T = EventHandler<Receiver, EventType>;
          return *static_cast<T*>(handler.get()) == tmp;
        });
    }
  }

  // unsubscribes a receiver from all events
  template<typename Receiver>
  void UnsubscribeAll(Receiver* receiver)
  {
    for (auto& [_, handlers] : m_Subscriptions)
    {
      std::erase_if(handlers, [&](const auto& handler)
        {
          using T = EventHandler<Receiver, Event>;
          return *static_cast<T*>(handler.get()) == receiver;
        });
    }
  }

private:
  std::unordered_map<std::type_index, HandlerList> m_Subscriptions;
};