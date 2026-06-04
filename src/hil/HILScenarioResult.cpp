#include "hil/HILScenarioResult.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

namespace {
double percentile(const std::vector<double>& sorted, double pct) {
    if (sorted.empty()) {
        return 0.0;
    }
    const double rank = (pct / 100.0) * static_cast<double>(sorted.size() - 1);
    const auto lower = static_cast<std::size_t>(std::floor(rank));
    const auto upper = static_cast<std::size_t>(std::ceil(rank));
    if (lower == upper) {
        return sorted[lower];
    }
    const double fraction = rank - static_cast<double>(lower);
    return sorted[lower] + (sorted[upper] - sorted[lower]) * fraction;
}
} // namespace

bool HILScenarioResult::isEvidenceComplete() const {
    return !scenarioId.empty() && !hazardIds.empty() && !riskControls.empty() && !phantomProfile.empty() &&
           !buildId.empty() && !softwareVersion.empty() && !expectedOutcome.empty() && !observedOutcome.empty() &&
           !operatorName.empty() && !reviewer.empty() && !timestamp.empty() && !artifactPath.empty();
}

HILLatencyStatistics computeHILLatencyStatistics(std::vector<double> samplesMs) {
    HILLatencyStatistics stats;
    if (samplesMs.empty()) {
        return stats;
    }
    std::sort(samplesMs.begin(), samplesMs.end());
    stats.worstMs = samplesMs.back();
    stats.p95Ms = percentile(samplesMs, 95.0);
    stats.p99Ms = percentile(samplesMs, 99.0);
    stats.averageMs = std::accumulate(samplesMs.begin(), samplesMs.end(), 0.0) / static_cast<double>(samplesMs.size());
    return stats;
}

} // namespace dnxr
