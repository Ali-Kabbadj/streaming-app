#include "ui/main_window.hpp"
#include "utils/logger.hpp"
#include "core/config/config_manager.hpp"
#include "services/media/media_service.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Initialize the logger
    app::utils::Logger::Initialize();
    app::utils::Logger::Info("Starting application...");

    // Load configuration from the AppData directory
    auto configLoadResult = app::config::ConfigManager::Instance().LoadFromAppData("StreamingApp");
    if (configLoadResult.IsError())
    {
        app::utils::Logger::Error("Failed to load config: " + configLoadResult.GetError().message);
        return 1;
    }

    // Initialize the main window
    app::ui::MainWindow mainWindow;
    if (!mainWindow.Initialize(hInstance, nCmdShow))
    {
        app::utils::Logger::Error("Failed to initialize main window.");
        return 1;
    }

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Shutdown the logger
    app::utils::Logger::Shutdown();
    return 0;
}