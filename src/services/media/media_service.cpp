#include "media_service.hpp"
#include "services/providers/OmdbMediaProvider.hpp"
#include "services/providers/tmdb_provider.hpp"
#include "core/utils/logger.hpp"
#include <fmt/format.h>
#include "../cache/cache_manager.hpp"
#include "../providers/utils/rating_normalizer.hpp"

namespace app::services
{

    MediaService &MediaService::Instance()
    {
        static MediaService instance;
        return instance;
    }

    bool MediaService::Initialize()
    {
        std::lock_guard<std::mutex> lock(providerMutex_);

        if (initialized_)
        { // 🛑 Prevent re-initialization
            return true;
        }

        try
        {
            utils::Logger::Info("Initializing MediaService...");
            LoadDefaultProviders();
            initialized_ = true;
            std::string msg = fmt::format("MediaService initialized with {} providers", providers_.size());
            utils::Logger::Info(msg);

            return true;
        }
        catch (const std::exception &e)
        {
            // Log the actual exception, not a Win32 error
            std::string errorMsg = fmt::format("MediaService initialization failed: {}", e.what());
            utils::Logger::Error(errorMsg);
            return false;
        }
    }

    void MediaService::Shutdown()
    {
        std::lock_guard<std::mutex> lock(providerMutex_);
        providers_.clear();
        initialized_ = false;
        utils::Logger::Info("MediaService shut down");
    }

    void MediaService::RegisterProvider(const std::string &providerId,
                                        std::unique_ptr<IMediaProvider> provider)
    {
        // std::lock_guard<std::mutex> lock(providerMutex_);
        providers_[providerId] = std::move(provider);
        utils::Logger::Info(fmt::format("Registered provider: {}", providerId));
    }

    void MediaService::LoadDefaultProviders()
    {
        // Add your API keys here
        RegisterProvider("tmdb", std::make_unique<TmdbProvider>("c9babff33d8b3805280b20c04b111497"));
        RegisterProvider("omdb", std::make_unique<OmdbMediaProvider>("b4e6b940"));
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    MediaService::UnifiedSearch(const std::string &query, const MediaFilter &filter, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
            std::vector<std::future<utils::Result<std::vector<domain::MediaMetadata>>>> futures;
            std::vector<domain::MediaMetadata> aggregated;
            
            // Check cache first
            auto cacheKey = fmt::format("search:{}:{}:{}", query, filter.ToString(), page);
            if (auto cached = cache::CacheManager::Instance().Get<std::vector<domain::MediaMetadata>>(cacheKey)) {
                return utils::Result<std::vector<domain::MediaMetadata>>(*cached);
            }
            
            // Parallel provider search
            {
                std::lock_guard<std::mutex> lock(providerMutex_);
                for (const auto& [id, provider] : providers_) {
                    if (provider->GetCapabilities().supportsSearch) {
                        futures.push_back(provider->SearchMedia(query, filter, page));
                    }
                }
            }
            
            // Aggregate results
            for (auto& future : futures) {
                try {
                    auto result = future.get();
                    if (result.IsOk()) {
                        for (auto resultItem : result.Value())
                        { // Non-const copy for modification
                            if (resultItem.rating)
                            {
                                resultItem.normalizedRating = services::RatingNormalizer::Instance()
                                                                  .NormalizeRating(resultItem.id.source, static_cast<float>(resultItem.rating));
                            }
                            aggregated.push_back(std::move(resultItem)); // Move copy into aggregated
                        }
                    }
                } catch (const std::exception& e) {
                    utils::Logger::Error("Provider search failed: " + std::string(e.what()));
                }
            }
            
            // Sort and cache results
            std::sort(aggregated.begin(), aggregated.end(),
                     [](const auto& a, const auto& b) { 
                         return a.normalizedRating > b.normalizedRating; 
                     });
            
            cache::CacheManager::Instance().Set(cacheKey, aggregated);
            return utils::Result<std::vector<domain::MediaMetadata>>(aggregated); });
    }
    // std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    // MediaService::UnifiedSearch(const std::string &query, int page)
    // {
    //     return std::async(std::launch::async, [=]()
    //                       {
    //     std::vector<std::future<utils::Result<std::vector<domain::MediaMetadata>>>> futures;

    //     {
    //         std::lock_guard<std::mutex> lock(providerMutex_);
    //         for(const auto& [id, provider] : providers_) {
    //             futures.push_back(provider->SearchMedia(query, page));
    //         }
    //     }

    //     std::vector<domain::MediaMetadata> aggregated;
    //     std::unordered_map<std::string, bool> seenIds;

    //     for(auto& future : futures) {
    //         try {
    //             auto result = future.get();
    //             if(result.IsOk()) {
    //                 for(const auto& item : result.Value()) {
    //                     const std::string uniqueId = item.id.source + ":" + item.id.id;
    //                     if(!seenIds[uniqueId]) {
    //                         aggregated.push_back(item);
    //                         seenIds[uniqueId] = true;
    //                     }
    //                 }
    //             }
    //         }
    //         catch(const std::exception& e) {
    //             DWORD error = GetLastError();
    //             utils::Logger::Error("Provider error: " + std::to_string(error));
    //         }
    //     }

    //     // Sort by descending popularity
    //     std::sort(aggregated.begin(), aggregated.end(),
    //         [](const auto& a, const auto& b) { return a.popularity > b.popularity; });

    //     return utils::Result<std::vector<domain::MediaMetadata>>(aggregated); });
    // }

} // namespace app::services