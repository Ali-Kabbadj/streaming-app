namespace app::logging
{
    // core/logging/structured_logger.hpp
    class StructuredLogger
    {
    public:
        template <typename... Args>
        static void LogOperation(
            const char *operation,
            const char *category,
            LogLevel level,
            const char *format,
            Args &&...args)
        {
            json logEntry = {
                {"operation", operation},
                {"category", category},
                {"level", ToString(level)},
                {"timestamp", CurrentTimestamp()},
                {"message", fmt::format(format, std::forward<Args>(args)...)}};
            LogImpl(logEntry);
        }
    };
}