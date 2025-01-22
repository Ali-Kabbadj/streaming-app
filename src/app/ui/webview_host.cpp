#include "webview_host.hpp"
#include "utils/logger.hpp"
#include <format>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>
#include <wrl.h>
#include <wil/com.h>
#include "utils/win32_utils.hpp"
#include <spdlog/details/os-inl.h>
#include "utils/logger.hpp"

namespace app::ui
{

    WebViewHost::WebViewHost(HWND parentWindow)
        : parentWindow_(parentWindow), webview_(nullptr), controller_(nullptr) {}

    WebViewHost::~WebViewHost() = default;

    void WebViewHost::Initialize()
    {
        utils::Logger::Info("Initializing WebView2...");

        auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();

        CreateCoreWebView2EnvironmentWithOptions(
            nullptr, nullptr, options.Get(),
            Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                [this](HRESULT result, ICoreWebView2Environment *env) -> HRESULT
                {
                    if (FAILED(result))
                    {
                        utils::Logger::Error("Failed to create WebView2 environment");
                        return result;
                    }

                    env->CreateCoreWebView2Controller(
                        parentWindow_,
                        Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                            [this](HRESULT result, ICoreWebView2Controller *controller) -> HRESULT
                            {
                                if (FAILED(result))
                                {
                                    utils::Logger::Error("Failed to create WebView2 controller");
                                    return result;
                                }

                                controller_ = controller;
                                controller_->get_CoreWebView2(&webview_);

                                SetupWebMessageHandler();

                                RECT bounds;
                                GetClientRect(parentWindow_, &bounds);
                                controller_->put_Bounds(bounds);

                                if (initCallback_)
                                {
                                    initCallback_(true);
                                }

                                return S_OK;
                            })
                            .Get());
                    return S_OK;
                })
                .Get());
    }

    bool WebViewHost::IsValid() const
    {
        return webview_ != nullptr && controller_ != nullptr;
    }

    void WebViewHost::Navigate(const std::wstring &url)
    {
        if (webview_)
        {
            webview_->Navigate(url.c_str());
        }
    }

    void WebViewHost::Resize(const RECT &bounds)
    {
        if (controller_)
        {
            controller_->put_Bounds(bounds);
        }
    }

    // void WebViewHost::PostWebMessage(const std::wstring &message)
    // {
    //     if (webview_)
    //     {
    //         webview_->PostWebMessageAsJson(message.c_str());
    //     }
    // }

    void WebViewHost::PostWebMessage(const std::wstring &message)
    {
        if (!webview_)
        {
            utils::Logger::Error("WebView is null");
            return;
        }

        if (message.empty())
        {
            utils::Logger::Warning("Empty message received");
            return;
        }

        HRESULT hr = webview_->PostWebMessageAsJson(message.c_str());
        if (FAILED(hr))
        {
            utils::Logger::Error("Failed to post web message");
        }
    }

    void WebViewHost::SetupWebMessageHandler()
    {
        if (!webview_)
            return;

        utils::Logger::Info("Setting up WebView message handler...");

        webview_->add_WebMessageReceived(
            Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                [this](ICoreWebView2 *webview, ICoreWebView2WebMessageReceivedEventArgs *args) -> HRESULT
                {
                    LPWSTR message;
                    args->get_WebMessageAsJson(&message);

                    utils::Logger::Info("WebMessage received: " + utils::WideToUtf8(message));

                    if (messageCallback_)
                    {
                        messageCallback_(message);
                    }

                    CoTaskMemFree(message);
                    return S_OK;
                })
                .Get(),
            nullptr);

        utils::Logger::Info("WebView message handler setup complete");
    }

    // webview_host.cpp
    void WebViewHost::NavigateAndSendData(const std::wstring &url, const std::wstring &jsonData)
    {
        std::lock_guard<std::mutex> lock(navigationMutex_);

        // Clean up any existing navigation
        EnsureNavigationCleanup();

        // Create new navigation context
        currentNavigation_ = std::make_unique<NavigationContext>(
            NavigationContext{url, jsonData, NavigationState::Idle, nullptr});

        // Setup navigation completed handler
        auto handler = Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
                           [this](ICoreWebView2 *sender, ICoreWebView2NavigationCompletedEventArgs *args) -> HRESULT
                           {
                               HandleNavigationCompleted(args);
                               return S_OK;
                           })
                           .Get();

        // Start navigation
        currentNavigation_->state = NavigationState::Navigating;

        HRESULT hr = webview_->Navigate(url.c_str());
        if (FAILED(hr))
        {
            // utils::Logger::Error("Navigation failed with HRESULT: {}", hr);
            currentNavigation_->state = NavigationState::Error;
            return;
        }

        // Register completion handler
        webview_->add_NavigationCompleted(handler, nullptr);
    }

    void WebViewHost::HandleNavigationCompleted(ICoreWebView2NavigationCompletedEventArgs *args)
    {
        if (!currentNavigation_)
        {
            return;
        }

        BOOL isSuccess;
        args->get_IsSuccess(&isSuccess);

        if (isSuccess)
        {
            if (!currentNavigation_->data.empty())
            {
                webview_->PostWebMessageAsJson(currentNavigation_->data.c_str());
            }
            currentNavigation_->state = NavigationState::Complete;
        }
        else
        {
            currentNavigation_->state = NavigationState::Error;
            utils::Logger::Error("Navigation failed");
        }

        if (currentNavigation_->callback)
        {
            currentNavigation_->callback(isSuccess);
        }
    }

    void WebViewHost::EnsureNavigationCleanup()
    {
        if (currentNavigation_ &&
            (currentNavigation_->state == NavigationState::Complete ||
             currentNavigation_->state == NavigationState::Error))
        {
            currentNavigation_.reset();
        }
    }

} // namespace app::webview