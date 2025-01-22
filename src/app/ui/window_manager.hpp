// ui/window_manager.hpp

namespace app::ui
{
    // ui/window_manager.hpp
    class WindowManager
    {
    public:
        using WindowId = std::string;

        Result<WindowId> CreateWindow(const WindowConfig &config);
        void DestroyWindow(const WindowId &id);
        Result<Window *> GetWindow(const WindowId &id);

    private:
        std::unordered_map<WindowId, std::unique_ptr<Window>> windows_;
        // ... implementation
    };
}