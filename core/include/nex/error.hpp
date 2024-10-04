#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <nonstd/expected.hpp>

#include <nex/log.hpp>

namespace nex {
    struct Error;

    struct RuntimeError {
        const char* description;
    };

    struct StringError {
        std::string description;
    };

    struct InvalidPathError {
        std::string path;
    };

    struct GlfwError {
        int code;
        std::string description;
    };

    struct MultipleErrors {
        std::vector<Error> errors;
    };

    struct MissingUniformError {
        const char* name;
    };

    std::ostream& operator<<(std::ostream& os, const GlfwError& err);
    std::ostream& operator<<(std::ostream& os, const RuntimeError& err);
    std::ostream& operator<<(std::ostream& os, const StringError& err);
    std::ostream& operator<<(std::ostream& os, const MultipleErrors& err);
    std::ostream& operator<<(std::ostream& os, const InvalidPathError& err);
    std::ostream& operator<<(std::ostream& os, const MissingUniformError& err);

    using ErrorDetails = std::variant<
        std::monostate, 
        GlfwError,
        RuntimeError,
        StringError,
        InvalidPathError,
        MissingUniformError,
        MultipleErrors>;
    struct Error;

    template <typename T>
    using Expected = nonstd::expected<T, Error>;
    template <typename T>
    struct ExpectedRef;

    struct Error {
        int line = 0;
        std::string_view file = "N/A";
        ErrorDetails details;

        bool IsOk() const;
        bool IsError() const;
        std::string ToString() const;

        Error& operator|=(Error const& other);
        Error& operator+=(Error const& other);

        // Unwraps the expected object and stores the error in this
        template <typename T>
        std::optional<T> Unwrap(Expected<T>&& ex);
        // Unwraps the expected object and stores the result in this
        template <typename T>
        T* Unwrap(ExpectedRef<T>&& ref);
    };

    template <typename T>
    struct is_error : std::false_type {};
    template <>
    struct is_error<Error> : std::true_type {};
    template <typename T>
    constexpr bool is_error_v = is_error<T>::value;

    std::ostream& operator<<(std::ostream& os, const Error& err);

    Error operator|(Error const& err1, Error const& err2);
    Error operator+(Error const& err1, Error const& err2);

    template <typename T>
    struct ExpectedRef {
        Expected<std::reference_wrapper<T>> value;

        inline T* operator->() {
            return &value.value().get();
        }

        inline operator bool() const {
            return value.has_value();
        }

        inline ExpectedRef(std::reference_wrapper<T> ref) : value(ref.get()) {}
        inline ExpectedRef(T& ref) : value(ref) {}
        inline ExpectedRef(nonstd::unexpected<Error> err) : value(err) {}
    };

    template <typename T>
    struct is_expected : std::false_type {};
    template <typename T>
    struct is_expected<Expected<T>> : std::true_type {};
    template <typename T>
    struct is_expected<ExpectedRef<T>> : std::true_type {};
    template <typename T>
    constexpr bool is_expected_v = is_expected<T>::value;

    template <typename T>
    constexpr bool is_expected_or_error_v = std::disjunction<is_expected<T>, is_error<T>>::value;

    inline bool IsError(ErrorDetails err) {
        return !std::holds_alternative<std::monostate>(err);
    }
    inline bool IsError(Error err) {
        return err.IsError();
    }
    inline Error MakeError(Error err) {
        return err;
    }
    template <typename T>
    inline bool IsError(Expected<T> const& exp) {
        return !exp.has_value();
    }
    template <typename T>
    inline bool IsError(ExpectedRef<T> const& exp) {
        return !exp.value.has_value();
    }
    template <typename T>
    inline Error MakeError(Expected<T> const& exp) {
        return exp.error();
    }
    template <typename T>
    inline Error MakeError(ExpectedRef<T> const& exp) {
        return exp.value.error();
    }
    inline nonstd::unexpected_type<Error> MakeUnexpected(Error err) {
        return nonstd::make_unexpected(err);
    }
    template <typename T>
    inline nonstd::unexpected_type<Error> MakeUnexpected(nonstd::expected<T, Error> exp) {
        return nonstd::make_unexpected(std::move(exp.error()));
    }
    template <typename T>
    inline nonstd::unexpected_type<Error> MakeUnexpected(ExpectedRef<T> ref) {
        return nonstd::make_unexpected(std::move(ref.value.error()));
    }

    template <typename T>
    std::optional<T> Error::Unwrap(Expected<T>&& ex) {
        if (!ex.has_value()) {
            *this = ex.error();
            return {};
        } else {
            return std::move(ex.value());
        }
    }
    template <typename T>
    T* Error::Unwrap(ExpectedRef<T>&& ref) {
        if (!ref.value.has_value()) {
            *this = ref.value.error();
            return {};
        } else {
            return &ref.value.value().get();
        }
    }

    template <typename T>
    void Log(Expected<T> const& exp, bool isWarning = false) {
        if (!exp.has_value()) {
            return Log(exp.error(), isWarning);
        }
    }

    template <typename T>
    void Log(ExpectedRef<T> const& exp, bool isWarning = false) {
        if (!exp.value.has_value()) {
            return Log(exp.value.error(), isWarning);
        }
    }

    template <typename T>
    T UnwrapAndWarn(Expected<T>&& expected, T&& defaultValue) {
        if (expected.has_value()) {
            return std::move(expected.value());
        } else {
            Log(expected, true);
            return std::move(defaultValue);
        }
    }
}

#define NEX_ERR_MAKE(errDetails) \
    nex::Error{__LINE__, __FILE__, errDetails}
#define NEX_ERR_SET(err, errDetails) \
    err = nex::Error{__LINE__, __FILE__, errDetails}
#define NEX_ERR_IF(cond, err, errDetails) \
    if (cond) { \
        NEX_ERR_SET(err, errDetails); \
    } 
#define NEX_ERR_RETURN(err) \
    static_assert(is_expected_or_error_v<std::decay_t<decltype(err)>>, \
        "Type of argument must be error or expected!"); \
    if (IsError(err)) { \
        return MakeError(err); \
    } 
#define NEX_ERR_RETURN_IF(cond, errDetails) \
    { \
        nex::Error err____reserved; \
        NEX_ERR_IF(cond, err____reserved, errDetails); \
        NEX_ERR_RETURN(err____reserved); \
    }
#define NEX_EXP_RETURN(err) \
    static_assert(is_expected_or_error_v<std::decay_t<decltype(err)>>, \
        "Type of argument must be error or expected!"); \
    if (IsError(err)) { \
        return MakeUnexpected(err); \
    }
#define NEX_EXP_RETURN_IF(cond, errDetails) \
    { \
        nex::Error err____reserved; \
        NEX_ERR_IF(cond, err____reserved, errDetails); \
        NEX_EXP_RETURN(err____reserved); \
    }
#define NEX_ERR_UNWRAP(statement, error) \
    error.Unwrap([&]() { return statement; }()) \
        .value_or(typename decltype(statement)::value_type{}); \
    NEX_ERR_RETURN(error);
#define NEX_EXP_UNWRAP(statement, error) \
    error.Unwrap([&]() { return statement; }()) \
        .value_or(typename decltype(statement)::value_type{}); \
    NEX_EXP_RETURN(error);
#define NEX_ERR_UNWRAP_INTO(dest, statement) \
    { \
        auto result____reserved = statement; \
        NEX_ERR_RETURN(result____reserved); \
        dest = std::move(result____reserved.value()); \
    }
#define NEX_EXP_UNWRAP_INTO(dest, statement) \
    { \
        auto result____reserved = statement; \
        NEX_EXP_RETURN(result____reserved); \
        dest = std::move(result____reserved.value()); \
    }
#define NEX_ERR_RETURN_IF_FAIL(statement) \
    { \
        auto err = statement; \
        NEX_ERR_RETURN(err); \
    }
#define NEX_EXP_RETURN_IF_FAIL(statement) \
    { \
        auto err = statement; \
        NEX_EXP_RETURN(err); \
    }

#define NEX_NO_COPY(name) name(name const&) = delete; name& operator=(name const&) = delete
#define NEX_MOVE_ONLY(name) NEX_NO_COPY(name); name(name&&) = default; name& operator=(name&&) = default;