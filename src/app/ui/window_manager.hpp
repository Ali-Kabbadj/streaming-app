// ui/window_manager.hpp

#include <string>
#include <atomic>
namespace app::ui
{
    class WindowManager
    {
    public:
        using WindowId = std::string;

        static WindowManager &Instance()
        {
            static WindowManager instance;
            return instance;
        }

        utils::Result<WindowId> CreateWindow(const WindowConfig &config)
        {
            try
            {
                auto window = std::make_unique<Window>(config);
                const auto id = GenerateWindowId();
                windows_[id] = std::move(window);
                return utils::Result<WindowId>(id);
            }
            catch (const std::exception &e)
            {
                return utils::Result<WindowId>::Error(e.what());
            }
        }

        void DestroyWindow(const WindowId &id)
        {
            if (auto it = windows_.find(id); it != windows_.end())
            {
                it->second->Close();
                windows_.erase(it);
            }
        }

        utils::Result<Window *> GetWindow(const WindowId &id)
        {
            if (auto it = windows_.find(id); it != windows_.end())
            {
                return utils::Result<Window *>(it->second.get());
            }
            return utils::Result<Window *>::Error("Window not found: " + id);
        }

    private:
        std::unordered_map<WindowId, std::unique_ptr<Window>> windows_;

        std::string GenerateWindowId()
        {
            static std::atomic<uint64_t> counter{0};
            return "window_" + std::to_string(++counter);
        }
    };
}