#include "media_service.hpp"
#include "services/providers/OmdbMediaProvider.hpp"
#include "services/providers/tmdb_provider.hpp"
#include "core/utils/logger.hpp"

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

        try
        {
            utils::Logger::Info("Initializing MediaService...");
            LoadDefaultProviders();
            initialized_ = true;
            utils::Logger::Info("MediaService initialized with {} providers");
            return true;
        }
        catch (const std::exception &e)
        {
            DWORD error = GetLastError();
            utils::Logger::Error("CreateWindowExW failed with error: " + std::to_string(error));
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
        utils::Logger::Info("Registered provider: {}");
    }

    void MediaService::LoadDefaultProviders()
    {
        // Add your API keys here
        RegisterProvider("tmdb", std::make_unique<TmdbProvider>("your_tmdb_key"));
        RegisterProvider("omdb", std::make_unique<OmdbMediaProvider>("your_omdb_key"));
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    MediaService::UnifiedSearch(const std::string &query, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
        std::vector<std::future<utils::Result<std::vector<domain::MediaMetadata>>>> futures;
        
        {
            std::lock_guard<std::mutex> lock(providerMutex_);
            for(const auto& [id, provider] : providers_) {
                futures.push_back(provider->SearchMedia(query, page));
            }
        }

        std::vector<domain::MediaMetadata> aggregated;
        std::unordered_map<std::string, bool> seenIds;
        
        for(auto& future : futures) {
            try {
                auto result = future.get();
                if(result.IsOk()) {
                    for(const auto& item : result.Value()) {
                        const std::string uniqueId = item.id.source + ":" + item.id.id;
                        if(!seenIds[uniqueId]) {
                            aggregated.push_back(item);
                            seenIds[uniqueId] = true;
                        }
                    }
                }
            }
            catch(const std::exception& e) {
                DWORD error = GetLastError();
                utils::Logger::Error("Provider error: " + std::to_string(error));
            }
        }

        // Sort by descending popularity
        std::sort(aggregated.begin(), aggregated.end(),
            [](const auto& a, const auto& b) { return a.popularity > b.popularity; });

        return utils::Result<std::vector<domain::MediaMetadata>>(aggregated); });
    }

} // namespace app::services