#include "ui/main_window.hpp"
#include "utils/logger.hpp"
#include <services/media/media_service.hpp>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    app::utils::Logger::Initialize();
    app::utils::Logger::Info("Starting application...");

    // Initialize services
    app::services::MediaService::Instance().Initialize();

    app::ui::MainWindow mainWindow;
    if (!mainWindow.Initialize(hInstance, nCmdShow))
    {
        app::utils::Logger::Error("Failed to initialize main window.");
        return 1;
    }

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    app::utils::Logger::Shutdown();
    return 0;
}
