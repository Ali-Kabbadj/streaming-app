#pragma once
#include <variant>
#include <string>
#include <type_traits>

namespace app::utils
{

    template <typename T>
    class Result
    {
    public:
        struct Error
        {
            std::string message;
            int code;

            Error(std::string msg, int c = -1)
                : message(std::move(msg)), code(c) {}
        };

        // Constructors
        Result(T value) : data_(std::move(value)) {}
        Result(Error error) : data_(std::move(error)) {}

        // Status checks
        bool IsOk() const { return std::holds_alternative<T>(data_); }
        bool IsError() const { return std::holds_alternative<Error>(data_); }

        // Value access
        const T &Value() const &
        {
            if (IsError())
            {
                throw std::runtime_error(std::get<Error>(data_).message);
            }
            return std::get<T>(data_);
        }

        // Add this non-const overload
        T &Value() &
        {
            if (IsError())
            {
                throw std::runtime_error(std::get<Error>(data_).message);
            }
            return std::get<T>(data_);
        }

        T &&Value() &&
        {
            if (IsError())
            {
                throw std::runtime_error(std::get<Error>(data_).message);
            }
            return std::move(std::get<T>(data_));
        }

        const Error &GetError() const
        {
            if (!IsError())
            {
                throw std::runtime_error("Result contains a value, not an error");
            }
            return std::get<Error>(data_);
        }

        // Value or default
        T ValueOr(T &&default_value) const &
        {
            if (IsOk())
            {
                return Value();
            }
            return std::forward<T>(default_value);
        }

    private:
        std::variant<T, Error> data_;
    };

    // Specialization for void
    template <>
    class Result<void>
    {
    public:
        using Error = Result<int>::Error;

        Result() : error_(std::nullopt) {}
        Result(Error error) : error_(std::move(error)) {}

        bool IsOk() const { return !error_.has_value(); }
        bool IsError() const { return error_.has_value(); }

        void Value() const
        {
            if (IsError())
            {
                throw std::runtime_error(error_->message);
            }
        }

        const Error &GetError() const
        {
            if (!IsError())
            {
                throw std::runtime_error("Result contains a value, not an error");
            }
            return *error_;
        }

    private:
        std::optional<Error> error_;
    };

} // namespace app::utils