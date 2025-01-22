// #pragma once
// #include <string>
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>

// namespace app::utils
// {
//     class Logger
//     {
//     public:
//         static void Initialize();
//         static void Shutdown();

//         static void Info(const std::string &message);
//         static void Error(const std::string &message);
//         static void Debug(const std::string &message);
//         static void Warning(const std::string &message);

//     private:
//         static std::shared_ptr<spdlog::logger> logger_;
//     };
// }

#include "logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>

namespace app::utils
{

    std::shared_ptr<spdlog::logger> Logger::logger_;

    void Logger::Initialize(const std::string &logPath)
    {
        try
        {
            // Create logs directory if it doesn't exist
            std::filesystem::create_directories(logPath);

            // Setup sinks
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logPath + "/app.log",
                1024 * 1024 * 5, // 5MB size
                3                // Keep 3 files
            );

            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            logger_ = std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());

            logger_->set_level(spdlog::level::debug);
            logger_->flush_on(spdlog::level::debug);

            spdlog::register_logger(logger_);

            Info("Logger initialized");
        }
        catch (const std::exception &ex)
        {
            // If logger fails to initialize, try to at least write to stderr
            std::cerr << "Failed to initialize logger: " << ex.what() << std::endl;
        }
    }

    void Logger::Shutdown()
    {
        if (logger_)
        {
            Info("Logger shutting down");
            spdlog::shutdown();
        }
    }

    std::string Logger::FormatMessage(const std::string &message, const std::source_location &location)
    {
        return fmt::format("[{}:{}] {}",
                           std::filesystem::path(location.file_name()).filename().string(),
                           location.line(),
                           message);
    }

    void Logger::Info(const std::string &message, const std::source_location &location)
    {
        if (logger_)
        {
            logger_->info(FormatMessage(message, location));
        }
    }

    void Logger::Error(const std::string &message, const std::source_location &location)
    {
        if (logger_)
        {
            logger_->error(FormatMessage(message, location));
        }
    }

    void Logger::Debug(const std::string &message, const std::source_location &location)
    {
        if (logger_)
        {
            logger_->debug(FormatMessage(message, location));
        }
    }

    void Logger::Warning(const std::string &message, const std::source_location &location)
    {
        if (logger_)
        {
            logger_->warn(FormatMessage(message, location));
        }
    }

} // namespace app::utils