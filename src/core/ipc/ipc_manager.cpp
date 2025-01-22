#include "ipc_manager.hpp"
#include "utils/logger.hpp"

namespace app::ipc
{

    IpcManager::IpcManager() = default;
    IpcManager::~IpcManager() = default;

    void IpcManager::HandleWebMessage(const std::wstring &message)
    {
        try
        {
            std::string utf8Message(message.begin(), message.end());
            json data = json::parse(utf8Message);

            std::string type = data["type"];
            std::string id = data["id"];
            json payload = data["payload"];

            if (handlers_.find(type) != handlers_.end())
            {
                handlers_[type](payload, [this, id](const json &response)
                                { SendResponse(id, response); });
            }
            else
            {
                utils::Logger::Warning("No handler registered for type: " + type);
            }
        }
        catch (const std::exception &e)
        {
            utils::Logger::Error("Failed to handle web message: " + std::string(e.what()));
        }
    }

    void IpcManager::SetWebViewCallback(WebMessageCallback callback)
    {
        webviewCallback_ = std::move(callback);
    }

    void IpcManager::RegisterHandler(const std::string &type, IpcHandlerCallback handler)
    {
        handlers_[type] = std::move(handler);
    }

    void IpcManager::SendResponse(const std::string &id, const json &response)
    {
        if (!webviewCallback_)
            return;

        json message = {{"id", id}, {"payload", response}};
        std::wstring wideMessage(message.dump().begin(), message.dump().end());
        webviewCallback_(wideMessage);
    }

} // namespace app::ipc
