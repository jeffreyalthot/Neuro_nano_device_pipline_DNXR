#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

class FrameBudgetMonitor {
public:
    explicit FrameBudgetMonitor(double budgetMs = 20.0) noexcept : budgetMs_(budgetMs) {}
    bool observe(double frameMs) noexcept;
    std::uint64_t violations() const noexcept { return violations_; }
    std::string report() const;
private:
    double budgetMs_;
    double worstMs_ = 0.0;
    std::uint64_t violations_ = 0;
};

} // namespace dnxr
