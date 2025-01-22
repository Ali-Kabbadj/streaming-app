#include "movie_handler.hpp"
#include "data/data_manager.hpp"
#include "utils/logger.hpp"

namespace app::modules::api
{

    void MovieHandler::RegisterHandlers(app::ipc::IpcManager &ipcManager)
    {
        ipcManager.RegisterHandler("movies/list", [](const app::ipc::json &payload, auto respond)
                                   {
        try {
            int page = payload.at("page").get<int>();
            auto futureMovies = app::modules::data::DataManager::Instance().FetchMovies("", page);
            auto movies = futureMovies.get();

            app::ipc::json response = {
                {"movies", app::ipc::json::array()},
                {"current_page", page},
                {"total_pages", 5}  // Mock total pages
            };

            for (const auto& movie : movies) {
                response["movies"].push_back({
                    {"id", movie.id},
                    {"title", movie.title},
                    {"poster_path", movie.poster_path},
                    {"year", movie.year},
                    {"rating", movie.rating},
                    {"genres", movie.genres}
                });
            }
            respond(response);
        } catch (const std::exception& e) {
            respond({{"error", e.what()}});
        } });
    }

} // namespace app::modules::api
