// core/config/config_manager.hpp

#include <unordered_map>
namespace app::congig
{
    // core/config/config_manager.hpp
    class ConfigManager
    {
    public:
        static ConfigManager &Instance();

        template <typename T>
        Result<T> Get(const std::string &key) const;

        template <typename T>
        void Set(const std::string &key, const T &value);

    private:
        std::unordered_map<std::string, std::variant<std::string, int, bool>> settings_;
        // ... implementation
    };
}