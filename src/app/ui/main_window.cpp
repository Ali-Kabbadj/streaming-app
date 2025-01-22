#include "main_window.hpp"
#include "utils/logger.hpp"

namespace app::ui
{

    MainWindow::MainWindow()
        : ipcManager_(std::make_unique<ipc::IpcManager>()),
          webview_(nullptr) {}

    MainWindow::~MainWindow() = default;

    bool MainWindow::Initialize(HINSTANCE hInstance, int nCmdShow)
    {
        if (!WindowBase::Initialize(hInstance, nCmdShow))
            return false;

        InitializeWebView();
        SetupIpcHandlers();
        return true;
    }

    void MainWindow::InitializeWebView()
    {
        webview_ = std::make_unique<WebViewHost>(hwnd_);
        webview_->SetMessageCallback([this](const std::wstring &message)
                                     { ipcManager_->HandleWebMessage(message); });

        webview_->Initialize();
        webview_->Navigate(L"http://localhost:3000"); // Replace with your front-end URL
    }

    void MainWindow::SetupIpcHandlers()
    {
        // Add handlers for API requests
        // Example: movies/list
        ipcManager_->RegisterHandler("movies/list", [](const ipc::json &payload, auto respond)
                                     {
                                         respond({{"success", true}, {"movies", {}}}); // Stub response
                                     });
    }

    std::wstring MainWindow::GetWindowTitle() const
    {
        return L"Streaming App";
    }

    std::wstring MainWindow::GetWindowClassName() const
    {
        return L"MainWindowClass";
    }

    LRESULT MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_SIZE:
            if (webview_)
            {
                RECT bounds;
                GetClientRect(hwnd_, &bounds);
                webview_->Resize(bounds);
            }
            break;
        }
        return WindowBase::HandleMessage(msg, wParam, lParam);
    }

    void MainWindow::OnSize(UINT width, UINT height)
    {
        if (webview_)
        {
            RECT bounds = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
            webview_->Resize(bounds);
        }
    }

} // namespace app::ui
