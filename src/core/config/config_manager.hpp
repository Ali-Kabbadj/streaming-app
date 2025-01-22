#include <string>
#include <unordered_map>
#include <variant>
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

        template <typename T>
        utils::Result<T> Get(const std::string &key) const
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
        void Set(const std::string &key, const T &value)
        {
            settings_[key] = value;
        }

    private:
        std::unordered_map<std::string, std::variant<std::string, int, bool>> settings_;
        ConfigManager() = default;
    };
}