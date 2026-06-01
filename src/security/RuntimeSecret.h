#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

namespace dnxr {

enum class SecretOrigin {
    Environment,
    SimulationDeterministic,
    Missing
};

class SecureBuffer {
public:
    SecureBuffer() = default;
    explicit SecureBuffer(std::string value) : value_(std::move(value)) {}
    SecureBuffer(const SecureBuffer&) = default;
    SecureBuffer& operator=(const SecureBuffer&) = default;
    SecureBuffer(SecureBuffer&& other) noexcept : value_(std::move(other.value_)) { other.clear(); }
    SecureBuffer& operator=(SecureBuffer&& other) noexcept {
        if (this != &other) {
            clear();
            value_ = std::move(other.value_);
            other.clear();
        }
        return *this;
    }
    ~SecureBuffer() { clear(); }

    void clear() {
        std::fill(value_.begin(), value_.end(), '\0');
        value_.clear();
    }
    std::string_view view() const { return value_; }
    bool empty() const { return value_.empty(); }

private:
    std::string value_;
};

struct RuntimeSecret {
    std::string id;
    std::string value;
    SecureBuffer secureValue;
    SecretOrigin origin = SecretOrigin::Missing;

    RuntimeSecret() = default;
    RuntimeSecret(std::string secretId, std::string secretValue, SecretOrigin secretOrigin)
        : id(std::move(secretId)),
          value(secretValue),
          secureValue(std::move(secretValue)),
          origin(secretOrigin) {}

    bool empty() const { return value.empty(); }
    bool simulationGenerated() const { return origin == SecretOrigin::SimulationDeterministic; }

    void clear() {
        std::fill(value.begin(), value.end(), '\0');
        value.clear();
        secureValue.clear();
        origin = SecretOrigin::Missing;
    }
};

} // namespace dnxr
