namespace app::error
{
    class ErrorHandler
    {
    public:
        template <typename T>
        static Result<T> Handle(std::function<T()> operation)
        {
            try
            {
                return operation();
            }
            catch (const std::exception &e)
            {
                Logger::Error("Operation failed: {}", e.what());
                return Result<T>::Error(e.what());
            }
        }
    };
}