#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace dnxr {

struct SchedulerTaskBudget {
    std::string name;
    int priority{0};
    double targetMs{0.0};
};

struct SchedulerTickReport {
    double totalMs{0.0};
    double maxTaskMs{0.0};
    double jitterMs{0.0};
    bool withinBudget{true};
};

class NeuroRealtimeScheduler {
public:
    explicit NeuroRealtimeScheduler(double frameBudgetMs = 20.0);
    void registerTask(const SchedulerTaskBudget& task);
    SchedulerTickReport beginTick();
    void endTask(const std::string& taskName, const std::chrono::steady_clock::time_point& start, SchedulerTickReport& report);
    void endTick(SchedulerTickReport& report);
    double frameBudgetMs() const;

private:
    double frameBudgetMs_;
    std::vector<SchedulerTaskBudget> tasks_;
    std::chrono::steady_clock::time_point tickStart_;
    double previousTickMs_{0.0};
};

} // namespace dnxr
