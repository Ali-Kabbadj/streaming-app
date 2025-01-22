#pragma once
#include "../media/media_service.hpp"
#include "core/utils/http_client.hpp"
#include "provider_repository.hpp"

namespace app::services
{
    class GenericProvider : public IMediaProvider
    {
    public:
        explicit GenericProvider(const ProviderManifest &manifest, const std::string &apiKey);

        std::string GetProviderName() const override;
        std::string GetProviderVersion() const override;
        ProviderCapabilities GetCapabilities() const override;

        std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        SearchMedia(const std::string &query, const MediaFilter &filter, int page) override;

        std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        GetCatalog(const std::string &catalogType, const MediaFilter &filter, int page) override;

        std::future<utils::Result<domain::MediaMetadata>>
        GetMediaDetails(const domain::MediaId &mediaId) override;

    private:
        ProviderManifest manifest_;
        std::string apiKey_;

        std::string BuildUrl(const std::string &endpointType,
                             const std::string &query,
                             const MediaFilter &filter,
                             int page,
                             const CatalogConfig *catalogConfig = nullptr) const;

        domain::MediaMetadata ParseItem(const nlohmann::json &item) const;
    };
}