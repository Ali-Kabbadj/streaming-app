#include "utils/logger.hpp"

namespace app::utils
{

    std::shared_ptr<spdlog::logger> Logger::logger_;

    void Logger::Initialize()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/app.log", true);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        logger_ = std::make_shared<spdlog::logger>("app", sinks.begin(), sinks.end());

        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::debug);

        spdlog::register_logger(logger_);
    }

    void Logger::Shutdown()
    {
        spdlog::shutdown();
    }

    void Logger::Info(const std::string &message)
    {
        if (logger_)
        {
            logger_->info(message);
        }
    }

    void Logger::Error(const std::string &message)
    {
        if (logger_)
        {
            logger_->error(message);
        }
    }

    void Logger::Debug(const std::string &message)
    {
        if (logger_)
        {
            logger_->debug(message);
        }
    }

    void Logger::Warning(const std::string &message)
    {
        if (logger_)
        {
            logger_->warn(message);
        }
    }

} // namespace app::utils