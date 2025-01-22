#pragma once
#include "core/utils/result.hpp"
#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include <optional>

namespace app::ipc
{

    using json = nlohmann::json;

    struct IpcMessage
    {
        std::string type;
        std::string id;
        json payload;
    };

    struct IpcResponse
    {
        std::string id;
        json payload;
        std::optional<std::string> error;
    };

    using WebMessageCallback = std::function<void(const std::wstring &)>;
    using IpcHandlerCallback = std::function<void(const IpcMessage &, std::function<void(const IpcResponse &)>)>;

    class IpcManager
    {
    public:
        IpcManager();
        ~IpcManager();

        utils::Result<void> HandleWebMessage(const std::wstring &message);
        void SetWebViewCallback(WebMessageCallback callback);
        void RegisterHandler(const std::string &type, IpcHandlerCallback handler);
        void SendResponse(const IpcResponse &response);

    private:
        WebMessageCallback webviewCallback_;
        std::unordered_map<std::string, IpcHandlerCallback> handlers_;
        utils::Result<IpcMessage> ParseMessage(const std::wstring &message);
    };

} // namespace app::ipc