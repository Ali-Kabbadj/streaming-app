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
            if (message.empty())
            {
                throw std::runtime_error("Empty message received");
            }

            // Allocate a buffer for the converted UTF-8 string
            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, message.c_str(), static_cast<int>(message.length()),
                                                 nullptr, 0, nullptr, nullptr);

            if (bufferSize <= 0)
            {
                throw std::runtime_error("Failed to calculate buffer size for UTF-8 conversion");
            }

            // Allocate buffer for the converted string (+1 for null terminator)
            std::vector<char> utf8Buffer(bufferSize + 1);

            int result = WideCharToMultiByte(CP_UTF8, 0, message.c_str(), static_cast<int>(message.length()),
                                             utf8Buffer.data(), bufferSize, nullptr, nullptr);

            if (result <= 0)
            {
                throw std::runtime_error("Failed to convert wide string to UTF-8");
            }

            // Ensure null termination
            utf8Buffer[bufferSize] = '\0';

            // Create string from buffer
            std::string utf8Message(utf8Buffer.data(), bufferSize);

            // Parse JSON
            json data = json::parse(utf8Message);

            // Validate required fields
            if (!data.contains("type") || !data.contains("id") || !data.contains("payload"))
            {
                throw std::runtime_error("Missing required fields in message");
            }

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
        std::string utf8Message = message.dump();

        // Convert UTF-8 to wide string properly
        int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Message.c_str(), -1, nullptr, 0);
        if (wideLength == 0)
        {
            utils::Logger::Error("Failed to calculate wide string length");
            return;
        }

        std::wstring wideMessage(wideLength - 1, L'\0'); // -1 to not include null terminator in string
        if (MultiByteToWideChar(CP_UTF8, 0, utf8Message.c_str(), -1, &wideMessage[0], wideLength) == 0)
        {
            utils::Logger::Error("Failed to convert UTF-8 to wide string");
            return;
        }

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
