#pragma once
#include <memory>
#include <string>
#include <source_location>

namespace spdlog
{
    class logger;
}

namespace app::utils
{

    class Logger
    {
    public:
        static void Initialize(const std::string &logPath = "logs");
        static void Shutdown();

        static void Info(const std::string &message,
                         const std::source_location &location = std::source_location::current());
        static void Error(const std::string &message,
                          const std::source_location &location = std::source_location::current());
        static void Debug(const std::string &message,
                          const std::source_location &location = std::source_location::current());
        static void Warning(const std::string &message,
                            const std::source_location &location = std::source_location::current());

    private:
        static std::shared_ptr<spdlog::logger> logger_;
        static std::string FormatMessage(const std::string &message, const std::source_location &location);
    };

} // namespace app::utils