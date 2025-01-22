#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace app::ipc
{
    using json = nlohmann::json;

    enum class MessageType
    {
        Request,
        Response,
        Error
    };

    struct Message
    {
        MessageType type;
        std::string id;
        json payload;
    };

    struct Error
    {
        int code;
        std::string message;
    };
} // namespace app::ipc