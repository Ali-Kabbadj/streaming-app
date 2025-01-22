#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>

namespace app::utils
{
    std::wstring GetLastErrorAsString();
    RECT GetWindowRect(HWND hwnd);
    void CenterWindow(HWND hwnd, HWND parent = nullptr);
    std::wstring LoadStringResource(UINT id);
    std::string WideToUtf8(const std::wstring &wstr);
    std::wstring Utf8ToWide(const std::string &str);
    std::string GetAppDataDirectory(const std::string &appName);
}