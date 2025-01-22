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
            // Allocate a buffer for the converted UTF-8 string
            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, message.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize == 0)
            {
                throw std::runtime_error("Failed to calculate the buffer size for UTF-8 conversion.");
            }

            // Convert the wide string to a UTF-8 string
            std::string utf8Message(bufferSize - 1, '\0'); // Subtract 1 to ignore the null terminator
            WideCharToMultiByte(CP_UTF8, 0, message.c_str(), -1, &utf8Message[0], bufferSize, nullptr, nullptr);

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

    void IpcManager::RegisterNavigationHandler()
    {
        RegisterHandler("navigate", [this](const json &payload, auto respond)
                        {
            try {
                NavigationRequest request{
                    payload["route"].get<std::string>(),
                    payload["data"]
                };

                ValidateAndProcessNavigation(request);
                respond(json{{"success", true}});
            } catch (const std::exception& e) {
                respond(json{
                    {"success", false},
                    {"error", e.what()}
                });
            } });
    }

    void IpcManager::ValidateAndProcessNavigation(const NavigationRequest &request)
    {
        if (!request.validateRoute())
        {
            throw std::invalid_argument("Invalid route format");
        }
        // Process navigation...
    }

} // namespace app::ipc
