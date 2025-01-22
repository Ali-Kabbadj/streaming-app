#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <optional>

namespace app::domain
{

    enum class MediaType
    {
        Movie,
        TvShow,
        Episode
    };

     struct MediaId
    {
        std::string id;
        MediaType type;
        std::string source;      // "tmdb", "imdb", etc.
        std::string original_id; // Original ID from source
    };
    struct MediaMetadata
    {
        MediaId id;
        std::string title;
        std::optional<std::string> originalTitle;
        std::string overview;
        std::vector<std::string> genres;
        std::chrono::system_clock::time_point releaseDate;
        float rating;
        int voteCount;
        std::optional<std::string> posterPath;
        std::optional<std::string> backdropPath;
        float popularity = 0.0f;
        float normalizedRating;
    };

    struct MovieInfo : MediaMetadata
    {
        int runtime; // in minutes
        std::string director;
        std::vector<std::string> cast;
        std::string imdbId;
    };

    struct EpisodeInfo
    {
        std::string title;
        std::string overview;
        int episodeNumber;
        int seasonNumber;
        std::chrono::system_clock::time_point airDate;
        std::optional<std::string> stillPath;
    };

    struct TvShowInfo : MediaMetadata
    {
        int numberOfSeasons;
        int numberOfEpisodes;
        std::string status; // "Continuing", "Ended", etc.
        std::vector<EpisodeInfo> episodes;
        std::string imdbId;
    };

    struct MediaFilter
    {
        MediaType type;
        std::vector<std::string> genres;
        int minYear;
        int maxYear;
        float minRating;
    };

    struct RatingSource
    {
        std::string source; // "IMDb", "RottenTomatoes"
        float value;
        std::string url;
    };

    struct Review
    {
        std::string author;
        std::string content;
        std::string source;
        // date::year_month_day date;
        // date::time_of_day time;
    };

    struct MediaDetails : MediaMetadata
    {
        std::vector<RatingSource> ratings;
        std::vector<Review> reviews;
        std::vector<std::string> availableProviders;
        std::vector<EpisodeInfo> episodes; // For TV shows
    };

} // namespace app::domain