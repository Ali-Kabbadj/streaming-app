#include "media_service.hpp"
#include "core/utils/logger.hpp"
#include <random>
#include <thread>

namespace app::services
{
    std::future<utils::Result<std::vector<domain::TvShowInfo>>>
    MockMediaProvider::GetPopularTvShows(int page)
    {
        return std::async(std::launch::async, [page]()
                          {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            std::vector<domain::TvShowInfo> shows;
            for (int i = 1; i <= 20; ++i) {
                domain::TvShowInfo show;
                show.title = "Mock TV Show " + std::to_string((page - 1) * 20 + i);
                show.overview = "This is a mock TV show description for testing purposes.";
                show.rating = ratingDist(gen);
                show.voteCount = voteDist(gen);
                show.numberOfSeasons = i % 10 + 1;
                show.numberOfEpisodes = show.numberOfSeasons * 12;
                show.status = "Continuing";
                show.releaseDate = std::chrono::system_clock::now() - 
                                 std::chrono::hours(24 * 365 * (i % 5));
                show.genres = {MOCK_GENRES[i % MOCK_GENRES.size()]};
                shows.push_back(std::move(show));
            }
            
            return utils::Result<std::vector<domain::TvShowInfo>>(std::move(shows)); });
    }

    std::future<utils::Result<domain::MovieInfo>>
    MockMediaProvider::GetMovieDetails(const std::string &id)
    {
        return std::async(std::launch::async, [id]()
                          {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            
            domain::MovieInfo movie;
            movie.title = "Movie " + id;
            movie.overview = "Detailed description for movie " + id;
            movie.rating = ratingDist(gen);
            movie.voteCount = voteDist(gen);
            movie.runtime = runtimeDist(gen);
            movie.releaseDate = std::chrono::system_clock::now() - 
                               std::chrono::hours(24 * 365 * 2);
            movie.genres = {"Action", "Drama"};
            movie.director = MOCK_DIRECTORS[0];
            movie.cast = {MOCK_ACTORS[0], MOCK_ACTORS[1]};
            movie.imdbId = "tt" + id;
            
            return utils::Result<domain::MovieInfo>(std::move(movie)); });
    }

    std::future<utils::Result<domain::TvShowInfo>>
    MockMediaProvider::GetTvShowDetails(const std::string &id)
    {
        return std::async(std::launch::async, [id]()
                          {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            
            domain::TvShowInfo show;
            show.title = "TV Show " + id;
            show.overview = "Detailed description for TV show " + id;
            show.rating = ratingDist(gen);
            show.voteCount = voteDist(gen);
            show.numberOfSeasons = 5;
            show.numberOfEpisodes = 60;
            show.status = "Continuing";
            show.releaseDate = std::chrono::system_clock::now() - 
                             std::chrono::hours(24 * 365 * 2);
            show.genres = {"Drama", "Thriller"};
            
            // Add some episodes
            for (int i = 1; i <= 10; ++i) {
                domain::EpisodeInfo episode;
                episode.title = "Episode " + std::to_string(i);
                episode.overview = "Episode description " + std::to_string(i);
                episode.episodeNumber = i;
                episode.seasonNumber = 1;
                episode.airDate = std::chrono::system_clock::now() - 
                                std::chrono::hours(24 * i);
                show.episodes.push_back(std::move(episode));
            }
            
            return utils::Result<domain::TvShowInfo>(std::move(show)); });
    }
}
