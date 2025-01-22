// media/service_interface.hpp

namespace app::services
{
    class IMediaService
    {
    public:
        virtual ~IMediaService() = default;
        virtual Result<MovieDetails> GetMovieDetails(const std::string &id) = 0;
        virtual Result<std::vector<Movie>> GetMovies(const MovieFilter &filter) = 0;
        // ... other methods
    };

    // Implementation with proper dependency injection
    class MediaService : public IMediaService
    {
    private:
        std::unique_ptr<IMediaProvider> provider_;
        std::unique_ptr<ICache> cache_;
        std::shared_ptr<ILogger> logger_;

    public:
        MediaService(
            std::unique_ptr<IMediaProvider> provider,
            std::unique_ptr<ICache> cache,
            std::shared_ptr<ILogger> logger);
        // ... implement interface methods
    };
}