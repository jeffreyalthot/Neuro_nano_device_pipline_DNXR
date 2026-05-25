#include "NeuroRealtimeScheduler.h"

#include <algorithm>

namespace dnxr {

NeuroRealtimeScheduler::NeuroRealtimeScheduler(double frameBudgetMs)
    : frameBudgetMs_(std::max(1.0, frameBudgetMs)) {}

void NeuroRealtimeScheduler::registerTask(const SchedulerTaskBudget& task) {
    tasks_.push_back(task);
    std::sort(tasks_.begin(), tasks_.end(), [](const auto& a, const auto& b) { return a.priority > b.priority; });
}

SchedulerTickReport NeuroRealtimeScheduler::beginTick() {
    tickStart_ = std::chrono::steady_clock::now();
    return SchedulerTickReport{};
}

void NeuroRealtimeScheduler::endTask(const std::string&, const std::chrono::steady_clock::time_point& start, SchedulerTickReport& report) {
    const auto now = std::chrono::steady_clock::now();
    const double taskMs = std::chrono::duration<double, std::milli>(now - start).count();
    report.maxTaskMs = std::max(report.maxTaskMs, taskMs);
}

void NeuroRealtimeScheduler::endTick(SchedulerTickReport& report) {
    report.totalMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - tickStart_).count();
    report.jitterMs = std::abs(report.totalMs - previousTickMs_);
    report.withinBudget = report.totalMs <= frameBudgetMs_;
    previousTickMs_ = report.totalMs;
}

double NeuroRealtimeScheduler::frameBudgetMs() const { return frameBudgetMs_; }

} // namespace dnxr
