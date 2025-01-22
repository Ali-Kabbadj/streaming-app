#pragma once
#include "../media/media_service.hpp"
#include "core/utils/http_client.hpp"

namespace app::services
{

    class OmdbMediaProvider : public IMediaProvider
    {
    public:
        explicit OmdbMediaProvider(const std::string &apiKey);

        std::string GetProviderName() const override { return "OMDb"; }
        std::string GetProviderVersion() const override;
        ProviderCapabilities GetCapabilities() const override;
        std::future<utils::Result<std::vector<domain::MediaMetadata>>> GetCatalog(const MediaFilter &filter, int page) override;

        std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        SearchMedia(const std::string &query, const MediaFilter &filter, int page) override;

        std::future<utils::Result<domain::MediaMetadata>>
        GetMediaDetails(const domain::MediaId &id) override;

    private:
        std::string apiKey_;
        domain::MediaMetadata ParseItem(const nlohmann::json &item) const;
        std::string BuildUrl(const std::string &query, int page) const;
    };
}