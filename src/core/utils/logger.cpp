#pragma once
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace app::utils
{
    class Logger
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void Info(const std::string &message);
        static void Error(const std::string &message);
        static void Debug(const std::string &message);
        static void Warning(const std::string &message);

    private:
        static std::shared_ptr<spdlog::logger> logger_;
    };
}