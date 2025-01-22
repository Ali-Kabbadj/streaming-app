#include "main_window.hpp"
#include "utils/logger.hpp"
#include "config/config_manager.hpp"
#include "services/media/media_service.hpp"

namespace app::ui
{

    MainWindow::MainWindow()
        : ipcManager_(std::make_unique<ipc::IpcManager>()),
          webview_(nullptr) {}

    MainWindow::~MainWindow() = default;

    void MainWindow::InitializeWebView()
    {
        webview_ = std::make_unique<WebViewHost>(hwnd_);

        webview_->SetInitCallback([this](bool success)
                                  {
        if (success) {
            // Set up IPC handlers first
            SetupIpcHandlers();
            
            // Connect WebView message handling to IPC manager
            webview_->SetMessageCallback([this](const std::wstring& message) {
                utils::Logger::Info("Forwarding message to IPC manager");
                ipcManager_->HandleWebMessage(message);
            });
            
            // Set up WebView callback in IPC manager
            ipcManager_->SetWebViewCallback([this](const std::wstring& message) {
                utils::Logger::Info("IPC manager sending response");
                webview_->PostWebMessage(message);
            });

            auto &config = config::ConfigManager::Instance();
            config.Set("webview.url", "http://localhost:3000");
            auto url = config.Get<std::string>("webview.url").Value();
            webview_->Navigate(utils::Utf8ToWide(url));
            utils::Logger::Info("WebView initialized successfully.");
        } else {
            utils::Logger::Error("WebView initialization failed.");
        } });

        webview_->Initialize();
    }

    bool MainWindow::Initialize(HINSTANCE hInstance, int nCmdShow)
    {
        if (!WindowBase::Initialize(hInstance, nCmdShow))
            return false;

        services::MediaService::Instance().Initialize();

        if (!services::MediaService::Instance().Initialize())
        {
            utils::Logger::Error("Failed to initialize MediaService");
            return false;
        }

        InitializeWebView();
        return true;
    }

    void MainWindow::SetupIpcHandlers()
    {
        utils::Logger::Info("Setting up IPC handlers...");
        ipcManager_->RegisterHandler("movies", [](const ipc::json &payload, auto respond)
                                     {
        try {
            utils::Logger::Info("Processing 'movies' IPC request.");
            auto result = services::MediaService::Instance().GetPopularMovies().get();
            auto result1 = services::MediaService::Instance().GetPopularMovies(2).get();

            if (result.IsOk() && result1.IsOk())
            {
                ipc::json movieArray = ipc::json::array();
                
                for (const auto &movie : result.Value()) {
                    try {
                        // Create safe getters with default values
                        auto safeTitle = movie.title.empty() ? "Untitled" : movie.title;
                        auto safeOverview = movie.overview.empty() ? "No overview available" : movie.overview;
                        auto safeId = movie.imdbId.empty() ? "unknown" : movie.imdbId;
                        
                        ipc::json movieJson = {
                            {"title", safeTitle},
                            {"overview", safeOverview},
                            {"rating", movie.rating},
                            {"voteCount", movie.voteCount},
                            {"id", safeId}
                        };

                        // Handle optional poster path separately
                        if (movie.posterPath && !movie.posterPath->empty()) {
                            movieJson["poster"] = *movie.posterPath;
                        } else {
                            movieJson["poster"] = nullptr;
                        }

                        movieArray.push_back(movieJson);
                        
                        utils::Logger::Info("Successfully processed movie: " + safeTitle);
                    }
                    catch (const std::exception& movieEx) {
                        utils::Logger::Error("Error processing individual movie: " + std::string(movieEx.what()));
                        // Continue processing other movies even if one fails
                        continue;
                    }
                }
                
                
                ipc::json response = {
                    {"success", true},
                    {"movies", movieArray},
                };
                
                utils::Logger::Info("Sending response with " + 
                    std::to_string(movieArray.size()) + " movies");
                    
                respond(response);
                
                utils::Logger::Info("Response sent successfully");
            }
            else
            {
                utils::Logger::Error(std::string("Error: ") + result.GetError().message);
                respond({
                    {"success", false},
                    {"error", result.GetError().message},
                });
            }
        } catch (const std::exception &ex) {
            utils::Logger::Error(std::string("Exception in 'movies' handler: ") + ex.what());
            respond({
                {"success", false},
                {"error", ex.what()},
            });
        } });
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
