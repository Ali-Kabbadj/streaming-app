#include "rating_normalizer.hpp"

namespace app::services
{
    RatingNormalizer &RatingNormalizer::Instance()
    {
        static RatingNormalizer instance; // Singleton instance
        return instance;
    }

    float RatingNormalizer::NormalizeRating(const std::string &source, float rating)
    {
        // Example normalization logic
        if (source == "tmdb")
        {
            // TMDB ratings are on a scale of 0-10, normalize to 0-5
            return rating / 2.0f;
        }
        else if (source == "omdb")
        {
            // OMDb ratings are on a scale of 0-10, normalize to 0-5
            return rating / 2.0f;
        }
        else
        {
            // Default normalization (no change)
            return rating;
        }
    }
}