#include "tmdb_provider.hpp"
#include "core/utils/logger.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace app::services
{

    TmdbProvider::TmdbProvider(const std::string &apiKey)
        : apiKey_(apiKey) {}

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    TmdbProvider::SearchMedia(const std::string &query, const MediaFilter &filter, int page)
    {
        // Suppress unused parameter warnings
        (void)filter;
        (void)page;

        return std::async(std::launch::async, [=]() -> utils::Result<std::vector<domain::MediaMetadata>>
                          {
            try {
                const std::string url = BuildUrl(query, page);
                const auto response = utils::HttpClient::Get(url);
                const auto json = nlohmann::json::parse(response);

                std::vector<domain::MediaMetadata> results;
                for (const auto &item : json["results"])
                {
                    results.push_back(ParseItem(item));
                }

                return utils::Result<std::vector<domain::MediaMetadata>>(results);
            }
            catch(const std::exception& e) {
                return utils::Result<std::vector<domain::MediaMetadata>>::Error(
                    "TMDB search failed: " + std::string(e.what()));
            } });
    }

    domain::MediaMetadata TmdbProvider::ParseItem(const nlohmann::json &item) const
    {
        domain::MediaMetadata metadata;

        metadata.id.source = "tmdb";
        metadata.id.id = std::to_string(item["id"].get<int>());
        metadata.title = item.value("title", item.value("name", ""));
        metadata.overview = item.value("overview", "");
        metadata.popularity = item.value("popularity", 0.0f);

        if (item.contains("poster_path"))
        {
            metadata.posterPath = "https://image.tmdb.org/t/p/w500" +
                                  item["poster_path"].get<std::string>();
        }

        if (item.contains("release_date") && !item["release_date"].is_null())
        {
            try
            {
                const auto date = std::chrono::sys_days{
                    std::chrono::year_month_day{
                        std::chrono::year{std::stoi(item["release_date"].get<std::string>().substr(0, 4))},
                        std::chrono::month{static_cast<unsigned>(std::stoi(item["release_date"].get<std::string>().substr(5, 2)))},
                        std::chrono::day{static_cast<unsigned>(std::stoi(item["release_date"].get<std::string>().substr(8, 2)))}}};
                metadata.releaseDate = date;
            }
            catch (...)
            {
                // Handle invalid date format if needed
            }
        }

        return metadata;
    }

    std::string TmdbProvider::BuildUrl(const std::string &query, int page) const
    {
        // Suppress unused parameter warnings
        (void)query;
        (void)page;

        return fmt::format(
            "https://api.themoviedb.org/3/discover/movie?api_key={}&include_adult=false&include_video=false&language=en-US&page=1&sort_by=popularity.desc",
            apiKey_);
    }

    std::future<utils::Result<domain::MediaMetadata>>
    TmdbProvider::GetMediaDetails(const domain::MediaId &id)
    {
        // Suppress unused parameter warnings
        (void)id;

        return std::async(std::launch::async, [=]() -> utils::Result<domain::MediaMetadata>
                          { return utils::Result<domain::MediaMetadata>::Error("Not implemented"); });
    }

    std::string TmdbProvider::GetProviderVersion() const
    {
        return "3.0.0";
    }

    ProviderCapabilities TmdbProvider::GetCapabilities() const
    {
        return {
            .supportedTypes = {"movie", "tv"},
            .supportedGenres = {"Action", "Comedy", "Drama"},
            .supportedSortOptions = {"popularity", "release_date", "vote_average"},
            .supportsSearch = true,
            .supportsCatalog = true};
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    TmdbProvider::GetCatalog(const MediaFilter &filter, int page)
    {
        return std::async(std::launch::async, [=]() -> utils::Result<std::vector<domain::MediaMetadata>>
                          {
            try {
                const std::string url = BuildCatalogUrl(filter, page);
                const auto response = utils::HttpClient::Get(url);
                const auto json = nlohmann::json::parse(response);

                std::vector<domain::MediaMetadata> results;
                for (const auto &item : json["results"]) {
                    results.push_back(ParseItem(item));
                }

                return utils::Result<std::vector<domain::MediaMetadata>>(results);
            }
            catch (const std::exception &e) {
                return utils::Result<std::vector<domain::MediaMetadata>>::Error(
                    "TMDB catalog failed: " + std::string(e.what())
                );
            } });
    }

    std::string TmdbProvider::BuildCatalogUrl(const MediaFilter &filter, int page) const
    {
        std::vector<std::string> params;

        // Base parameters
        params.push_back(fmt::format("api_key={}", apiKey_));
        params.push_back(fmt::format("page={}", page));

        // Genre filter
        if (filter.genre.has_value())
        {
            params.push_back(fmt::format("with_genres={}", utils::HttpClient::EscapeUrl(*filter.genre)));
        }

        // Media type
        if (filter.type.has_value())
        {
            params.push_back(fmt::format("include_adult={}", (*filter.type == "movie") ? "false" : "false"));
        }

        // Sort parameters
        if (filter.sortBy.has_value())
        {
            std::string sort_dir = (filter.sortDesc.value_or(true)) ? "desc" : "asc";
            params.push_back(fmt::format("sort_by={}.{}", utils::HttpClient::EscapeUrl(*filter.sortBy), sort_dir));
        }

        // Release year
        if (filter.year.has_value())
        {
            params.push_back(fmt::format("primary_release_year={}", *filter.year));
        }

        // Join parameters with '&'
        std::string queryString;
        for (size_t i = 0; i < params.size(); ++i)
        {
            queryString += params[i];
            if (i < params.size() - 1)
            {
                queryString += "&";
            }
        }

        return fmt::format("https://api.themoviedb.org/3/discover/movie?{}", queryString);
    }
}