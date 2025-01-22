#include "provider_repository.hpp"
#include "GenericProvider.hpp" // Add this line
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include "utils/logger.hpp"

namespace app::services
{
    ProviderRepository &ProviderRepository::Instance()
    {
        static ProviderRepository instance;
        return instance;
    }

    void ProviderRepository::LoadProvidersFromManifests(const std::string &providersDir)
    {
        for (const auto &entry : std::filesystem::directory_iterator(providersDir))
        {
            if (entry.path().extension() == ".json" && entry.path().filename() != "providers.json")
            {
                std::ifstream file(entry.path());
                nlohmann::json manifestJson;
                file >> manifestJson;
                ProviderManifest manifest = manifestJson.get<ProviderManifest>();
                if (ValidateManifest(manifest))
                {
                    auto provider = std::make_unique<GenericProvider>(
                        manifest,
                        GetApiKeyForProvider(providersDir, manifest.id));
                    MediaService::Instance().RegisterProvider(manifest.id, std::move(provider));
                }
            }
        }
    }

    bool ProviderRepository::ValidateManifest(const ProviderManifest &manifest)
    {
        return !manifest.id.empty() && !manifest.endpoint.empty();
    }

    std::string ProviderRepository::GetApiKeyForProvider(const std::string &providersDir, const std::string &providerId)
    {
        std::ifstream file(providersDir + "providers.json");
        nlohmann::json providersJson;
        file >> providersJson;

        for (const auto &provider : providersJson)
        {
            if (provider["id"] == providerId)
            {
                std::string apiKey = provider["api_key"];
                utils::Logger::Info(fmt::format("API key for provider {}: {}", providerId, apiKey)); // Add this line
                return apiKey;
            }
        }
        throw std::runtime_error("API key not found for provider: " + providerId);
    }

    // Define the functions in the source file
    void from_json(const nlohmann::json &j, AuthConfig &auth)
    {
        j.at("type").get_to(auth.type);
        j.at("key_param").get_to(auth.key_param);
    }

    void from_json(const nlohmann::json &j, SearchConfig &search)
    {
        j.at("path").get_to(search.path);
        j.at("query_params").get_to(search.query_params);
        j.at("response_mapping").get_to(search.response_mapping);
    }

    void from_json(const nlohmann::json &j, CatalogConfig &catalog)
    {
        j.at("path").get_to(catalog.path);
        j.at("query_params").get_to(catalog.query_params);

        // Only parse response_mapping if it exists in the JSON
        if (j.contains("response_mapping"))
        {
            catalog.response_mapping = j.at("response_mapping").get<std::unordered_map<std::string, std::string>>();
        }
    }

    void from_json(const nlohmann::json &j, ProviderManifest &manifest)
    {
        j.at("id").get_to(manifest.id);
        j.at("version").get_to(manifest.version);
        j.at("name").get_to(manifest.name);
        j.at("endpoint").get_to(manifest.endpoint);
        if (j.contains("auth"))
        {
            manifest.auth = j.at("auth").get<AuthConfig>();
        }
        j.at("capabilities").at("search").get_to(manifest.capabilities.search);
        j.at("capabilities").at("catalog").get_to(manifest.capabilities.catalog);
        j.at("search").get_to(manifest.search);

        // Parse catalogs if they exist
        if (manifest.capabilities.catalog && j.contains("catalogs"))
        {
            for (const auto &[key, value] : j.at("catalogs").items())
            {
                manifest.catalogs[key] = value.get<CatalogConfig>();
            }
        }
    }
}