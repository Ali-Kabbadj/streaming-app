#pragma once
#include "../media/media_service.hpp"
#include "core/utils/http_client.hpp"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace app::services
{

    class TmdbProvider : public IMediaProvider
    {
    public:
        explicit TmdbProvider(const std::string &apiKey);

        std::string GetProviderName() const override { return "TMDB"; }
        std::future<utils::Result<std::vector<domain::MediaMetadata>>> SearchMedia(const std::string &query, int page) override;
        std::future<utils::Result<domain::MediaMetadata>> GetMediaDetails(const domain::MediaId &id) override;

    private:
        std::string apiKey_;

        utils::Result<std::vector<domain::MediaMetadata>> ParseSearchResults(const Json &json) const;
        std::string BuildSearchUrl(const std::string &query, int page) const;
    };

} // namespace app::services