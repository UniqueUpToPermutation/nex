#include <nex/error.hpp>

#include <sstream>
#include <filesystem>

using namespace nex;

std::ostream& nex::operator<<(std::ostream& os, const GlfwError& err) {
    os << "GLFW Error (Code: " << err.code << "): " << err.description;
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const RuntimeError& err) {
    os << "Runtime Error: " << err.description;
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const StringError& err) {
    os << err.description;
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const MultipleErrors& errs) {
    os << "Multiple Errors: \n";
    for (auto& err : errs.errors) {
        os << err.ToString() << "\n";
    }
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const InvalidPathError& err) {
    os << "Invalid Path: " << std::filesystem::absolute(err.path);
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const MissingUniformError& err) {
    os << "Missing uniform: " << err.name;
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const ShaderCompilationError& err) {
    os << "Failed to compile shader: " << err.path;
    return os;
}

std::ostream& nex::operator<<(std::ostream& os, const Error& err) {
    std::visit([&err, &os](auto const& obj) {
        if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, std::monostate>) {
            os << "No error";
        } else {
            if constexpr (!std::is_same_v<std::decay_t<decltype(obj)>, MultipleErrors>) {
                os << err.file << ":" << err.line << ": " << obj;
            } else {
                os << obj;
            }
        }
    }, err.details);
    return os;
}

Error nex::operator|(Error const& err1, Error const& err2) {
    Error result = err1;
    result |= err2;
    return result;
}

Error nex::operator+(Error const& err1, Error const& err2) {
    Error result = err1;
    result += err2;
    return result;
}

Error& nex::Error::operator|=(Error const& other) {
    if (!IsError() && other.IsError()) {
        *this = other;
    }
    return *this;
}

Error& nex::Error::operator+=(Error const& other) {
    if (IsError() && other.IsError()) {
        auto thisMulti = std::get_if<MultipleErrors>(&details);
        auto otherMulti = std::get_if<MultipleErrors>(&other.details);
        if (thisMulti && otherMulti) {
            std::copy(otherMulti->errors.begin(), otherMulti->errors.end(), 
                std::back_inserter(thisMulti->errors));
        } else if (thisMulti) {
            thisMulti->errors.emplace_back(other);
        } else if (otherMulti) {
            MultipleErrors merge{.errors = {*this}};
            std::copy(otherMulti->errors.begin(), otherMulti->errors.end(), 
                std::back_inserter(merge.errors));
            *this = Error{.details = std::move(merge)};
        } else {
            MultipleErrors merge{.errors = {*this, other}};
            *this = Error{.details = std::move(merge)};
        }
    } else if (!IsError() && other.IsError()) {
        *this = other;
    }

    return *this;
}

bool nex::Error::IsOk() const {
    return !IsError();
}

bool nex::Error::IsError() const {
    return ::IsError(details);
}

std::string nex::Error::ToString() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}