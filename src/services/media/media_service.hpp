#pragma once
#include "domain/models/media_types.hpp"
#include "core/utils/result.hpp"
#include <future>
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace app::services

{

    class IMediaProvider
    {
    public:
        virtual ~IMediaProvider() = default;
        virtual std::string GetProviderName() const = 0;        // Add this method
        virtual std::string GetProviderDescription() const = 0; // Add this for UI

        virtual std::future<utils::Result<std::vector<domain::MovieInfo>>>
        GetPopularMovies(int page = 1) = 0;

        virtual std::future<utils::Result<domain::MovieInfo>>
        GetMovieDetails(const std::string &id) = 0;

        virtual std::future<utils::Result<std::vector<domain::TvShowInfo>>>
        GetPopularTvShows(int page = 1) = 0;

        virtual std::future<utils::Result<domain::TvShowInfo>>
        GetTvShowDetails(const std::string &id) = 0;
    };

    class IMediaProviderPlugin
    {
    public:
        virtual ~IMediaProviderPlugin() = default;
        virtual std::string GetPluginName() const = 0;
        virtual std::string GetPluginVersion() const = 0;
        virtual std::unique_ptr<IMediaProvider> CreateProvider() = 0;
    };

    struct ProviderInfo
    {
        std::string name;
        std::string description;
        bool isDefault;
        std::function<std::unique_ptr<IMediaProvider>()> factory;
    };

    class MediaService
    {
    public:
        // Add these new methods
        void LoadPluginProvider(const std::string &pluginPath);
        void LoadNativeProvider(std::unique_ptr<IMediaProviderPlugin> plugin);

        static MediaService &Instance();

        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return !providers_.empty(); }

        // Provider management
        void RegisterProvider(const std::string &providerId,
                              const ProviderInfo &info);
        bool SetActiveProvider(const std::string &providerId);
        std::string GetActiveProviderId() const { return activeProviderId_; }
        std::vector<std::pair<std::string, ProviderInfo>> GetAvailableProviders() const;

        // Media Provider System
        std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        SearchMedia(const std::string &query, domain::MediaType filterType, const std::string &genre, int page);

        std::future<utils::Result<std::vector<std::string>>>
        GetAvailableGenres();

        std::future<utils::Result<domain::MediaDetails>>
        GetEnhancedDetails(const std::string &mediaId);

        std::future<utils::Result<std::vector<domain::MovieInfo>>>
        GetPopularMovies(int page = 1);

        std::future<utils::Result<std::vector<domain::TvShowInfo>>>
        GetPopularTvShows(int page = 1);

        std::future<utils::Result<domain::MovieInfo>>
        GetMovieDetails(const std::string &id);

        std::future<utils::Result<domain::TvShowInfo>>
        GetTvShowDetails(const std::string &id);

    private:
        MediaService() = default;

        std::map<std::string, ProviderInfo> providerRegistry_;
        std::map<std::string, std::unique_ptr<IMediaProvider>> providers_;
        std::vector<std::unique_ptr<IMediaProviderPlugin>> plugins_; // Add this line
        std::string activeProviderId_;
        std::string defaultProviderId_;

        IMediaProvider *GetActiveProvider();

        // Add these helper methods
        std::vector<domain::MediaMetadata> DeduplicateResults(
            const std::vector<std::vector<domain::MediaMetadata>> &providerResults);

        domain::MediaDetails AggregateDetails(
            const std::vector<domain::MediaDetails> &providerDetails);
    };
}