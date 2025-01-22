#pragma once
#include <Windows.h>
#include <string>
#include <functional>

namespace app::ui
{

    class WebViewHost
    {
    public:
        explicit WebViewHost(HWND parentWindow);
        ~WebViewHost();

        void Initialize();
        void Navigate(const std::wstring &url);
        void Resize(const RECT &bounds);
        void SetMessageCallback(std::function<void(const std::wstring &)> callback);

    private:
        HWND parentWindow_;
        std::function<void(const std::wstring &)> messageCallback_;
    };

} // namespace app::ui
