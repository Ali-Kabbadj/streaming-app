#pragma once
#include "window_base.hpp"
#include "ipc/ipc_manager.hpp"
#include "webview_host.hpp"
#include "utils/win32_utils.hpp"
#include <memory>

namespace app::ui
{

    class MainWindow : public WindowBase
    {
    public:
        MainWindow();
        ~MainWindow() override;

        bool Initialize(HINSTANCE hInstance, int nCmdShow);

    protected:
        std::wstring GetWindowTitle() const override;
        std::wstring GetWindowClassName() const override;
        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        std::unique_ptr<ipc::IpcManager> ipcManager_;
        std::unique_ptr<WebViewHost> webview_;

        void InitializeWebView();
        void SetupIpcHandlers();
        void OnSize(UINT width, UINT height);
    };

} // namespace app::ui
