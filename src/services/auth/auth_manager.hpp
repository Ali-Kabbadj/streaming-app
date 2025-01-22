#include <string>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace app::auth
{
    class AuthManager
    {
    public:
        static AuthManager &Instance();

        struct AuthToken
        {
            std::string accessToken;
            std::string refreshToken;
            std::chrono::system_clock::time_point expiresAt;
        };

        AuthToken GetToken(const std::string &providerId);
        bool RefreshToken(const std::string &providerId);
        bool StartOAuthFlow(const std::string &providerId);

    private:
        std::unordered_map<std::string, AuthToken> tokens_;
        std::mutex tokenMutex_;
    };
}