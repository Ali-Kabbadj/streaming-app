#pragma once
#include "domain/models/media_types.hpp"
#include "core/utils/result.hpp"
#include <future>
#include <memory>
#include <vector>

namespace app::services
{

    class IMediaProvider
    {
    public:
        virtual ~IMediaProvider() = default;

        virtual std::future<utils::Result<std::vector<domain::MovieInfo>>>
        GetPopularMovies(int page = 1) = 0;

        virtual std::future<utils::Result<std::vector<domain::TvShowInfo>>>
        GetPopularTvShows(int page = 1) = 0;

        virtual std::future<utils::Result<domain::MovieInfo>>
        GetMovieDetails(const std::string &id) = 0;

        virtual std::future<utils::Result<domain::TvShowInfo>>
        GetTvShowDetails(const std::string &id) = 0;
    };

    // Mock provider for development
    class MockMediaProvider : public IMediaProvider
    {
    public:
        std::future<utils::Result<std::vector<domain::MovieInfo>>>
        GetPopularMovies(int page = 1) override;

        std::future<utils::Result<std::vector<domain::TvShowInfo>>>
        GetPopularTvShows(int page = 1) override;

        std::future<utils::Result<domain::MovieInfo>>
        GetMovieDetails(const std::string &id) override;

        std::future<utils::Result<domain::TvShowInfo>>
        GetTvShowDetails(const std::string &id) override;
    };

    class MediaService
    {
    public:
        static MediaService &Instance();

        void Initialize();
        void Shutdown();

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
        std::unique_ptr<IMediaProvider> provider_;
    };

} // namespace app::services