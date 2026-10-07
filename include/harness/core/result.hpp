#pragma once

#include <optional>
#include <string>
#include <utility>

namespace harness {

template <typename T>
class Result {
public:
    static Result ok(T value) {
        Result r;
        r.value_ = std::move(value);
        return r;
    }

    static Result err(std::string msg) {
        Result r;
        r.error_ = std::move(msg);
        return r;
    }

    bool ok() const { return value_.has_value(); }
    explicit operator bool() const { return ok(); }

    const T& value() const { return value_.value(); }
    const std::string& error() const { return error_; }

private:
    std::optional<T> value_;
    std::string error_;
};

template <>
class Result<void> {
public:
    static Result ok() { return Result{}; }

    static Result err(std::string msg) {
        Result r;
        r.error_ = std::move(msg);
        return r;
    }

    bool ok() const { return error_.empty(); }
    explicit operator bool() const { return ok(); }

    const std::string& error() const { return error_; }

private:
    std::string error_;
};

}  // namespace harness