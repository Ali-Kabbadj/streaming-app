#include "OmdbMediaProvider.hpp"
#include "core/utils/logger.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include "utils/result.hpp"

namespace app::services
{

    OmdbMediaProvider::OmdbMediaProvider(const std::string &apiKey)
        : apiKey_(apiKey) {}

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    OmdbMediaProvider::SearchMedia(const std::string &query, int page)
    {
        return std::async(std::launch::async, [=]() -> utils::Result<std::vector<domain::MediaMetadata>>
                          {
        try {
            const std::string url = BuildUrl(query, page);
            const auto response = utils::HttpClient::Get(url);
            const auto json = nlohmann::json::parse(response);

            if (json["Response"] == "False") {
                return utils::Result<std::vector<domain::MediaMetadata>>::Error(
                    json.value("Error", "Unknown OMDb error"));
            }

            std::vector<domain::MediaMetadata> results;
            for (const auto& item : json["Search"]) {
                results.push_back(ParseItem(item));
            }

            return utils::Result<std::vector<domain::MediaMetadata>>(results);
        }
        catch (const std::exception& e) {
            return utils::Result<std::vector<domain::MediaMetadata>>::Error(
                "OMDb search failed: " + std::string(e.what()));
        } });
    }

    domain::MediaMetadata OmdbMediaProvider::ParseItem(const nlohmann::json &item) const
    {
        domain::MediaMetadata metadata;

        metadata.id.source = "omdb";
        metadata.id.id = item["imdbID"].get<std::string>();
        metadata.title = item["Title"];
        metadata.posterPath = item.value("Poster", "");

        if (item.contains("Year"))
        {
            try
            {
                const int year = std::stoi(item["Year"].get<std::string>());
                metadata.releaseDate = std::chrono::system_clock::now() -
                                       std::chrono::hours(24 * 365 * (2024 - year));
            }
            catch (...)
            {
                // Handle invalid year format if needed
            }
        }

        if (item.contains("Genre"))
        {
            std::string genres = item["Genre"];
            size_t pos = 0;
            while ((pos = genres.find(", ")) != std::string::npos)
            {
                metadata.genres.push_back(genres.substr(0, pos));
                genres.erase(0, pos + 2);
            }
            metadata.genres.push_back(genres);
        }

        return metadata;
    }

    std::string OmdbMediaProvider::BuildUrl(const std::string &query, int page) const
    {
        return fmt::format(
            "http://www.omdbapi.com/?apikey={}&s={}&page={}&type=movie",
            apiKey_,
            utils::HttpClient::EscapeUrl(query),
            page);
    }

    std::future<utils::Result<domain::MediaMetadata>>
    OmdbMediaProvider::GetMediaDetails(const domain::MediaId &id)
    {
        return std::async(std::launch::async, [=]() -> utils::Result<domain::MediaMetadata>
                          { return utils::Result<domain::MediaMetadata>::Error("Not implemented"); });
    }

} // namespace app::services