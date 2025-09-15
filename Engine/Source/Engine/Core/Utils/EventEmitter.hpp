///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <Engine/Config.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <variant>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Forward declaration of TEvents for friend declaration
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct TEvents;

///////////////////////////////////////////////////////////////////////////////
// Namespace __internal
///////////////////////////////////////////////////////////////////////////////
namespace __internal
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Generic Event Emitter Template Class
///
/// \tparam EventTypes Variadic template of all possible event types
///
/// \example
/// Usage example:
/// struct KeyPressed { int keyCode; };
/// struct MouseClicked { int x, y; };
///
/// class MyClass : public TEventEmitter<KeyPressed, MouseClicked>
/// {
///     void setupEventHandlers(void)
///     {
///         // Register persistent listeners
///         auto keyHandle = On<KeyPressed>([](const KeyPressed& event) {
///             std::cout << "Key pressed: " << event.keyCode << std::endl;
///         });
///
///         // Register one-time listeners
///         Once<MouseClicked>([](const MouseClicked& event) {
///             std::cout << "Mouse clicked once at: " << event.x << ", " <<
///             event.y << std::endl;
///         });
///
///         // Remove a listener
///         RemoveListener(keyHandle);
///     }
///
///     void handleInput(void)
///     {
///         // Emit to queue for later processing
///         EmitEvent(KeyPressed{42});
///
///         // Or trigger immediately
///         TriggerEvent(MouseClicked{100, 200});
///
///         // Process queued events
///         while (auto event = PollEvent())
///         {
///             if (event->template Is<KeyPressed>())
///             {
///                 auto* keyEvent = event->template GetIf<KeyPressed>();
///                 // Handle key press
///             }
///         }
///     }
/// };
///
///////////////////////////////////////////////////////////////////////////////
template <typename... EventTypes>
class TEventEmitter
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Handle type for managing event listeners
    ///////////////////////////////////////////////////////////////////////////
    using ListenerHandle = uint64_t;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Generic Event wrapper
    ///////////////////////////////////////////////////////////////////////////
    class Event
    {
    private:
        ///////////////////////////////////////////////////////////////////////
        // Member data
        ///////////////////////////////////////////////////////////////////////
        std::variant<EventTypes...> m_data;

        ///////////////////////////////////////////////////////////////////////
        /// \brief Check if a type is one of the variant types
        ///
        /// \tparam T Type to check
        ///
        /// \return True if the type is one of the variant types
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        static constexpr bool IsValidEventType =
            (std::disjunction_v<std::is_same<T, EventTypes>...>);

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructor from event data
        ///
        /// \tparam T Type of event data
        ///
        /// \param data The event data
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        Event(const T& data)
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>) { m_data = data; }
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Move constructor from event data
        ///
        /// \tparam T Type of event data
        ///
        /// \param data The event data
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        Event(T&& data)
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>) { m_data = std::move(data); }
        }

    public:
        ///////////////////////////////////////////////////////////////////////
        /// \brief Check if the event is of a specific type
        ///
        /// \tparam T Type to check
        ///
        /// \return True if the event is of the specified type
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        bool Is(void) const
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>)
            {
                return std::holds_alternative<T>(m_data);
            }
            return false;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get the event as a specific type (mutable)
        ///
        /// \tparam T Type to get
        ///
        /// \return Pointer to the event data, or nullptr if wrong type
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T* GetIf(void)
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>)
            {
                return std::get_if<T>(&m_data);
            }
            return nullptr;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get the event as a specific type (const)
        ///
        /// \tparam T Type to get
        ///
        /// \return Const pointer to the event data, or nullptr if wrong type
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        const T* GetIf(void) const
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>)
            {
                return std::get_if<T>(&m_data);
            }
            return nullptr;
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get the event as a specific type (throws on wrong type)
        ///
        /// \tparam T Type to get
        ///
        /// \return Reference to the event data
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        T& Get(void)
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>) { return std::get<T>(m_data); }
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Get the event as a specific type (throws on wrong type,
        /// const)
        ///
        /// \tparam T Type to get
        ///
        /// \return Const reference to the event data
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        const T& Get(void) const
        {
            static_assert(
                IsValidEventType<T>,
                "Invalid event type - not in EventTypes list"
            );
            if constexpr (IsValidEventType<T>) { return std::get<T>(m_data); }
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Visit the event with a visitor function/lambda
        ///
        /// \tparam Visitor Visitor type (usually lambda)
        ///
        /// \param visitor The visitor to apply
        ///
        /// \return Result of visitor application
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename Visitor>
        decltype(auto) Visit(Visitor&& visitor)
        {
            return std::visit(std::forward<Visitor>(visitor), m_data);
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Visit the event with a visitor function/lambda (const)
        ///
        /// \tparam Visitor Visitor type (usually lambda)
        ///
        /// \param visitor The visitor to apply
        ///
        /// \return Result of visitor application
        ///
        ///////////////////////////////////////////////////////////////////////
        template <typename Visitor>
        decltype(auto) Visit(Visitor&& visitor) const
        {
            return std::visit(std::forward<Visitor>(visitor), m_data);
        }
    };

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Base class for event listeners
    ///////////////////////////////////////////////////////////////////////////
    struct ListenerBase
    {
        virtual ~ListenerBase() = default;
        virtual void Call(const Event& event) = 0;
        virtual bool ShouldRemove() const = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Template listener implementation
    ///
    /// \tparam T Event type this listener handles
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    struct Listener : public ListenerBase
    {
        ///////////////////////////////////////////////////////////////////////
        // Class Member
        ///////////////////////////////////////////////////////////////////////
        std::function<void(const T&)> callback;   //<! Callback function
        bool once;             //<! If true, remove after first call
        mutable bool called;   //<! If true, has been called

        ///////////////////////////////////////////////////////////////////////
        /// \brief Constructor for Listener
        ///
        /// \param cb Callback function
        /// \param isOnce If true, the listener will be removed after the
        /// first call
        ///
        ///////////////////////////////////////////////////////////////////////
        Listener(std::function<void(const T&)> cb, bool isOnce = false)
            : callback(std::move(cb))
            , once(isOnce)
            , called(false)
        {}

        ///////////////////////////////////////////////////////////////////////
        /// \brief Call the listener with the event data
        ///
        /// \param event The event to call the listener with
        ///
        ///////////////////////////////////////////////////////////////////////
        void Call(const Event& event) override
        {
            if (auto eventData = event.template GetIf<T>())
            {
                callback(*eventData);
                called = true;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        /// \brief Check if the listener should be removed (for "once"
        /// listeners)
        ///
        /// \return True if the listener should be removed
        ///
        ///////////////////////////////////////////////////////////////////////
        bool ShouldRemove(void) const override { return once && called; }
    };

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~TEventEmitter() = default;

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Member data
    ///////////////////////////////////////////////////////////////////////////
    std::queue<Event> m_eventQueue;
    std::unordered_map<
        std::type_index,
        std::vector<std::pair<ListenerHandle, std::unique_ptr<ListenerBase>>>>
        m_listeners;
    ListenerHandle m_nextHandle = 1;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Poll the next event from the queue
    ///
    /// \return Optional event (empty if no events in queue)
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::optional<Event> PollEvent(void)
    {
        if (m_eventQueue.empty()) { return std::nullopt; }

        Event event = std::move(m_eventQueue.front());
        m_eventQueue.pop();
        return event;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if there are any events in the queue
    ///
    /// \return True if events are available
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool HasEvents(void) const { return !m_eventQueue.empty(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of events in the queue
    ///
    /// \return Number of queued events
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t GetEventCount(void) const { return m_eventQueue.size(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear all events from the queue
    ///
    ///////////////////////////////////////////////////////////////////////////
    void ClearEvents(void)
    {
        while (!m_eventQueue.empty()) { m_eventQueue.pop(); }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register a persistent event listener
    ///
    /// \tparam T Event type to listen for
    ///
    /// \param callback Function to call when the event is triggered
    ///
    /// \return Handle to the listener (for removal)
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    ListenerHandle On(std::function<void(const T&)> callback)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );

        auto typeIndex = std::type_index(typeid(T));
        auto handle = m_nextHandle++;

        auto listener =
            std::make_unique<Listener<T>>(std::move(callback), false);
        m_listeners[typeIndex].emplace_back(handle, std::move(listener));

        return handle;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Register a one-time event listener
    ///
    /// \tparam T Event type to listen for
    ///
    /// \param callback Function to call when the event is triggered (only
    /// once)
    ///
    /// \return Handle to the listener (for removal)
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    ListenerHandle Once(std::function<void(const T&)> callback)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );

        auto typeIndex = std::type_index(typeid(T));
        auto handle = m_nextHandle++;

        auto listener =
            std::make_unique<Listener<T>>(std::move(callback), true);
        m_listeners[typeIndex].emplace_back(handle, std::move(listener));

        return handle;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove a specific event listener by handle
    ///
    /// \param handle Handle returned by On() or Once()
    ///
    /// \return True if the listener was found and removed
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool RemoveListener(ListenerHandle handle)
    {
        for (auto& [typeIndex, listeners]: m_listeners)
        {
            auto it = std::find_if(
                listeners.begin(),
                listeners.end(),
                [handle](const auto& pair) { return pair.first == handle; }
            );

            if (it != listeners.end())
            {
                listeners.erase(it);
                return true;
            }
        }
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all listeners for a specific event type
    ///
    /// \tparam T Event type to remove listeners for
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void RemoveAllListeners()
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );

        auto typeIndex = std::type_index(typeid(T));
        m_listeners[typeIndex].clear();
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Remove all listeners for all event types
    ///
    ///////////////////////////////////////////////////////////////////////////
    void RemoveAllListeners() { m_listeners.clear(); }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the number of listeners for a specific event type
    ///
    /// \tparam T Event type to count listeners for
    ///
    /// \return Number of listeners for the event type
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    size_t GetListenerCount() const
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );

        auto typeIndex = std::type_index(typeid(T));
        auto it = m_listeners.find(typeIndex);
        return (it != m_listeners.end()) ? it->second.size() : 0;
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Trigger all listeners for a given event
    ///
    /// \param event The event to trigger listeners for
    ///
    ///////////////////////////////////////////////////////////////////////////
    void TriggerListeners(const Event& event)
    {
        // Visit the event to get its type and trigger appropriate listeners
        event.Visit(
            [this](const auto& eventData)
            {
                using EventType = std::decay_t<decltype(eventData)>;
                auto typeIndex = std::type_index(typeid(EventType));

                auto it = m_listeners.find(typeIndex);
                if (it != m_listeners.end())
                {
                    auto& listeners = it->second;

                    // Call all listeners
                    for (auto& [handle, listener]: listeners)
                    {
                        listener->Call(Event{ eventData });
                    }

                    // Remove any "once" listeners that have been called
                    listeners.erase(
                        std::remove_if(
                            listeners.begin(),
                            listeners.end(),
                            [](const auto& pair)
                            { return pair.second->ShouldRemove(); }
                        ),
                        listeners.end()
                    );
                }
            }
        );
    }

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emit an event (add it to the queue)
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to emit
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void EmitEvent(const T& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        m_eventQueue.emplace(eventData);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emit an event (move version)
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to emit
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void EmitEvent(T&& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        m_eventQueue.emplace(std::forward<T>(eventData));
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Immediately trigger listeners for an event (bypassing queue)
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to trigger
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void TriggerEvent(const T& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        TriggerListeners(Event{ eventData });
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Immediately trigger listeners for an event (move version)
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to trigger
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void TriggerEvent(T&& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        TriggerListeners(Event{ std::forward<T>(eventData) });
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emit an event and immediately trigger listeners
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to emit and trigger
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Emit(const T& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        m_eventQueue.emplace(eventData);
        TriggerListeners(Event{ eventData });
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Emit an event and immediately trigger listeners
    ///
    /// \tparam T Type of event
    ///
    /// \param eventData The event data to emit and trigger
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename T>
    void Emit(T&& eventData)
    {
        static_assert(
            (std::disjunction_v<std::is_same<T, EventTypes>...>),
            "Event type not in template parameter list"
        );
        m_eventQueue.emplace(std::forward<T>(eventData));
        TriggerListeners(Event{ std::forward<T>(eventData) });
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Helper to convert a TEvents struct to a TEventEmitter
///
/// \tparam Tuple Tuple of event types from TEvents::All
///
///////////////////////////////////////////////////////////////////////////////
template <typename Tuple>
struct TupleToTEventEmitter;

///////////////////////////////////////////////////////////////////////////////
/// \brief Helper to convert a tuple of types to a TEventEmitter
///
/// \tparam Ts Variadic template parameters representing event types
///
///////////////////////////////////////////////////////////////////////////////
template <typename... Ts>
struct TupleToTEventEmitter<std::tuple<Ts...>>
{
    using type = TEventEmitter<Ts...>;
};

template <typename T>
struct IsTuple : std::false_type
{};

template <typename... Ts>
struct IsTuple<std::tuple<Ts...>> : std::true_type
{};

}   // namespace __internal

///////////////////////////////////////////////////////////////////////////////
/// \brief Unified TEventEmitter alias that handles both tuple and variadic
/// cases
///
/// \tparam T First type parameter (could be tuple or first event type)
/// \tparam EventTypes Additional event types (empty for tuple case)
///
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename... EventTypes>
using TEventEmitter = std::conditional_t<
    __internal::IsTuple<T>::value && sizeof...(EventTypes) == 0,
    typename __internal::TupleToTEventEmitter<T>::type,
    __internal::TEventEmitter<T, EventTypes...>>;

}   // namespace tkd
