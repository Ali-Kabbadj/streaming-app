#pragma once
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>
#include "core/utils/lru_cache.hpp"

namespace app::services
{

    struct AuthConfig
    {
        std::string type; // "oauth2", "apikey"
        std::string authEndpoint;
        std::vector<std::string> scopes;
    };

    struct ProviderManifest
    {
        std::string id;
        std::string version;
        std::string name;
        std::string description;
        std::string endpoint;
        std::vector<std::string> types;
        std::vector<std::string> genres;
        std::vector<std::string> sortOptions;
        struct
        {
            bool search;
            bool catalog;
        } capabilities;
        std::optional<AuthConfig> auth;
    };

    class ProviderRepository
    {
    public:
        static ProviderRepository &Instance();

        std::vector<ProviderManifest> DiscoverProviders(const std::string &repositoryUrl);
        bool InstallProvider(const ProviderManifest &manifest);
        bool UninstallProvider(const std::string &providerId);
        std::vector<ProviderManifest> GetInstalledProviders() const;

    private:
        std::unordered_map<std::string, ProviderManifest> installedProviders_;
        std::string localStoragePath_;
        bool ValidateManifest(const ProviderManifest &manifest);
    };
}