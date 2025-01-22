#pragma once
#include <string>
#include <vector>

namespace app::native::data
{

    struct MovieSummary
    {
        std::string id;
        std::string title;
        std::string poster_path;
        int year;
        double rating;
        std::vector<std::string> genres;
    };

    struct MovieDetails
    {
        std::string id;
        std::string title;
        std::string overview;
        std::string poster_path;
        std::string backdrop_path;
        int year;
        int runtime;
        double rating;
        std::vector<std::string> genres;
        std::vector<std::string> cast;
        std::string director;
        // Add more fields as needed
    };

}