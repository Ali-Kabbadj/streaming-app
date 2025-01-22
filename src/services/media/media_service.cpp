

#include "media_service.hpp"
#include "services/providers/GenericProvider.hpp"
#include "services/providers/provider_repository.hpp"
#include "core/utils/logger.hpp"
#include <fmt/format.h>
#include "../cache/cache_manager.hpp"
#include "utils/rating_normalizer.hpp"

namespace app::services
{
    MediaService &MediaService::Instance()
    {
        static MediaService instance;
        return instance;
    }

    bool MediaService::Initialize(const std::string &providersDir)
    {
        try
        {
            // Ensure the providers directory exists
            if (!std::filesystem::exists(providersDir))
            {
                utils::Logger::Error("Providers directory does not exist: " + providersDir);
                return false;
            }

            // Load providers from the specified directory
            ProviderRepository::Instance().LoadProvidersFromManifests(providersDir);
            return true;
        }
        catch (const std::exception &e)
        {
            utils::Logger::Error("MediaService initialization failed: " + std::string(e.what()));
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
        std::lock_guard<std::mutex> lock(providerMutex_);
        providers_[providerId] = std::move(provider);
        utils::Logger::Info(fmt::format("Registered provider: {}", providerId));
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    MediaService::UnifiedSearch(const std::string &query, const std::string &catalogType, const MediaFilter &filter, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
        try {
            // Step 1: Check the cache first
            auto cacheKey = fmt::format("catalog:{}:{}:{}", catalogType, filter.ToString(), page);
            if (auto cached = cache::CacheManager::Instance().Get<std::vector<domain::MediaMetadata>>(cacheKey)) {
                utils::Logger::Info("Returning cached results for key: " + cacheKey);
                return utils::Result<std::vector<domain::MediaMetadata>>(*cached);
            }

            // Step 2: Use catalog if no query is provided
            std::vector<std::future<utils::Result<std::vector<domain::MediaMetadata>>>> futures;
            {
                std::lock_guard<std::mutex> lock(providerMutex_);
                for (const auto& [id, provider] : providers_) {
                    if (query.empty() && provider->GetCapabilities().supportsCatalog) {
                        // Use catalog for popular movies
                        futures.push_back(provider->GetCatalog(catalogType, filter, page));
                    } else if (provider->GetCapabilities().supportsSearch) {
                        // Use search if a query is provided
                        futures.push_back(provider->SearchMedia(query, filter, page));
                    }
                }
            }

            // Step 3: Aggregate results
            std::vector<domain::MediaMetadata> aggregated;
            for (auto& future : futures) {
                try {
                    auto result = future.get();
                    if (result.IsOk()) {
                        for (auto& resultItem : result.Value()) {
                            // Normalize ratings if available
                            if (resultItem.rating) {
                                resultItem.normalizedRating = services::RatingNormalizer::Instance()
                                    .NormalizeRating(resultItem.id.source, static_cast<float>(resultItem.rating));
                            }
                            aggregated.push_back(std::move(resultItem));
                        }
                    } else {
                        utils::Logger::Error("Provider search/catalog failed: " + result.GetError().message);
                    }
                } catch (const std::exception& e) {
                    utils::Logger::Error("Provider search/catalog exception: " + std::string(e.what()));
                }
            }

            // Step 4: Sort results by normalized rating
            std::sort(aggregated.begin(), aggregated.end(), [](const auto& a, const auto& b) {
                return a.normalizedRating > b.normalizedRating;
            });

            // Step 5: Cache the results
            cache::CacheManager::Instance().Set(cacheKey, aggregated);

            // Step 6: Return the aggregated results
            return utils::Result<std::vector<domain::MediaMetadata>>(aggregated);
        } catch (const std::exception& e) {
            utils::Logger::Error("UnifiedSearch exception: " + std::string(e.what()));
            return utils::Result<std::vector<domain::MediaMetadata>>(
                utils::Result<std::vector<domain::MediaMetadata>>::Error(e.what())
            );
        } });
    }
} // namespace app::services