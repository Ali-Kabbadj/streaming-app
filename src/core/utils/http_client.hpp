#pragma once
#include <string>
#include <curl/curl.h>
#include <stdexcept>

namespace app::utils
{

    class HttpClient
    {
    public:
        static std::string Get(const std::string &url)
        {
            CURL *curl = curl_easy_init();
            std::string response;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK)
            {
                throw std::runtime_error("HTTP request failed");
            }

            return response;
        }

        static std::string EscapeUrl(const std::string &url)
        {
            CURL *curl = curl_easy_init();
            if (!curl)
            {
                throw std::runtime_error("Failed to initialize CURL");
            }

            char *encoded = curl_easy_escape(curl, url.c_str(), url.length());
            if (!encoded)
            {
                curl_easy_cleanup(curl);
                throw std::runtime_error("Failed to encode URL");
            }

            std::string result(encoded);
            curl_free(encoded);
            curl_easy_cleanup(curl);

            return result;
        }

    private:
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
        {
            size_t total = size * nmemb;
            output->append(static_cast<char *>(contents), total);
            return total;
        }
    };
}