#include "RuntimeRealtimeTelemetry.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

RuntimeRateContract RuntimeRateContract::negotiate(double requestedHz, double hardwareMaxHz) noexcept {
    RuntimeRateContract out;
    const double boundedRequest = std::isfinite(requestedHz) && requestedHz > 0.0 ? requestedHz : kDefaultTargetHz;
    const double boundedHardware = std::isfinite(hardwareMaxHz) && hardwareMaxHz > 0.0 ? hardwareMaxHz : kDefaultTargetHz;
    out.targetHz = boundedRequest;
    out.negotiatedHz = std::min(boundedRequest, boundedHardware);
    out.hardwareCompatible = boundedHardware >= boundedRequest;
    out.frameBudgetMs = 1000.0 / out.negotiatedHz;
    return out;
}

RuntimeRealtimeTelemetry::RuntimeRealtimeTelemetry(RuntimeRateContract rate) noexcept
    : rate_(rate) {}

void RuntimeRealtimeTelemetry::beginCycle(clock::time_point now) noexcept {
    cycleStart_ = now;
    if (hasPreviousCycleStart_) {
        const double periodMs = std::chrono::duration<double, std::milli>(now - previousCycleStart_).count();
        jitterMs_ = std::fabs(periodMs - rate_.frameBudgetMs);
    }
    previousCycleStart_ = now;
    hasPreviousCycleStart_ = true;
}

void RuntimeRealtimeTelemetry::endCycle(clock::time_point now) noexcept {
    lastCycleMs_ = std::max(0.0, std::chrono::duration<double, std::milli>(now - cycleStart_).count());
    maxCycleMs_ = std::max(maxCycleMs_, lastCycleMs_);
    ++cycles_;
}

void RuntimeRealtimeTelemetry::recordStage(CanonicalBciStage stage, double elapsedMs) noexcept {
    const std::size_t index = stageIndex(stage);
    const double boundedMs = std::isfinite(elapsedMs) ? std::max(0.0, elapsedMs) : 0.0;
    push(stageWindows_[index], boundedMs);
    RuntimeStageRealtimeStats& stats = stageStats_[index];
    ++stats.sampleCount;
    stats.lastMs = boundedMs;
    stats.maxMs = std::max(stats.maxMs, boundedMs);
    stats.wcetMs = stats.maxMs;
    stats.p50Ms = percentile(stageWindows_[index], 0.50);
    stats.p95Ms = percentile(stageWindows_[index], 0.95);
    stats.p99Ms = percentile(stageWindows_[index], 0.99);
}

void RuntimeRealtimeTelemetry::recordQueueDepths(std::size_t worldQueueDepth, std::size_t chipQueueDepth) noexcept {
    worldQueueDepth_ = worldQueueDepth;
    chipQueueDepth_ = chipQueueDepth;
}

void RuntimeRealtimeTelemetry::recordDroppedWorldFrame(const char* cause) noexcept {
    ++droppedWorldFrames_;
    lastDropCause_ = cause ? cause : "world_frame_dropped";
}

void RuntimeRealtimeTelemetry::recordHeldChipFrame(const char* cause) noexcept {
    ++heldChipFrames_;
    lastHoldCause_ = cause ? cause : "chip_frame_held";
}

RuntimeRealtimeSnapshot RuntimeRealtimeTelemetry::snapshot() const noexcept {
    RuntimeRealtimeSnapshot out;
    out.rate = rate_;
    out.cycles = cycles_;
    out.lastCycleMs = lastCycleMs_;
    out.maxCycleMs = maxCycleMs_;
    out.jitterMs = jitterMs_;
    out.droppedWorldFrames = droppedWorldFrames_;
    out.heldChipFrames = heldChipFrames_;
    out.lastDropCause = lastDropCause_;
    out.lastHoldCause = lastHoldCause_;
    out.worldQueueDepth = worldQueueDepth_;
    out.chipQueueDepth = chipQueueDepth_;
    out.stages = stageStats_;
    return out;
}

const RuntimeRateContract& RuntimeRealtimeTelemetry::rate() const noexcept {
    return rate_;
}

std::size_t RuntimeRealtimeTelemetry::stageIndex(CanonicalBciStage stage) noexcept {
    const auto raw = static_cast<std::size_t>(stage);
    return raw < kCanonicalRealtimeStageCount ? raw : (kCanonicalRealtimeStageCount - 1U);
}

double RuntimeRealtimeTelemetry::percentile(const RollingWindow& window, double q) noexcept {
    if (window.count == 0U) {
        return 0.0;
    }
    std::array<double, kWindow> sorted{};
    for (std::size_t i = 0; i < window.count; ++i) {
        sorted[i] = window.samples[i];
    }
    std::sort(sorted.begin(), sorted.begin() + static_cast<std::ptrdiff_t>(window.count));
    const double boundedQ = std::clamp(q, 0.0, 1.0);
    const std::size_t index = std::min(window.count - 1U,
        static_cast<std::size_t>(std::ceil(boundedQ * static_cast<double>(window.count)) - 1.0));
    return sorted[index];
}

void RuntimeRealtimeTelemetry::push(RollingWindow& window, double value) noexcept {
    window.samples[window.next] = value;
    window.next = (window.next + 1U) % kWindow;
    if (window.count < kWindow) {
        ++window.count;
    }
}

} // namespace dnxr
