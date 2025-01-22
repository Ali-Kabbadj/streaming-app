#include "window_base.hpp"
#include "utils/logger.hpp"
#include "utils/win32_utils.hpp"

namespace app::window
{

    WindowBase::WindowBase() : hwnd_(nullptr), hInstance_(nullptr) {}

    bool WindowBase::Initialize(HINSTANCE hInstance, int nCmdShow)
    {
        hInstance_ = hInstance;

        if (!RegisterWindowClass())
        {
            utils::Logger::Error("Failed to register window class");
            return false;
        }

        if (!CreateWindowInstance())
        {
            utils::Logger::Error("Failed to create window instance");
            return false;
        }

        Show(nCmdShow);
        return true;
    }

    void WindowBase::Show(int nCmdShow)
    {
        ShowWindow(hwnd_, nCmdShow);
        UpdateWindow(hwnd_);
    }

    void WindowBase::Hide()
    {
        ShowWindow(hwnd_, SW_HIDE);
    }

    void WindowBase::Close()
    {
        if (hwnd_)
        {
            DestroyWindow(hwnd_);
        }
    }

    DWORD WindowBase::GetWindowStyle() const
    {
        return WS_OVERLAPPEDWINDOW;
    }

    DWORD WindowBase::GetWindowExStyle() const
    {
        return 0;
    }

    void WindowBase::OnSize(UINT width, UINT height)
    {
        // Base implementation does nothing
    }

    void WindowBase::OnClose()
    {
        Close();
    }

    LRESULT WindowBase::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_SIZE:
            OnSize(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_CLOSE:
            OnClose();
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd_, msg, wParam, lParam);
    }

    bool WindowBase::RegisterWindowClass()
    {
        auto threadId = GetCurrentThreadId();
        utils::Logger::Info("Registering window class in thread: " + std::to_string(threadId));
        utils::Logger::Info("Using HINSTANCE: " + std::to_string((uint64_t)hInstance_));

        // First check if class is already registered
        WNDCLASSEXW wc = {};
        if (GetClassInfoExW(hInstance_, GetWindowClassName().c_str(), &wc))
        {
            utils::Logger::Info("Window class already registered with WNDPROC: " +
                                std::to_string((uint64_t)wc.lpfnWndProc));
            return true;
        }

        // Store class name for verification
        std::wstring className = GetWindowClassName();

        // If not registered, proceed with registration
        wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProcSetup;
        wc.hInstance = hInstance_;
        wc.lpszClassName = className.c_str();
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        utils::Logger::Info("Attempting to register with WNDPROC: " +
                            std::to_string((uint64_t)WindowProcSetup));

        if (!RegisterClassExW(&wc))
        {
            DWORD error = GetLastError();
            utils::Logger::Error("RegisterClassExW failed with error: " + std::to_string(error));
            return false;
        }

        // Verify registration immediately
        WNDCLASSEXW wcVerify = {};
        if (!GetClassInfoExW(hInstance_, className.c_str(), &wcVerify))
        {
            DWORD error = GetLastError();
            utils::Logger::Error("Immediate verification failed. Error: " + std::to_string(error));
            return false;
        }

        utils::Logger::Info("Verified registration - Class name: " + utils::WideToUtf8(className));
        utils::Logger::Info("Registered WNDPROC matches: " +
                            std::to_string(wcVerify.lpfnWndProc == WindowProcSetup));

        return true;
    }

    bool WindowBase::CreateWindowInstance()
    {
        auto threadId = GetCurrentThreadId();
        utils::Logger::Info("Creating window in thread: " + std::to_string(threadId));

        DWORD style = GetWindowStyle();
        DWORD exStyle = GetWindowExStyle();

        // Store strings to ensure they remain valid
        std::wstring className = GetWindowClassName();
        std::wstring titleName = GetWindowTitle();

        // Create the window without passing 'this' initially
        hwnd_ = CreateWindowExW(
            exStyle,           // Extended window style
            className.c_str(), // Window class name
            titleName.c_str(), // Window title
            style,             // Window style
            CW_USEDEFAULT,     // Initial x position
            CW_USEDEFAULT,     // Initial y position
            1280,              // Initial width
            720,               // Initial height
            nullptr,           // Parent window handle
            nullptr,           // Menu handle
            hInstance_,        // Instance handle
            nullptr);          // Additional application data

        if (!hwnd_)
        {
            DWORD error = GetLastError();
            utils::Logger::Error("CreateWindowExW failed with error: " + std::to_string(error));
            return false;
        }

        // Store the this pointer after window creation
        SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        utils::Logger::Info("Window created successfully with handle: " + std::to_string((uint64_t)hwnd_));
        return true;
    }

    // Modify the WindowProc functions
    LRESULT CALLBACK WindowBase::WindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            utils::Logger::Info("WM_NCCREATE received");

            // Get the pointer to the window instance
            const CREATESTRUCTW *const create = reinterpret_cast<CREATESTRUCTW *>(lParam);

            // Set the window instance pointer
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));

            // Set up the window procedure
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowBase::WindowProcThunk));
        }

        // Forward any other messages to the default window procedure
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK WindowBase::WindowProcThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        // Get the pointer we stored during WM_NCCREATE
        WindowBase *const window = reinterpret_cast<WindowBase *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        // If we have a valid window pointer, forward the message
        if (window)
        {
            return window->HandleMessage(msg, wParam, lParam);
        }

        // If we don't have a window pointer yet, use the default handler
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

} // namespace app::window
