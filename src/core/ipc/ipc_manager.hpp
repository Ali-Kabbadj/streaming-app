#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <nlohmann/json.hpp>
#include <Windows.h>
#include <string>

namespace app::ipc
{

    using json = nlohmann::json;
    using WebMessageCallback = std::function<void(const std::wstring &)>;
    using IpcHandlerCallback = std::function<void(const json &, std::function<void(const json &)>)>;

    struct NavigationRequest
    {
        std::string route;
        json data;
        bool validateRoute() const
        {
            return !route.empty() && route[0] == '/';
        }
    };

    class IpcManager
    {
    public:
        IpcManager();
        ~IpcManager();

        void HandleWebMessage(const std::wstring &message);
        void SetWebViewCallback(WebMessageCallback callback);
        void RegisterHandler(const std::string &type, IpcHandlerCallback handler);
        void RegisterNavigationHandler();
        void ValidateAndProcessNavigation(const NavigationRequest &request);
        void RegisterMediaHandlers();

    private:
        WebMessageCallback webviewCallback_;
        std::unordered_map<std::string, IpcHandlerCallback> handlers_;

        void SendResponse(const std::string &id, const json &response);
    };

} // namespace app::ipc
