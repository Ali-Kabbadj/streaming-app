#pragma once
#include <Windows.h>
#include <wrl.h>
#include <WebView2.h>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include <WebView2EnvironmentOptions.h>
using json = nlohmann::json;

namespace app::ui
{
    class WebViewHost
    {
    public:
        explicit WebViewHost(HWND parentWindow);
        ~WebViewHost();

        void Initialize();
        bool IsValid() const;
        void Navigate(const std::wstring &url);
        void Resize(const RECT &bounds);
        void PostWebMessage(const std::wstring &message);
        void InitializeWebView();
        void SetupWebMessageHandler();
        void NavigateAndSendData(const std::wstring &url, const std::wstring &jsonData);

        using InitCallback = std::function<void(bool)>;
        using MessageCallback = std::function<void(const wchar_t *)>;

        void SetInitCallback(InitCallback callback) { initCallback_ = std::move(callback); }
        void SetMessageCallback(MessageCallback callback) { messageCallback_ = std::move(callback); }

        // Add this declaration
        nlohmann::json FetchMovieDetails(int movieId); // Function to fetch movie details

    private:
        HWND parentWindow_;
        Microsoft::WRL::ComPtr<ICoreWebView2> webview_;
        Microsoft::WRL::ComPtr<ICoreWebView2Controller> controller_;

        InitCallback initCallback_;
        MessageCallback messageCallback_;
    };
}
