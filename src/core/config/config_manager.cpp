#include "core/config/config_manager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include "utils/win32_utils.hpp" // For GetAppDataDirectory
#include "utils/logger.hpp"

namespace app::config
{
    utils::Result<void> ConfigManager::LoadFromFile(const std::string &filePath)
    {
        try
        {
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                return utils::Result<void>::Error("Failed to open config file: " + filePath);
            }

            nlohmann::json json;
            file >> json;
            file.close();

            // Parse the JSON and store key-value pairs
            for (auto &[key, value] : json.items())
            {
                if (value.is_null())
                {
                    utils::Logger::Warning("Skipping null value for key: " + key);
                    continue; // Skip null values
                }

                if (value.is_string())
                {
                    settings_[key] = value.get<std::string>();
                }
                else if (value.is_number_integer())
                {
                    settings_[key] = value.get<int>();
                }
                else if (value.is_boolean())
                {
                    settings_[key] = value.get<bool>();
                }
                else if (value.is_object())
                {
                    // Handle nested objects (e.g., "webview" or "window")
                    for (auto &[nestedKey, nestedValue] : value.items())
                    {
                        std::string fullKey = key + "." + nestedKey;
                        if (nestedValue.is_null())
                        {
                            utils::Logger::Warning("Skipping null value for key: " + fullKey);
                            continue; // Skip null values
                        }

                        if (nestedValue.is_string())
                        {
                            settings_[fullKey] = nestedValue.get<std::string>();
                        }
                        else if (nestedValue.is_number_integer())
                        {
                            settings_[fullKey] = nestedValue.get<int>();
                        }
                        else if (nestedValue.is_boolean())
                        {
                            settings_[fullKey] = nestedValue.get<bool>();
                        }
                        else
                        {
                            return utils::Result<void>::Error("Unsupported type for key: " + fullKey);
                        }
                    }
                }
                else
                {
                    return utils::Result<void>::Error("Unsupported type for key: " + key);
                }
            }

            return utils::Result<void>();
        }
        catch (const std::exception &e)
        {
            return utils::Result<void>::Error("Failed to load config file: " + std::string(e.what()));
        }
    }

    utils::Result<void> ConfigManager::LoadFromAppData(const std::string &appName)
    {
        std::string appDataDir = utils::GetAppDataDirectory(appName);
        std::string configFilePath = appDataDir + "/app_config.json";

        // Check if the config file exists
        if (!std::filesystem::exists(configFilePath))
        {
            // Create default configuration
            std::ofstream file(configFilePath);
            if (!file.is_open())
            {
                return utils::Result<void>::Error("Failed to create default config file: " + configFilePath);
            }

            // Set the providers_dir to the AppData directory
            std::string providersDir = appDataDir + "\\config\\providers\\";

            nlohmann::json defaultConfig = {
                {"providers_dir", providersDir},
                {"window", {{"width", 1280}, {"height", 720}}},
                {"webview", {{"url", "http://localhost:3000"}}}};

            utils::Logger::Info("Loading providers from: " + defaultConfig["providersDir"]);

            if (!std::filesystem::exists(providersDir))
            {
                utils::Logger::Error("Providers directory does not exist: " + providersDir);
                std::filesystem::create_directories(providersDir);
                utils::Logger::Info("Created Providers dir at" + providersDir);
            }

            file << defaultConfig.dump(4); // Pretty-print with 4 spaces
            file.close();
        }

        return LoadFromFile(configFilePath);
    }

    utils::Result<void> ConfigManager::SaveToFile(const std::string &filePath)
    {
        try
        {
            nlohmann::json json;

            // Convert settings to JSON
            for (const auto &[key, value] : settings_)
            {
                if (std::holds_alternative<std::string>(value))
                {
                    json[key] = std::get<std::string>(value);
                }
                else if (std::holds_alternative<int>(value))
                {
                    json[key] = std::get<int>(value);
                }
                else if (std::holds_alternative<bool>(value))
                {
                    json[key] = std::get<bool>(value);
                }
            }

            // Write JSON to file
            std::ofstream file(filePath);
            if (!file.is_open())
            {
                return utils::Result<void>::Error("Failed to open config file for writing: " + filePath);
            }

            file << json.dump(4); // Pretty-print with 4 spaces
            file.close();

            return utils::Result<void>();
        }
        catch (const std::exception &e)
        {
            return utils::Result<void>::Error("Failed to save config file: " + std::string(e.what()));
        }
    }

    utils::Result<void> ConfigManager::SaveToAppData(const std::string &appName)
    {
        std::string appDataDir = utils::GetAppDataDirectory(appName);
        std::string configFilePath = appDataDir + "/app_config.json";
        return SaveToFile(configFilePath);
    }

    // Template implementations for Get and GetOrDefault
    template <typename T>
    utils::Result<T> ConfigManager::Get(const std::string &key) const
    {
        auto it = settings_.find(key);
        if (it == settings_.end())
        {
            return utils::Result<T>::Error("Configuration key not found: " + key);
        }

        try
        {
            return utils::Result<T>(std::get<T>(it->second));
        }
        catch (const std::bad_variant_access &)
        {
            return utils::Result<T>::Error("Type mismatch for key: " + key);
        }
    }

    template <typename T>
    T ConfigManager::GetOrDefault(const std::string &key, const T &defaultValue) const
    {
        auto result = Get<T>(key);
        return result.IsOk() ? result.Value() : defaultValue;
    }

    // Explicit template instantiations
    template utils::Result<std::string> ConfigManager::Get<std::string>(const std::string &key) const;
    template utils::Result<int> ConfigManager::Get<int>(const std::string &key) const;
    template utils::Result<bool> ConfigManager::Get<bool>(const std::string &key) const;

    template std::string ConfigManager::GetOrDefault<std::string>(const std::string &key, const std::string &defaultValue) const;
    template int ConfigManager::GetOrDefault<int>(const std::string &key, const int &defaultValue) const;
    template bool ConfigManager::GetOrDefault<bool>(const std::string &key, const bool &defaultValue) const;
}