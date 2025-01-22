#pragma once
#include "domain/models/media_types.hpp"
#include "core/utils/result.hpp"
#include <future>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace app::services
{

    class IMediaProvider
    {
    public:
        virtual ~IMediaProvider() = default;

        // Provider identification
        virtual std::string GetProviderName() const = 0;

        // Core search functionality
        virtual std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        SearchMedia(const std::string &query, int page) = 0;

        // Media details
        virtual std::future<utils::Result<domain::MediaMetadata>>
        GetMediaDetails(const domain::MediaId &id) = 0;

        // // Optional: Get available genres
        // virtual std::future<utils::Result<std::vector<std::string>>>
        // GetAvailableGenres()
        // {
        //     return std::async(std::launch::async, []()
        //                       { return utils::Result<std::vector<std::string>>::Error("Not implemented"); });
        // }

        // // Optional: Get provider configuration
        // virtual std::future<utils::Result<nlohmann::json>>
        // GetConfiguration()
        // {
        //     return std::async(std::launch::async, []()
        //                       { return utils::Result<nlohmann::json>::Error("Not implemented"); });
        // }
    };

} // namespace app::services