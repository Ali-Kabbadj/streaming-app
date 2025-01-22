namespace app::events
{
    // core/events/event_system.hpp
    template <typename EventType>
    class EventDispatcher
    {
    public:
        using HandlerId = uint64_t;
        using Handler = std::function<void(const EventType &)>;

        HandlerId Subscribe(Handler handler);
        void Unsubscribe(HandlerId id);
        void Dispatch(const EventType &event);

    private:
        std::unordered_map<HandlerId, Handler> handlers_;
        // ... implementation
    };
}