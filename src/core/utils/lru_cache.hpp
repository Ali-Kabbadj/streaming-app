#pragma once
#include <list>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <chrono>

namespace app::utils
{

    template <typename K, typename V>
    class LRUCache
    {
    private:
        struct CacheEntry
        {
            V value;
            std::chrono::system_clock::time_point expiresAt;
        };

        using ListIterator = typename std::list<std::pair<K, CacheEntry>>::iterator;

        size_t capacity_;
        std::list<std::pair<K, CacheEntry>> items_;
        std::unordered_map<K, ListIterator> cache_;
        mutable std::mutex mutex_;

        void cleanup()
        {
            auto now = std::chrono::system_clock::now();
            auto it = items_.begin();
            while (it != items_.end())
            {
                if (it->second.expiresAt <= now)
                {
                    cache_.erase(it->first);
                    it = items_.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

    public:
        explicit LRUCache(size_t capacity = 1000) : capacity_(capacity) {}

        std::optional<V> get(const K &key)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cleanup();

            auto it = cache_.find(key);
            if (it == cache_.end())
            {
                return std::nullopt;
            }

            if (it->second->second.expiresAt <= std::chrono::system_clock::now())
            {
                cache_.erase(it);
                items_.erase(it->second);
                return std::nullopt;
            }

            // Move to front (most recently used)
            items_.splice(items_.begin(), items_, it->second);
            return it->second->second.value;
        }

        void put(const K &key, const V &value,
                 std::chrono::seconds ttl = std::chrono::seconds(3600))
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cleanup();

            auto expiresAt = std::chrono::system_clock::now() + ttl;

            // If key exists, update value and move to front
            auto it = cache_.find(key);
            if (it != cache_.end())
            {
                items_.erase(it->second);
                cache_.erase(it);
            }

            // Insert new item at front
            items_.emplace_front(key, CacheEntry{value, expiresAt});
            cache_[key] = items_.begin();

            // Remove oldest if over capacity
            if (cache_.size() > capacity_)
            {
                auto last = items_.end();
                --last;
                cache_.erase(last->first);
                items_.pop_back();
            }
        }

        void remove(const K &key)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = cache_.find(key);
            if (it != cache_.end())
            {
                items_.erase(it->second);
                cache_.erase(it);
            }
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            items_.clear();
            cache_.clear();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cleanup();
            return cache_.size();
        }

        bool contains(const K &key)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cleanup();
            return cache_.find(key) != cache_.end();
        }
    };

} // namespace app::utils