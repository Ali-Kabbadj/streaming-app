namespace app::events
{
    template <typename EventType>
    class EventDispatcher
    {
    public:
        using HandlerId = uint64_t;
        using Handler = std::function<void(const EventType &)>;

        HandlerId Subscribe(Handler handler)
        {
            const auto id = nextHandlerId_++;
            handlers_[id] = std::move(handler);
            return id;
        }

        void Unsubscribe(HandlerId id)
        {
            handlers_.erase(id);
        }

        void Dispatch(const EventType &event)
        {
            for (const auto &[id, handler] : handlers_)
            {
                handler(event);
            }
        }

    private:
        std::unordered_map<HandlerId, Handler> handlers_;
        static inline std::atomic<HandlerId> nextHandlerId_{0};
    };
}