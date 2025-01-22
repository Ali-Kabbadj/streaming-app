#include <string>
#include <vector>
#include <unordered_map>

namespace app::services
{
    class RatingNormalizer
    {
    public:
        static RatingNormalizer &Instance();

        float NormalizeRating(const std::string &providerId, float rawRating);
        float GetAggregateRating(const std::vector<std::pair<std::string, float>> &ratings);

    private:
        struct ProviderRatingMetrics
        {
            float minRating;
            float maxRating;
            float avgRating;
            int sampleSize;
        };
        std::unordered_map<std::string, ProviderRatingMetrics> metrics_;
    };
}