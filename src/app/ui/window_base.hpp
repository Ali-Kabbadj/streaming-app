#pragma once
#include <Windows.h>
#include <string>

namespace app::ui
{

    class WindowBase

    {

    public:
        WindowBase();

        virtual ~WindowBase() = default;

        bool Initialize(HINSTANCE hInstance, int nCmdShow);

        void Show(int nCmdShow);

        void Hide();

        void Close();

        virtual DWORD GetWindowStyle() const;

        virtual DWORD GetWindowExStyle() const;

        virtual std::wstring GetWindowTitle() const = 0;

        virtual std::wstring GetWindowClassName() const = 0;
        HWND hwnd_;

    protected:
        virtual void OnSize(UINT width, UINT height);

        virtual void OnClose();

        virtual LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        static LRESULT CALLBACK WindowProcSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK WindowProcThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        bool RegisterWindowClass();

        bool CreateWindowInstance();

        HINSTANCE hInstance_;
    };
}
