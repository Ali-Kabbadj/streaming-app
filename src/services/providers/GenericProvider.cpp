#include "GenericProvider.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include "utils/logger.hpp"

namespace app::services
{
    GenericProvider::GenericProvider(const ProviderManifest &manifest, const std::string &apiKey)
        : manifest_(manifest), apiKey_(apiKey) {}

    std::string GenericProvider::GetProviderName() const
    {
        return manifest_.name;
    }

    std::string GenericProvider::GetProviderVersion() const
    {
        return manifest_.version;
    }

    ProviderCapabilities GenericProvider::GetCapabilities() const
    {
        return {
            .supportedTypes = manifest_.types,
            .supportedGenres = manifest_.genres,
            .supportedSortOptions = manifest_.sortOptions,
            .supportsSearch = manifest_.capabilities.search,
            .supportsCatalog = manifest_.capabilities.catalog};
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    GenericProvider::SearchMedia(const std::string &query, const MediaFilter &filter, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
        try {
            const std::string url = BuildUrl("search", query, filter, page);
            utils::Logger::Info("Constructed Search URL: " + url);
            const auto response = utils::HttpClient::Get(url);
            const auto json = nlohmann::json::parse(response);

            if (!json.contains("results")) {
                utils::Logger::Error("Invalid API response: 'results' key missing");
                return utils::Result<std::vector<domain::MediaMetadata>>(
                    utils::Result<std::vector<domain::MediaMetadata>>::Error("Invalid API response: 'results' key missing")
                );
            }

            std::vector<domain::MediaMetadata> results;
            for (const auto& item : json["results"]) {
                results.push_back(ParseItem(item));
            }
            return utils::Result<std::vector<domain::MediaMetadata>>(results);
        } catch (const std::exception& e) {
            utils::Logger::Error("SearchMedia failed: " + std::string(e.what()));
            return utils::Result<std::vector<domain::MediaMetadata>>(
                utils::Result<std::vector<domain::MediaMetadata>>::Error(e.what())
            );
        } });
    }

    std::future<utils::Result<std::vector<domain::MediaMetadata>>>
    GenericProvider::GetCatalog(const std::string &catalogType, const MediaFilter &filter, int page)
    {
        return std::async(std::launch::async, [=]()
                          {
        try {
            // Find the catalog configuration
            auto it = manifest_.catalogs.find(catalogType);
            if (it == manifest_.catalogs.end()) {
                utils::Logger::Error("Catalog type not found: " + catalogType);
                return utils::Result<std::vector<domain::MediaMetadata>>(
                    utils::Result<std::vector<domain::MediaMetadata>>::Error("Catalog type not found: " + catalogType)
                );
            }

            // Build the URL for the catalog endpoint
            const std::string url = BuildUrl("catalog", "", filter, page, &it->second);
            utils::Logger::Info("Constructed Catalog URL: " + url);

            // Fetch the API response
            const auto response = utils::HttpClient::Get(url);
            const auto json = nlohmann::json::parse(response);

            // Check if the "results" array exists
            if (!json.contains("results") || !json["results"].is_array()) {
                utils::Logger::Error("Invalid API response: 'results' key missing or not an array");
                return utils::Result<std::vector<domain::MediaMetadata>>(
                    utils::Result<std::vector<domain::MediaMetadata>>::Error("Invalid API response: 'results' key missing or not an array")
                );
            }

            // Parse each item in the "results" array
            std::vector<domain::MediaMetadata> results;
            for (const auto& item : json["results"]) {
                try {
                    results.push_back(ParseItem(item));
                } catch (const std::exception& e) {
                    utils::Logger::Error("Failed to parse item: " + std::string(e.what()));
                    continue; // Skip invalid items
                }
            }

            return utils::Result<std::vector<domain::MediaMetadata>>(results);
        } catch (const std::exception& e) {
            utils::Logger::Error("GetCatalog failed: " + std::string(e.what()));
            return utils::Result<std::vector<domain::MediaMetadata>>(
                utils::Result<std::vector<domain::MediaMetadata>>::Error(e.what())
            );
        } });
    }

    std::future<utils::Result<domain::MediaMetadata>>
    GenericProvider::GetMediaDetails(const domain::MediaId &mediaId)
    {
        return std::async(std::launch::async, [=]()
                          {
        try {
            // Build the URL for fetching media details
            const std::string url = manifest_.endpoint + "/details?id=" + mediaId.id;
            const auto response = utils::HttpClient::Get(url);
            const auto json = nlohmann::json::parse(response);

            // Parse the response and return the media metadata
            domain::MediaMetadata metadata = ParseItem(json);
            return utils::Result<domain::MediaMetadata>(metadata);
        }
        catch (const std::exception& e) {
            // Construct an Error object explicitly
            return utils::Result<domain::MediaMetadata>(
                utils::Result<domain::MediaMetadata>::Error(e.what())
            );
        } });
    }

    std::string GenericProvider::BuildUrl(const std::string &endpointType,
                                          const std::string &query,
                                          const MediaFilter &filter,
                                          int page,
                                          const CatalogConfig *catalogConfig) const
    {
        std::variant<const SearchConfig *, const CatalogConfig *> endpointConfig;

        if (endpointType == "search")
        {
            endpointConfig = &manifest_.search;
        }
        else
        {
            if (!catalogConfig)
            {
                throw std::runtime_error("CatalogConfig must be provided for catalog endpoints");
            }
            endpointConfig = catalogConfig;
        }

        return std::visit([&](auto *config)
                          {
        std::string url = manifest_.endpoint + config->path + "?";
        for (const auto& [key, value] : config->query_params) {
            std::string paramValue = value;
            if (paramValue.find("{query}") != std::string::npos) {
                paramValue.replace(paramValue.find("{query}"), 7, query);
            }
            if (paramValue.find("{page}") != std::string::npos) {
                paramValue.replace(paramValue.find("{page}"), 6, std::to_string(page));
            }
            if (paramValue.find("{sortBy}") != std::string::npos) {
                paramValue.replace(paramValue.find("{sortBy}"), 8, filter.sortBy.value_or("popularity"));
            }
            url += key + "=" + paramValue + "&";
        }
        if (manifest_.auth.has_value()) {
            url += manifest_.auth->key_param + "=" + apiKey_;
        }
        return url; }, endpointConfig);
    }

    domain::MediaMetadata GenericProvider::ParseItem(const nlohmann::json &item) const
    {
        domain::MediaMetadata metadata;
        metadata.id.source = manifest_.id;

        // Parse numeric fields
        metadata.id.id = std::to_string(item["id"].get<int>()); // Convert numeric ID to string
        metadata.rating = item["vote_average"].get<float>();    // Parse vote_average as float
        metadata.voteCount = item["vote_count"].get<int>();     // Parse vote_count as int

        // Parse string fields
        metadata.title = item["title"].get<std::string>();
        metadata.overview = item["overview"].get<std::string>();

        // Handle optional poster path
        if (item.contains("poster_path") && !item["poster_path"].is_null())
        {
            metadata.posterPath = "https://image.tmdb.org/t/p/w500" + item["poster_path"].get<std::string>();
        }

        // Handle optional backdrop path
        if (item.contains("backdrop_path") && !item["backdrop_path"].is_null())
        {
            metadata.backdropPath = "https://image.tmdb.org/t/p/w500" + item["backdrop_path"].get<std::string>();
        }

        // Parse release date
        if (item.contains("release_date") && !item["release_date"].is_null())
        {
            std::string releaseDateStr = item["release_date"].get<std::string>();
            // Convert release date string to time_point (if needed)
            // Example: metadata.releaseDate = ParseDate(releaseDateStr);
        }

        return metadata;
    }
}