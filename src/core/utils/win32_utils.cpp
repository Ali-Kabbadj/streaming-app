#include "utils/win32_utils.hpp"

namespace app::utils
{

    std::wstring GetLastErrorAsString()
    {
        DWORD error = GetLastError();
        if (error == 0)
        {
            return std::wstring();
        }

        LPWSTR buffer = nullptr;
        size_t size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPWSTR>(&buffer),
            0,
            nullptr);

        std::wstring message(buffer, size);
        LocalFree(buffer);

        return message;
    }

    RECT GetWindowRect(HWND hwnd)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        return rect;
    }

    void CenterWindow(HWND hwnd, HWND parent)
    {
        RECT windowRect, parentRect;
        GetWindowRect(hwnd, &windowRect);

        if (parent)
        {
            GetWindowRect(parent, &parentRect);
        }
        else
        {
            parentRect.left = 0;
            parentRect.top = 0;
            parentRect.right = GetSystemMetrics(SM_CXSCREEN);
            parentRect.bottom = GetSystemMetrics(SM_CYSCREEN);
        }

        int width = windowRect.right - windowRect.left;
        int height = windowRect.bottom - windowRect.top;

        int x = ((parentRect.right - parentRect.left) - width) / 2 + parentRect.left;
        int y = ((parentRect.bottom - parentRect.top) - height) / 2 + parentRect.top;

        SetWindowPos(hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOSIZE);
    }

    std::string WideToUtf8(const std::wstring &wstr)
    {
        if (wstr.empty())
        {
            return std::string();
        }

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(),
                                              nullptr, 0, nullptr, nullptr);

        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(),
                            strTo.data(), size_needed, nullptr, nullptr);

        return strTo;
    }

    // std::wstring Utf8ToWide(const std::string &str)
    // {
    //     if (str.empty())
    //     {
    //         return std::wstring();
    //     }

    //     // Calculate the required buffer size for the wide string
    //     int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(),
    //                                           nullptr, 0);

    //     // Create a wide string buffer
    //     std::wstring wstr(size_needed, 0);

    //     // Perform the conversion
    //     MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(),
    //                         &wstr[0], size_needed);

    //     return wstr;
    // }

    std::wstring Utf8ToWide(const std::string &utf8Str)
    {
        if (utf8Str.empty())
            return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), NULL, 0);
        std::wstring wideStr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.size(), &wideStr[0], size_needed);
        return wideStr;
    }

} // namespace app::utils