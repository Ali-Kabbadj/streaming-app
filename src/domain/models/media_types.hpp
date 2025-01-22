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
        std::string source; // e.g., "tmdb", "imdb", etc.
    };

    struct MediaMetadata
    {
        std::string title;
        std::optional<std::string> originalTitle;
        std::string overview;
        std::vector<std::string> genres;
        std::chrono::system_clock::time_point releaseDate;
        double rating;
        int voteCount;
        std::optional<std::string> posterPath;
        std::optional<std::string> backdropPath;
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
    };

} // namespace app::domain