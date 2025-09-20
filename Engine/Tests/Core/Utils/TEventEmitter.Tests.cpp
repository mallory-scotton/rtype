///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <catch2/catch.hpp>
#include <Engine/Core/Utils/EventEmitter.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
using namespace tkd;

///////////////////////////////////////////////////////////////////////////////
// Test Event Emitter Class Forward Declaration
///////////////////////////////////////////////////////////////////////////////
class TestEventEmitter;

///////////////////////////////////////////////////////////////////////////////
// Test Event Types
///////////////////////////////////////////////////////////////////////////////
struct TestEvent1
{
    int value;
};

struct TestEvent2
{
    std::string message;
};

struct TestEvent3
{
    double x, y;
};

///////////////////////////////////////////////////////////////////////////////
// Test Event Emitter Class
///////////////////////////////////////////////////////////////////////////////
class TestEventEmitter
    : public TEventEmitter<TestEvent1, TestEvent2, TestEvent3>
{
public:
    // Expose protected methods for testing
    template <typename T>
    void PublicEmitEvent(const T& eventData)
    {
        EmitEvent(eventData);
    }

    template <typename T>
    void PublicEmitEvent(T&& eventData)
    {
        EmitEvent(std::forward<T>(eventData));
    }

    template <typename T>
    void PublicTriggerEvent(const T& eventData)
    {
        TriggerEvent(eventData);
    }

    template <typename T>
    void PublicTriggerEvent(T&& eventData)
    {
        TriggerEvent(std::forward<T>(eventData));
    }

    template <typename T>
    void PublicEmit(const T& eventData)
    {
        Emit(eventData);
    }

    template <typename T>
    void PublicEmit(T&& eventData)
    {
        Emit(std::forward<T>(eventData));
    }
};

///////////////////////////////////////////////////////////////////////////////
// Test Cases
///////////////////////////////////////////////////////////////////////////////

TEST_CASE("EventEmitter Event class basic functionality", "[EventEmitter]")
{
    TestEventEmitter emitter;

    SECTION("Event construction and type checking")
    {
        TestEvent1 event1{ 42 };
        typename TestEventEmitter::Event e(event1);

        REQUIRE(e.Is<TestEvent1>());
        REQUIRE_FALSE(e.Is<TestEvent2>());
        REQUIRE_FALSE(e.Is<TestEvent3>());
    }

    SECTION("Event Get and GetIf")
    {
        TestEvent2 event2{ "hello" };
        typename TestEventEmitter::Event e(event2);

        REQUIRE(e.GetIf<TestEvent2>() != nullptr);
        REQUIRE(e.GetIf<TestEvent1>() == nullptr);

        const auto& constE = e;
        REQUIRE(constE.GetIf<TestEvent2>() != nullptr);
        REQUIRE(constE.GetIf<TestEvent1>() == nullptr);

        REQUIRE(e.Get<TestEvent2>().message == "hello");
        REQUIRE_THROWS(e.Get<TestEvent1>());
    }

    SECTION("Event Visit")
    {
        TestEvent3 event3{ 1.5, 2.5 };
        typename TestEventEmitter::Event e(event3);

        bool visited = false;
        e.Visit(
            [&](const auto& data)
            {
                using T = std::decay_t<decltype(data)>;
                if constexpr (std::is_same_v<T, TestEvent3>)
                {
                    REQUIRE(data.x == 1.5);
                    REQUIRE(data.y == 2.5);
                    visited = true;
                }
            }
        );
        REQUIRE(visited);
    }
}

TEST_CASE("EventEmitter event queue operations", "[EventEmitter]")
{
    TestEventEmitter emitter;

    SECTION("EmitEvent and PollEvent")
    {
        REQUIRE_FALSE(emitter.HasEvents());
        REQUIRE(emitter.GetEventCount() == 0);

        emitter.PublicEmitEvent(TestEvent1{ 100 });
        REQUIRE(emitter.HasEvents());
        REQUIRE(emitter.GetEventCount() == 1);

        auto event = emitter.PollEvent();
        REQUIRE(event.has_value());
        REQUIRE(event->Is<TestEvent1>());
        REQUIRE(event->Get<TestEvent1>().value == 100);

        REQUIRE_FALSE(emitter.HasEvents());
        REQUIRE(emitter.GetEventCount() == 0);
    }

    SECTION("Multiple events in queue")
    {
        emitter.PublicEmitEvent(TestEvent1{ 1 });
        emitter.PublicEmitEvent(TestEvent2{ "test" });
        emitter.PublicEmitEvent(TestEvent3{ 3.14, 2.71 });

        REQUIRE(emitter.GetEventCount() == 3);

        auto e1 = emitter.PollEvent();
        REQUIRE(e1->Is<TestEvent1>());
        REQUIRE(e1->Get<TestEvent1>().value == 1);

        auto e2 = emitter.PollEvent();
        REQUIRE(e2->Is<TestEvent2>());
        REQUIRE(e2->Get<TestEvent2>().message == "test");

        auto e3 = emitter.PollEvent();
        REQUIRE(e3->Is<TestEvent3>());
        REQUIRE(e3->Get<TestEvent3>().x == 3.14);
        REQUIRE(e3->Get<TestEvent3>().y == 2.71);

        REQUIRE_FALSE(emitter.HasEvents());
    }

    SECTION("ClearEvents")
    {
        emitter.PublicEmitEvent(TestEvent1{ 1 });
        emitter.PublicEmitEvent(TestEvent2{ "test" });

        REQUIRE(emitter.GetEventCount() == 2);

        emitter.ClearEvents();

        REQUIRE_FALSE(emitter.HasEvents());
        REQUIRE(emitter.GetEventCount() == 0);
    }

    SECTION("PollEvent on empty queue")
    {
        auto event = emitter.PollEvent();
        REQUIRE_FALSE(event.has_value());
    }
}

TEST_CASE("EventEmitter listener registration and removal", "[EventEmitter]")
{
    TestEventEmitter emitter;

    SECTION("On listener registration")
    {
        bool called = false;
        emitter.On<TestEvent1>(
            [&](const TestEvent1& e)
            {
                called = true;
                REQUIRE(e.value == 42);
            }
        );

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 1);

        emitter.PublicTriggerEvent(TestEvent1{ 42 });
        REQUIRE(called);

        // Listener should still be there
        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 1);
    }

    SECTION("Once listener registration")
    {
        int callCount = 0;
        emitter.Once<TestEvent2>(
            [&](const TestEvent2& e)
            {
                callCount++;
                REQUIRE(e.message == "once");
            }
        );

        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 1);

        emitter.PublicTriggerEvent(TestEvent2{ "once" });
        REQUIRE(callCount == 1);

        // Listener should be removed after first call
        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 0);

        // Second trigger should not call anything
        emitter.PublicTriggerEvent(TestEvent2{ "again" });
        REQUIRE(callCount == 1);
    }

    SECTION("RemoveListener")
    {
        bool called = false;
        auto handle =
            emitter.On<TestEvent1>([&](const TestEvent1&) { called = true; });

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 1);

        REQUIRE(emitter.RemoveListener(handle));
        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 0);

        emitter.PublicTriggerEvent(TestEvent1{ 1 });
        REQUIRE_FALSE(called);

        // Try to remove non-existent handle
        REQUIRE_FALSE(emitter.RemoveListener(999));
    }

    SECTION("RemoveAllListeners for specific type")
    {
        emitter.On<TestEvent1>([](const TestEvent1&) {});
        emitter.On<TestEvent1>([](const TestEvent1&) {});
        emitter.On<TestEvent2>([](const TestEvent2&) {});

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 2);
        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 1);

        emitter.RemoveAllListeners<TestEvent1>();

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 0);
        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 1);
    }

    SECTION("RemoveAllListeners for all types")
    {
        emitter.On<TestEvent1>([](const TestEvent1&) {});
        emitter.On<TestEvent2>([](const TestEvent2&) {});
        emitter.On<TestEvent3>([](const TestEvent3&) {});

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 1);
        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 1);
        REQUIRE(emitter.GetListenerCount<TestEvent3>() == 1);

        emitter.RemoveAllListeners();

        REQUIRE(emitter.GetListenerCount<TestEvent1>() == 0);
        REQUIRE(emitter.GetListenerCount<TestEvent2>() == 0);
        REQUIRE(emitter.GetListenerCount<TestEvent3>() == 0);
    }
}

TEST_CASE("EventEmitter Emit method (queue + trigger)", "[EventEmitter]")
{
    TestEventEmitter emitter;

    SECTION("Emit adds to queue and triggers listeners")
    {
        bool triggered = false;
        bool polled = false;

        emitter.On<TestEvent1>(
            [&](const TestEvent1& e)
            {
                triggered = true;
                REQUIRE(e.value == 123);
            }
        );

        emitter.PublicEmit(TestEvent1{ 123 });

        REQUIRE(triggered);
        REQUIRE(emitter.HasEvents());

        auto event = emitter.PollEvent();
        REQUIRE(event.has_value());
        REQUIRE(event->Is<TestEvent1>());
        REQUIRE(event->Get<TestEvent1>().value == 123);
        polled = true;

        REQUIRE(triggered);
        REQUIRE(polled);
    }
}

TEST_CASE("EventEmitter multiple listeners for same event", "[EventEmitter]")
{
    TestEventEmitter emitter;

    int callCount1 = 0;
    int callCount2 = 0;

    auto handle1 =
        emitter.On<TestEvent1>([&](const TestEvent1&) { callCount1++; });

    emitter.On<TestEvent1>([&](const TestEvent1&) { callCount2++; });

    emitter.PublicTriggerEvent(TestEvent1{ 1 });

    REQUIRE(callCount1 == 1);
    REQUIRE(callCount2 == 1);

    emitter.RemoveListener(handle1);
    emitter.PublicTriggerEvent(TestEvent1{ 2 });

    REQUIRE(callCount1 == 1);   // Should not be called again
    REQUIRE(callCount2 == 2);
}

TEST_CASE("EventEmitter listener handles are unique", "[EventEmitter]")
{
    TestEventEmitter emitter;

    auto handle1 = emitter.On<TestEvent1>([](const TestEvent1&) {});
    auto handle2 = emitter.On<TestEvent2>([](const TestEvent2&) {});

    REQUIRE(handle1 != handle2);
    REQUIRE(handle1 != 0);
    REQUIRE(handle2 != 0);
}

TEST_CASE("EventEmitter move semantics", "[EventEmitter]")
{
    TestEventEmitter emitter;

    std::string original = "moved message";
    std::string moved = std::move(original);

    emitter.PublicEmitEvent(TestEvent2{ std::move(moved) });
    auto event = emitter.PollEvent();

    REQUIRE(event->Get<TestEvent2>().message == "moved message");
    REQUIRE(original.empty());   // Original should be empty after move
}
