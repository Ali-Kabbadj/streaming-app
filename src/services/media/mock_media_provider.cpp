#include "media_service.hpp"
#include "core/utils/logger.hpp"
#include <random>
#include <thread>

namespace app::services
{

    namespace
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> ratingDist(6.0, 9.5);
        std::uniform_int_distribution<> voteDist(1000, 50000);
        std::uniform_int_distribution<> runtimeDist(90, 180);

        const std::vector<std::string> MOCK_GENRES = {
            "Action", "Adventure", "Comedy", "Drama", "Sci-Fi", "Thriller"};

        const std::vector<std::string> MOCK_DIRECTORS = {
            "Christopher Nolan", "Steven Spielberg", "Martin Scorsese"};

        const std::vector<std::string> MOCK_ACTORS = {
            "Tom Hanks", "Leonardo DiCaprio", "Meryl Streep", "Brad Pitt"};
    }

    std::future<utils::Result<std::vector<domain::MovieInfo>>>
    MockMediaProvider::GetPopularMovies(int page)
    {
        return std::async(std::launch::async, [page]()
                          {
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate network delay
        
        std::vector<domain::MovieInfo> movies;
        for (int i = 1; i <= 20; ++i) {
            domain::MovieInfo movie;
            movie.title = "Mock Movie " + std::to_string((page - 1) * 20 + i);
            movie.overview = "This is a mock movie description for testing purposes.";
            movie.rating = ratingDist(gen);
            movie.voteCount = voteDist(gen);
            movie.runtime = runtimeDist(gen);
            movie.releaseDate = std::chrono::system_clock::now() - 
                               std::chrono::hours(24 * 365 * (i % 5));
            movie.genres = {MOCK_GENRES[i % MOCK_GENRES.size()]};
            movie.director = MOCK_DIRECTORS[i % MOCK_DIRECTORS.size()];
            movie.cast = {MOCK_ACTORS[i % MOCK_ACTORS.size()]};
            movie.imdbId = "tt" + std::to_string(1000000 + i);
            movies.push_back(std::move(movie));
        }
        
        return utils::Result<std::vector<domain::MovieInfo>>(std::move(movies)); });
    }

    // Implement other mock methods similarly...
    // GetPopularTvShows, GetMovieDetails, GetTvShowDetails

} // namespace app::services