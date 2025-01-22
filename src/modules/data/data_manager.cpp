#include "data_manager.hpp"
#include <stdexcept>
#include <chrono>
#include <thread>

namespace app::modules::data
{

    // Singleton pattern implementation
    DataManager &DataManager::Instance()
    {
        static DataManager instance;
        return instance;
    }

    DataManager::DataManager() {}

    // Mock movie data
    std::vector<MovieSummary> DataManager::GetMockMovies(int count)
    {
        std::vector<MovieSummary> movies;
        for (int i = 1; i <= count; ++i)
        {
            movies.push_back({std::to_string(i),
                              "Mock Movie " + std::to_string(i),
                              "/path/to/poster_" + std::to_string(i) + ".jpg",
                              2023,
                              7.5 + (i % 3),
                              {"Action", "Adventure"}});
        }
        return movies;
    }

    MovieDetails DataManager::GetMockMovieDetails(const std::string &movieId)
    {
        return {
            movieId,
            "Detailed Movie " + movieId,
            "This is a detailed description for movie " + movieId,
            "/path/to/poster_" + movieId + ".jpg",
            "/path/to/backdrop_" + movieId + ".jpg",
            2023,
            120 + (std::stoi(movieId) % 10),
            8.0 + (std::stoi(movieId) % 2),
            {"Action", "Adventure"},
            {"Actor 1", "Actor 2"},
            "Director Name"};
    }

    // Fetch movies (asynchronous)
    std::future<std::vector<MovieSummary>> DataManager::FetchMovies(const std::string &query, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
                              std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate API delay
                              return GetMockMovies(10);                                    // Return 10 movies per page
                          });
    }

    // Fetch movie details (asynchronous)
    std::future<MovieDetails> DataManager::GetMovieDetails(const std::string &movieId)
    {
        return std::async(std::launch::async, [=]()
                          {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Simulate API delay
        return GetMockMovieDetails(movieId); });
    }

} // namespace app::modules::data
