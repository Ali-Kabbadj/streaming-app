#include <optional>
#include <string>
#include <chrono>
#include "core/utils/lru_cache.hpp"
#include <any>

namespace app::cache
{
    class CacheManager
    {
    public:
        static CacheManager &Instance();

        template <typename T>
        std::optional<T> Get(const std::string &key)
        {
            auto value = cache_.get(key); // Retrieve the value from the cache
            if (value.has_value())
            { // Check if the value exists
                try
                {
                    return std::any_cast<T>(value.value()); // Attempt to cast to T
                }
                catch (const std::bad_any_cast &)
                {
                    return std::nullopt; // Return empty if the cast fails
                }
            }
            return std::nullopt; // Return empty if the value doesn't exist
        }

        template <typename T>
        void Set(const std::string &key, const T &value,
                 std::chrono::seconds ttl = std::chrono::seconds(3600))
        {
            cache_.put(key, value, ttl);
        }

    private:
        utils::LRUCache<std::string, std::any> cache_;
    };
}