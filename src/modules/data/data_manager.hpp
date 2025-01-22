#pragma once
#include <string>
#include <vector>
#include <future>
#include "models/movie.hpp"

namespace app::modules::data
{

    class DataManager
    {
    public:
        // Singleton access
        static DataManager &Instance();

        // Asynchronous data fetching methods
        std::future<std::vector<MovieSummary>> FetchMovies(const std::string &query, int page = 1);
        std::future<MovieDetails> GetMovieDetails(const std::string &movieId);

    private:
        DataManager(); // Private constructor for singleton
        // Mock data provider (replace with real API calls later)
        std::vector<MovieSummary> GetMockMovies(int count);
        MovieDetails GetMockMovieDetails(const std::string &movieId);
    };

} // namespace app::modules::data
