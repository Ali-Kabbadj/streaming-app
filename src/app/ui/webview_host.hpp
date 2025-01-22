#include "webview_host.hpp"
#include "utils/logger.hpp"

namespace app::ui
{

    WebViewHost::WebViewHost(HWND parentWindow)
        : parentWindow_(parentWindow) {}

    WebViewHost::~WebViewHost() = default;

    void WebViewHost::Initialize()
    {
        // Initialization code for WebView2
        utils::Logger::Info("WebView initialized.");
    }

    void WebViewHost::Navigate(const std::wstring &url)
    {
        // Navigation logic
        utils::Logger::Info("Navigating to " + std::string(url.begin(), url.end()));
    }

    void WebViewHost::Resize(const RECT &bounds)
    {
        // Handle resizing
    }

    void WebViewHost::SetMessageCallback(std::function<void(const std::wstring &)> callback)
    {
        messageCallback_ = std::move(callback);
    }

} // namespace app::ui
