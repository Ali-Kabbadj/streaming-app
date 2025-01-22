#include "cache_manager.hpp"

namespace app::cache
{
    CacheManager &CacheManager::Instance()
    {
        static CacheManager instance; // Singleton instance
        return instance;
    }
}