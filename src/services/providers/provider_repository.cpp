#include "provider_repository.hpp"
#include <regex>
#include <iostream>
#include <string>
#include <string_view>

namespace app::services
{

    bool ProviderRepository::ValidateManifest(const ProviderManifest &manifest)
    {
        if (manifest.id.empty() || manifest.version.empty() ||
            manifest.name.empty() || manifest.endpoint.empty())
        {
            return false;
        }

        // Version format check (semver)
        std::regex semver(R"(^\d+\.\d+\.\d+$)");
        if (!std::regex_match(manifest.version, semver))
        {
            return false;
        }

        // URL validation
        try
        {
            std::string url = manifest.endpoint;
            if (!url.starts_with("http://") && !url.starts_with("https://"))
            {
                return false;
            }
        }
        catch (...)
        {
            return false;
        }

        return true;
    }
}