#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include "utils/result.hpp"

namespace app::config
{
    class ConfigManager
    {
    public:
        static ConfigManager &Instance()
        {
            static ConfigManager instance;
            return instance;
        }

        // Load configuration from a specific file
        utils::Result<void> LoadFromFile(const std::string &filePath);

        // Load configuration from the AppData directory
        utils::Result<void> LoadFromAppData(const std::string &appName);

        // Save configuration to a specific file
        utils::Result<void> SaveToFile(const std::string &filePath);

        // Save configuration to the AppData directory
        utils::Result<void> SaveToAppData(const std::string &appName);

        // Get a configuration value
        template <typename T>
        utils::Result<T> Get(const std::string &key) const;

        // Get a configuration value with a default fallback
        template <typename T>
        T GetOrDefault(const std::string &key, const T &defaultValue) const;

        // Set a configuration value
        template <typename T>
        void Set(const std::string &key, const T &value);

    private:
        std::unordered_map<std::string, std::variant<std::string, int, bool>> settings_;
        ConfigManager() = default;
    };
}