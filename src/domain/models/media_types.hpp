#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <date/date.h>

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
        double rating;
        int voteCount;
        std::optional<std::string> posterPath;
        std::optional<std::string> backdropPath;
        float popularity = 0.0f;
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

    struct MediaDetails : MediaMetadata
    {
        std::vector<RatingSource> ratings;
        std::vector<Review> reviews;
        std::vector<std::string> availableProviders;
        std::vector<EpisodeInfo> episodes; // For TV shows
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
        date::year_month_day date;
        // date::time_of_day time;
    };

} // namespace app::domain