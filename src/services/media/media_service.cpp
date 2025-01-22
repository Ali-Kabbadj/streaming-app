#include "media_service.hpp"
#include "core/utils/logger.hpp"
#include "services/providers/omdbMediaProvider.hpp"

namespace app::services
{
    MediaService &MediaService::Instance()
    {
        static MediaService instance;
        return instance;
    }

    class MockMediaProvider : public IMediaProvider
    {
    public:
        std::string GetProviderName() const override
        {
            return "Mock Provider";
        }

        std::string GetProviderDescription() const override
        {
            return "Mock provider for testing purposes";
        }

        std::future<utils::Result<std::vector<domain::MovieInfo>>>
        GetPopularMovies(int page = 1) override
        {
            return std::async(std::launch::async, [page]()
                              {
            std::vector<domain::MovieInfo> movies;
            // Add some mock movies
            domain::MovieInfo movie;
            movie.title = "Mock Movie";
            movie.overview = "This is a mock movie for testing";
            movie.rating = 8.5;
            movie.voteCount = 1000;
            movie.imdbId = "tt0000001";
            movies.push_back(movie);
            
            return utils::Result<std::vector<domain::MovieInfo>>(std::move(movies)); });
        }

        std::future<utils::Result<domain::MovieInfo>>
        GetMovieDetails(const std::string &id) override
        {
            return std::async(std::launch::async, [id]()
                              {
            domain::MovieInfo movie;
            movie.title = "Mock Movie Details";
            movie.overview = "This is a mock movie detail for testing";
            movie.imdbId = id;
            movie.rating = 8.5;
            movie.voteCount = 1000;
            
            return utils::Result<domain::MovieInfo>(std::move(movie)); });
        }

        std::future<utils::Result<std::vector<domain::TvShowInfo>>>
        GetPopularTvShows(int page = 1) override
        {
            return std::async(std::launch::async, [page]()
                              {
            std::vector<domain::TvShowInfo> shows;
            domain::TvShowInfo show;
            show.title = "Mock TV Show";
            show.overview = "This is a mock TV show for testing";
            show.rating = 8.5;
            show.voteCount = 1000;
            show.imdbId = "tt0000002";
            show.numberOfSeasons = 3;
            shows.push_back(show);
            
            return utils::Result<std::vector<domain::TvShowInfo>>(std::move(shows)); });
        }

        std::future<utils::Result<domain::TvShowInfo>>
        GetTvShowDetails(const std::string &id) override
        {
            return std::async(std::launch::async, [id]()
                              {
            domain::TvShowInfo show;
            show.title = "Mock TV Show Details";
            show.overview = "This is a mock TV show detail for testing";
            show.imdbId = id;
            show.rating = 8.5;
            show.voteCount = 1000;
            show.numberOfSeasons = 3;
            
            return utils::Result<domain::TvShowInfo>(std::move(show)); });
        }
    };

    bool MediaService::Initialize()
    {
        try
        {
            app::utils::Logger::Info("Initializing MediaService...");

            // Register built-in providers
            RegisterProvider("mock", {"Mock Provider",
                                      "Mock provider for testing",
                                      false, // Changed to false since OMDB will be default
                                      []()
                                      { return std::make_unique<MockMediaProvider>(); }});

            plugins_.push_back(std::move(omdbPlugin));

            // Initialize all providers
            for (const auto &[id, info] : providerRegistry_)
            {
                providers_[id] = info.factory();
                if (info.isDefault)
                {
                    defaultProviderId_ = id;
                    activeProviderId_ = id;
                }
            }

            app::utils::Logger::Info("MediaService initialized successfully.");
            return true;
        }
        catch (const std::exception &e)
        {
            app::utils::Logger::Error("Failed to initialize MediaService: " + std::string(e.what()));
            return false;
        }
    }

    void MediaService::RegisterProvider(const std::string &providerId,
                                        const ProviderInfo &info)
    {
        providerRegistry_[providerId] = info;
        app::utils::Logger::Info("Registered provider: " + providerId);
    }

    bool MediaService::SetActiveProvider(const std::string &providerId)
    {
        if (providers_.find(providerId) != providers_.end())
        {
            activeProviderId_ = providerId;
            app::utils::Logger::Info("Switched to provider: " + providerId);
            return true;
        }
        return false;
    }

    std::vector<std::pair<std::string, ProviderInfo>> MediaService::GetAvailableProviders() const
    {
        std::vector<std::pair<std::string, ProviderInfo>> result;
        for (const auto &[id, info] : providerRegistry_)
        {
            result.push_back({id, info});
        }
        return result;
    }

    IMediaProvider *MediaService::GetActiveProvider()
    {
        auto it = providers_.find(activeProviderId_);
        if (it == providers_.end())
        {
            it = providers_.find(defaultProviderId_);
            if (it == providers_.end())
            {
                return nullptr;
            }
        }
        return it->second.get();
    }

    void MediaService::Shutdown()
    {
        app::utils::Logger::Info("Shutting down MediaService...");
        providers_.clear();
        providerRegistry_.clear();
        app::utils::Logger::Info("MediaService shut down successfully.");
    }

    // Existing interface implementations now use GetActiveProvider()
    std::future<utils::Result<std::vector<domain::MovieInfo>>>
    MediaService::GetPopularMovies(int page)
    {
        auto provider = GetActiveProvider();
        if (!provider)
        {
            return std::async(std::launch::async, []
                              {
            app::utils::Logger::Error("MediaService::GetPopularMovies called before initialization.");
            return utils::Result<std::vector<domain::MovieInfo>>(
                utils::Result<std::vector<domain::MovieInfo>>::Error("MediaService is not initialized.")); });
        }
        return provider->GetPopularMovies(page);
    }

}