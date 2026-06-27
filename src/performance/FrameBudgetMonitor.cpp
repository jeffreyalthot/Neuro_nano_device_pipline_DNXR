#include "FrameBudgetMonitor.h"

#include <sstream>

namespace dnxr {

bool FrameBudgetMonitor::observe(double frameMs) noexcept {
    if (frameMs > worstMs_) worstMs_ = frameMs;
    if (frameMs > budgetMs_) { ++violations_; return false; }
    return true;
}

std::string FrameBudgetMonitor::report() const {
    std::ostringstream out;
    out << "frame_budget_ms=" << budgetMs_ << " worst_ms=" << worstMs_ << " violations=" << violations_;
    return out.str();
}

} // namespace dnxr
