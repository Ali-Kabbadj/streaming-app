#include "OmdbMediaProvider.hpp"
#include "core/utils/logger.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <chrono>

namespace app::services
{
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
    }

    std::string OmdbMediaProvider::FetchDataFromApi(const std::string &url)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            app::utils::Logger::Error("Failed to initialize CURL.");
            return "";
        }

        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            app::utils::Logger::Error("cURL request failed: " + std::string(curl_easy_strerror(res)));
            return "";
        }
        curl_easy_cleanup(curl);
        return response_string;
    }

    std::future<utils::Result<std::vector<domain::MovieInfo>>> OmdbMediaProvider::GetPopularMovies(int page)
    {
        return std::async(std::launch::async, [this, page]
                          {
            const std::string apiKey = "b4e6b940";  // Replace with your OMDb API Key
            const std::string url = "http://www.omdbapi.com/?s=movie&apikey=" + apiKey + "&page=" + std::to_string(page);

            std::string response = FetchDataFromApi(url);
            if (response.empty())
            {
                return utils::Result<std::vector<domain::MovieInfo>>(
                    utils::Result<std::vector<domain::MovieInfo>>::Error("Failed to fetch data from OMDb API."));
            }

            try
            {
                auto jsonResponse = nlohmann::json::parse(response);
                if (jsonResponse["Response"] == "False")
                {
                    return utils::Result<std::vector<domain::MovieInfo>>(
                        utils::Result<std::vector<domain::MovieInfo>>::Error(jsonResponse["Error"].get<std::string>()));
                }

                std::vector<domain::MovieInfo> movies;
                for (const auto &movieData : jsonResponse["Search"])
                {
                    utils::Logger::Info("movie data " + movieData.dump());
                    domain::MovieInfo movie;
                    movie.title = movieData["Title"];
                    movie.posterPath = movieData["Poster"];
                    // movie.releaseDate = movieData["Year"];
                    movie.imdbId = movieData["imdbID"];
                    // movie.originalTitle = movieData.contains("OriginalTitle") ? std::optional<std::string>(movieData["OriginalTitle"]) : std::nullopt;
                    // // movie.overview = movieData["Plot"];
                    // // movie.genres.push_back(movieData["Genre"]);
                    // movie.releaseDate = std::chrono::system_clock::now(); // Placeholder for actual release date parsing
                    // movie.rating = std::stod(movieData["imdbRating"].get<std::string>());
                    // movie.voteCount = std::stoi(movieData["imdbVotes"].get<std::string>());
                    // movie.posterPath = movieData.contains("Poster") ? std::optional<std::string>(movieData["Poster"]) : std::nullopt;
                    // movie.imdbId = movieData["imdbID"];
                    // movie.runtime = std::stoi(movieData["Runtime"].get<std::string>());
                    // movie.director = movieData["Director"];
                    // movie.cast = {movieData["Cast"].get<std::string>()}; // Assuming the cast data is in string, adjust as necessary
                    movies.push_back(movie);
                }

                return utils::Result<std::vector<domain::MovieInfo>>(std::move(movies));
            }
            catch (const std::exception &e)
            {
                return utils::Result<std::vector<domain::MovieInfo>>(
                    utils::Result<std::vector<domain::MovieInfo>>::Error("Failed to parse OMDb response: " + std::string(e.what())));
            } });
    }

    std::future<utils::Result<domain::MovieInfo>> OmdbMediaProvider::GetMovieDetails(const std::string &id)
    {
        return std::async(std::launch::async, [this, id]
                          {
            const std::string apiKey = "YOUR_OMDB_API_KEY";  // Replace with your OMDb API Key
            const std::string url = "http://www.omdbapi.com/?i=" + id + "&apikey=" + apiKey;

            std::string response = FetchDataFromApi(url);
            if (response.empty())
            {
                return utils::Result<domain::MovieInfo>(
                    utils::Result<domain::MovieInfo>::Error("Failed to fetch data from OMDb API."));
            }

            try
            {
                auto jsonResponse = nlohmann::json::parse(response);
                if (jsonResponse["Response"] == "False")
                {
                    return utils::Result<domain::MovieInfo>(
                        utils::Result<domain::MovieInfo>::Error(jsonResponse["Error"].get<std::string>()));
                }

                domain::MovieInfo movie;
                movie.title = jsonResponse["Title"];
                movie.originalTitle = jsonResponse.contains("OriginalTitle") ? std::optional<std::string>(jsonResponse["OriginalTitle"]) : std::nullopt;
                movie.overview = jsonResponse["Plot"];
                movie.genres.push_back(jsonResponse["Genre"]);
                movie.releaseDate = std::chrono::system_clock::now(); // Placeholder for actual release date parsing
                movie.rating = std::stod(jsonResponse["imdbRating"].get<std::string>());
                movie.voteCount = std::stoi(jsonResponse["imdbVotes"].get<std::string>());
                movie.posterPath = jsonResponse.contains("Poster") ? std::optional<std::string>(jsonResponse["Poster"]) : std::nullopt;
                movie.imdbId = jsonResponse["imdbID"];
                movie.runtime = std::stoi(jsonResponse["Runtime"].get<std::string>());
                movie.director = jsonResponse["Director"];
                movie.cast = {jsonResponse["Cast"].get<std::string>()}; // Assuming the cast data is in string, adjust as necessary

                return utils::Result<domain::MovieInfo>(std::move(movie));
            }
            catch (const std::exception &e)
            {
                return utils::Result<domain::MovieInfo>(
                    utils::Result<domain::MovieInfo>::Error("Failed to parse OMDb response: " + std::string(e.what())));
            } });
    }

    std::future<utils::Result<std::vector<domain::TvShowInfo>>> OmdbMediaProvider::GetPopularTvShows(int page)
    {
        return std::async(std::launch::async, [this, page]
                          {
            const std::string apiKey = "YOUR_OMDB_API_KEY";  // Replace with your OMDb API Key
            const std::string url = "http://www.omdbapi.com/?s=tv&apikey=" + apiKey + "&page=" + std::to_string(page);

            std::string response = FetchDataFromApi(url);
            if (response.empty())
            {
                return utils::Result<std::vector<domain::TvShowInfo>>(
                    utils::Result<std::vector<domain::TvShowInfo>>::Error("Failed to fetch data from OMDb API."));
            }

            try
            {
                auto jsonResponse = nlohmann::json::parse(response);
                if (jsonResponse["Response"] == "False")
                {
                    return utils::Result<std::vector<domain::TvShowInfo>>(
                        utils::Result<std::vector<domain::TvShowInfo>>::Error(jsonResponse["Error"].get<std::string>()));
                }

                std::vector<domain::TvShowInfo> tvShows;
                for (const auto &tvShowData : jsonResponse["Search"])
                {
                    domain::TvShowInfo tvShow;
                    tvShow.title = tvShowData["Title"];
                    tvShow.originalTitle = tvShowData.contains("OriginalTitle") ? std::optional<std::string>(tvShowData["OriginalTitle"]) : std::nullopt;
                    tvShow.overview = tvShowData["Plot"];
                    tvShow.genres.push_back(tvShowData["Genre"]);
                    tvShow.releaseDate = std::chrono::system_clock::now(); // Placeholder for actual release date parsing
                    tvShow.rating = std::stod(tvShowData["imdbRating"].get<std::string>());
                    tvShow.voteCount = std::stoi(tvShowData["imdbVotes"].get<std::string>());
                    tvShow.posterPath = tvShowData.contains("Poster") ? std::optional<std::string>(tvShowData["Poster"]) : std::nullopt;
                    tvShow.imdbId = tvShowData["imdbID"];
                    tvShow.numberOfSeasons = std::stoi(tvShowData["TotalSeasons"].get<std::string>());
                    tvShows.push_back(tvShow);
                }

                return utils::Result<std::vector<domain::TvShowInfo>>(std::move(tvShows));
            }
            catch (const std::exception &e)
            {
                return utils::Result<std::vector<domain::TvShowInfo>>(
                    utils::Result<std::vector<domain::TvShowInfo>>::Error("Failed to parse OMDb response: " + std::string(e.what())));
            } });
    }

    std::future<utils::Result<domain::TvShowInfo>> OmdbMediaProvider::GetTvShowDetails(const std::string &id)
    {
        return std::async(std::launch::async, [this, id]
                          {
            const std::string apiKey = "YOUR_OMDB_API_KEY";  // Replace with your OMDb API Key
            const std::string url = "http://www.omdbapi.com/?i=" + id + "&apikey=" + apiKey;

            std::string response = FetchDataFromApi(url);
            if (response.empty())
            {
                return utils::Result<domain::TvShowInfo>(
                    utils::Result<domain::TvShowInfo>::Error("Failed to fetch data from OMDb API."));
            }

            try
            {
                auto jsonResponse = nlohmann::json::parse(response);
                if (jsonResponse["Response"] == "False")
                {
                    return utils::Result<domain::TvShowInfo>(
                        utils::Result<domain::TvShowInfo>::Error(jsonResponse["Error"].get<std::string>()));
                }

                domain::TvShowInfo tvShow;
                tvShow.title = jsonResponse["Title"];
                tvShow.originalTitle = jsonResponse.contains("OriginalTitle") ? std::optional<std::string>(jsonResponse["OriginalTitle"]) : std::nullopt;
                tvShow.overview = jsonResponse["Plot"];
                tvShow.genres.push_back(jsonResponse["Genre"]);
                tvShow.releaseDate = std::chrono::system_clock::now(); // Placeholder for actual release date parsing
                tvShow.rating = std::stod(jsonResponse["imdbRating"].get<std::string>());
                tvShow.voteCount = std::stoi(jsonResponse["imdbVotes"].get<std::string>());
                tvShow.posterPath = jsonResponse.contains("Poster") ? std::optional<std::string>(jsonResponse["Poster"]) : std::nullopt;
                tvShow.imdbId = jsonResponse["imdbID"];
                tvShow.numberOfSeasons = std::stoi(jsonResponse["TotalSeasons"].get<std::string>());
                
                return utils::Result<domain::TvShowInfo>(std::move(tvShow));
            }
            catch (const std::exception &e)
            {
                return utils::Result<domain::TvShowInfo>(
                    utils::Result<domain::TvShowInfo>::Error("Failed to parse OMDb response: " + std::string(e.what())));
            } });
    }
}
