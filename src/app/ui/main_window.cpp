#include "main_window.hpp"
#include "utils/logger.hpp"
#include "config/config_manager.hpp"
#include "services/media/media_service.hpp"
#include <fmt/format.h>

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

        // Initialize MediaService once
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
            
            // Create a MediaFilter object
            services::MediaFilter filter;
            filter.sortBy = "popularity"; // Default sort by popularity
            filter.sortDesc = true;       // Sort in descending order

            // Fetch movies from MediaService (page 1)
            auto& mediaService = services::MediaService::Instance();
            auto resultFuture = mediaService.UnifiedSearch("", filter, 1); // Empty query for popular movies

            auto result = resultFuture.get();

            if (result.IsOk()) {
                ipc::json movieArray = ipc::json::array();

                // Combine results from both pages
                std::vector<domain::MediaMetadata> allMovies = result.Value();

                for (const auto& movie : allMovies) {

                    try {
                        // Safely extract values
                        const std::string& safeTitle = !movie.title.empty() ? 
                            movie.title : "Untitled";
                        const std::string& safeOverview = !movie.overview.empty() ? 
                            movie.overview : "No overview available";
                        const std::string& safeId = fmt::format("{}:{}", 
                            movie.id.source, movie.id.id);

                        // Build movie JSON
                        ipc::json movieJson = {
                            {"title", safeTitle},
                            {"overview", safeOverview},
                            {"rating", movie.rating},
                            {"voteCount", movie.voteCount},
                            {"id", safeId}
                        };

                        // Handle optional poster path
                        if (movie.posterPath.has_value() && !movie.posterPath->empty()) {
                            movieJson["poster"] = *movie.posterPath;
                        } else {
                            movieJson["poster"] = nullptr;
                        }

                        movieArray.push_back(movieJson);
                        utils::Logger::Info(fmt::format("Processed: {}", safeTitle));
                    }
                    catch (const std::exception& ex) {
                        utils::Logger::Error(fmt::format("Movie processing error: {}", ex.what()));
                        continue; // Skip invalid entries
                    }
                }

                // Send response
                ipc::json response = {
                    {"success", true},
                    {"movies", movieArray}
                };
                respond(response);
                utils::Logger::Info(fmt::format("Sent {} movies", movieArray.size()));
            }
            else {
                // Handle errors
                std::string errorMsg;
                if (!result.IsOk()) errorMsg = result.GetError().message;
                
                utils::Logger::Error(fmt::format("Failed to fetch movies: {}", errorMsg));
                respond({{"success", false}, {"error", errorMsg}});
            }
        }
        catch (const std::exception& ex) {
            utils::Logger::Error(fmt::format("IPC handler failed: {}", ex.what()));
            respond({{"success", false}, {"error", ex.what()}});
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