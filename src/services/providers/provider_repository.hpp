#pragma once
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "../media/media_service.hpp" // Add this line
// Remove #include "GenericProvider.hpp"

namespace app::services
{

    struct AuthConfig
    {
        std::string type;      // "apikey", "oauth2"
        std::string key_param; // e.g., "api_key"
    };

    struct SearchConfig
    {
        std::string path;
        std::unordered_map<std::string, std::string> query_params;
        std::unordered_map<std::string, std::string> response_mapping;
    };
    struct CatalogConfig
    {
        std::string path;
        std::unordered_map<std::string, std::string> query_params;
        std::optional<std::unordered_map<std::string, std::string>> response_mapping; // Optional
    };

    struct ProviderManifest
    {
        std::string id;
        std::string version;
        std::string name;
        std::string endpoint;
        std::optional<AuthConfig> auth;
        struct
        {
            bool search;
            bool catalog;
        } capabilities;
        SearchConfig search;
        std::unordered_map<std::string, CatalogConfig> catalogs; // Map of catalogs
        std::vector<std::string> types;
        std::vector<std::string> genres;
        std::vector<std::string> sortOptions;
    };

    // Declare the functions in the header file
    void from_json(const nlohmann::json &j, AuthConfig &auth);
    void from_json(const nlohmann::json &j, SearchConfig &search);
    void from_json(const nlohmann::json &j, CatalogConfig &catalog);
    void from_json(const nlohmann::json &j, ProviderManifest &manifest);

    class ProviderRepository
    {
    public:
        static ProviderRepository &Instance();

        void LoadProvidersFromManifests(const std::string &providersDir);
        bool ValidateManifest(const ProviderManifest &manifest);
        std::string GetApiKeyForProvider(const std::string &providersDir, const std::string &providerId);

    private:
        std::unordered_map<std::string, std::unique_ptr<IMediaProvider>> providers_;
        std::mutex providerMutex_;
    };

} // namespace app::services