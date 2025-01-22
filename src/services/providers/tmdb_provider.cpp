#include "tmdb_provider.hpp"
#include "core/utils/logger.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace app::services
{

    TmdbProvider::TmdbProvider(const std::string &apiKey)
        : apiKey_(apiKey) {}

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    TmdbProvider::SearchMedia(const std::string &query, int page)
    {
        return std::async(std::launch::async, [=]() -> utils::Result<std::vector<domain::MediaMetadata>>
                          {
            try {
                const std::string url = BuildUrl(query, page);
                const auto response = utils::HttpClient::Get(url);
                const auto json = nlohmann::json::parse(response);

                std::vector<domain::MediaMetadata> results;
                for (const auto &item : json["results"]) // Changed from "Search" to "results"
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
        return fmt::format(
            "https://api.themoviedb.org/3/search/multi?api_key={}&query={}&page={}",
            apiKey_,
            utils::HttpClient::EscapeUrl(query),
            page);
    }

    std::future<utils::Result<domain::MediaMetadata>>
    TmdbProvider::GetMediaDetails(const domain::MediaId &id)
    {
        return std::async(std::launch::async, [=]() -> utils::Result<domain::MediaMetadata>
                          { return utils::Result<domain::MediaMetadata>::Error("Not implemented"); });
    }
} // namespace app::services