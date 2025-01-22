#pragma once
#include "domain/models/media_types.hpp"
#include "core/utils/result.hpp"
#include <future>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "services/media/IMediaProvider.hpp"

namespace app::services
{

    class IMediaProvider;

    class MediaService
    {
    public:
        static MediaService &Instance();

        // Lifecycle management
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return initialized_; }

        // Provider management
        void RegisterProvider(const std::string &providerId, std::unique_ptr<IMediaProvider> provider);
        bool SetActiveProvider(const std::string &providerId);
        std::vector<std::string> GetAvailableProviders() const;

        // Core functionality
        std::future<utils::Result<std::vector<domain::MediaMetadata>>>
        UnifiedSearch(const std::string &query, int page = 1);

    private:
        MediaService() = default;

        bool initialized_ = false;
        std::mutex providerMutex_;
        std::unordered_map<std::string, std::unique_ptr<IMediaProvider>> providers_;
        std::string activeProviderId_;

        void LoadDefaultProviders();
    };

} // namespace app::services