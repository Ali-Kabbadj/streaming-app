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
        webview_->SetInitCallback([this](bool success)
                                  {
        if (success) {
            webview_->Navigate(L"http://localhost:3000");
        } });
        webview_->Initialize();
    }

    void MainWindow::SetupIpcHandlers()
    {
        // Add handlers for API requests
        // Example: movies/list
        ipcManager_->RegisterHandler("movies/list", [](const ipc::json &payload, auto respond)
                                     {
                                         respond({{"success", true}, {"movies", {}}}); // Stub response
                                     });
        ipcManager_->RegisterHandler("navigate",
                                     [this](const ipc::json &payload, const std::function<void(const ipc::json &)> &respond)
                                     {
                                         try
                                         {
                                             std::string movieId = payload["id"].get<std::string>();
                                             utils::Logger::Info("Received movieId: " + movieId);

                                             // Here you would typically fetch the movie data based on the ID
                                             // For now, creating sample data
                                             json movieData = {
                                                 {"image", "https://example.com/movie-" + movieId + ".jpg"},
                                                 {"title", "Movie " + movieId},
                                                 {"genre", "Action"},
                                                 {"releaseDate", "2024"},
                                                 {"rating", "8.5"},
                                                 {"description", "Description for movie " + movieId}};

                                             std::string jsonStr = movieData.dump();
                                             std::wstring wideJsonStr = utils::Utf8ToWide(jsonStr);

                                             utils::Logger::Info("UTF-8 JSON: " + jsonStr);
                                             utils::Logger::Info("Wide JSON: " + std::string(wideJsonStr.begin(), wideJsonStr.end()));

                                             webview_->NavigateAndSendData(
                                                 utils::Utf8ToWide("http://localhost:3000/movie-details"),
                                                 wideJsonStr);

                                             respond(json{{"success", true}});
                                         }
                                         catch (const std::exception &e)
                                         {
                                             respond(json{
                                                 {"success", false},
                                                 {"error", e.what()}});
                                         }
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
