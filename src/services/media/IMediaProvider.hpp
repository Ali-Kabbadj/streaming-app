// #pragma once
// #include <string>
// #include <vector>
// #include <future>
// #include <optional>
// #include "nlohmann/json.hpp"
// #include <fmt/format.h>

// namespace app::services
// {
//     struct MediaFilter
//     {
//         std::optional<std::string> genre;
//         std::optional<std::string> type; // movie, series, etc
//         std::optional<int> year;
//         std::optional<std::string> sortBy; // popularity, year, etc
//         std::optional<bool> sortDesc;

//         std::string ToString() const
//         {
//             // Helper lambda to handle optional values
//             auto optionalToString = [](const auto &opt, const std::string &defaultValue = "")
//             {
//                 return opt ? fmt::format("{}", *opt) : defaultValue;
//             };

//             auto boolToString = [](std::optional<bool> opt)
//             {
//                 return opt ? (*opt ? "true" : "false") : "";
//             };

//             return fmt::format(
//                 "genre:{}|type:{}|year:{}|sortBy:{}|sortDesc:{}",
//                 optionalToString(genre),
//                 optionalToString(type),
//                 optionalToString(year, "0"),
//                 optionalToString(sortBy),
//                 boolToString(sortDesc));
//         }
//     };

//     struct ProviderCapabilities
//     {
//         std::vector<std::string> supportedTypes;
//         std::vector<std::string> supportedGenres;
//         std::vector<std::string> supportedSortOptions;
//         bool supportsSearch;
//         bool supportsCatalog;
//     };

//     class IMediaProvider
//     {
//     public:
//         virtual ~IMediaProvider() = default;

//         // Core identification and capabilities
//         virtual std::string GetProviderName() const = 0;
//         virtual std::string GetProviderVersion() const = 0;
//         virtual ProviderCapabilities GetCapabilities() const = 0;

//         // Catalog and search functionality
//         virtual std::future<utils::Result<std::vector<domain::MediaMetadata>>>
//         GetCatalog(const MediaFilter &filter, int page) = 0;

//         virtual std::future<utils::Result<std::vector<domain::MediaMetadata>>>
//         SearchMedia(const std::string &query, const MediaFilter &filter, int page) = 0;

//         // Media details and metadata
//         virtual std::future<utils::Result<domain::MediaMetadata>>
//         GetMediaDetails(const domain::MediaId &id) = 0;
//     };

//     // ProviderRegistry.hpp
//     class ProviderRegistry
//     {
//     public:
//         static ProviderRegistry &Instance();

//         void RegisterProvider(const std::string &manifest);
//         void UnregisterProvider(const std::string &providerId);
//         std::vector<IMediaProvider *> GetProvidersWithCapability(
//             const std::string &capability);

//     private:
//         std::unordered_map<std::string, std::unique_ptr<IMediaProvider>> providers_;
//         std::mutex providerMutex_;

//         bool ValidateManifest(const nlohmann::json &manifest);
//         std::unique_ptr<IMediaProvider> CreateProviderFromManifest(
//             const nlohmann::json &manifest);
//     };

// }
#pragma once
#include <string>
#include <vector>
#include <future>
#include <optional>
#include "nlohmann/json.hpp"
#include <fmt/format.h>

namespace app::services
{
    struct MediaFilter
    {
        std::optional<std::string> genre;
        std::optional<std::string> type; // movie, series, etc
        std::optional<int> year;
        std::optional<std::string> sortBy; // popularity, year, etc
        std::optional<bool> sortDesc;

        std::string ToString() const
        {
            // Helper lambda to handle optional values
            auto optionalToString = [](const auto &opt, const std::string &defaultValue = "")
            {
                return opt ? fmt::format("{}", *opt) : defaultValue;
            };

            auto boolToString = [](std::optional<bool> opt)
            {
                return opt ? (*opt ? "true" : "false") : "";
            };

            return fmt::format(
                "genre:{}|type:{}|year:{}|sortBy:{}|sortDesc:{}",
                optionalToString(genre),
                optionalToString(type),
                optionalToString(year, "0"),
                optionalToString(sortBy),
                boolToString(sortDesc));
        }
    };

    struct ProviderCapabilities
    {
        std::vector<std::string> supportedTypes;
        std::vector<std::string> supportedGenres;
        std::vector<std::string> supportedSortOptions;
        bool supportsSearch;
        bool supportsCatalog;
    };

    class IMediaProvider
    {
    public:
        virtual ~IMediaProvider() = default;

        // Core identification and capabilities
        virtual std::string GetProviderName() const = 0;
        virtual std::string GetProviderVersion() const = 0;
        virtual ProviderCapabilities GetCapabilities() const = 0;

        // Catalog and search functionality
        virtual std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        GetCatalog(const std::string &catalogType, const MediaFilter &filter, int page) = 0;

        virtual std::future<utils::Result<std::vector<domain::MediaMetadata>>> SearchMedia(const std::string &query, const MediaFilter &filter, int page) = 0;

        // Media details and metadata
        virtual std::future<utils::Result<domain::MediaMetadata>>
        GetMediaDetails(const domain::MediaId &id) = 0;
    };
} // namespace app::services