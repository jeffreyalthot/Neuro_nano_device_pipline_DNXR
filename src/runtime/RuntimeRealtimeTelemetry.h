#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace dnxr {

enum class CanonicalBciStage : std::uint8_t;
inline constexpr std::size_t kCanonicalRealtimeStageCount = 22U;

struct RuntimeRateContract {
    static constexpr double kDefaultTargetHz = 60.0;
    static constexpr double kDefaultFrameBudgetMs = 1000.0 / kDefaultTargetHz;

    double targetHz = kDefaultTargetHz;
    double negotiatedHz = kDefaultTargetHz;
    double frameBudgetMs = kDefaultFrameBudgetMs;
    bool hardwareCompatible = true;

    static RuntimeRateContract negotiate(double requestedHz, double hardwareMaxHz) noexcept;
};

struct RuntimeStageRealtimeStats {
    std::uint64_t sampleCount = 0;
    double lastMs = 0.0;
    double maxMs = 0.0;
    double wcetMs = 0.0;
    double p50Ms = 0.0;
    double p95Ms = 0.0;
    double p99Ms = 0.0;
};

struct RuntimeRealtimeSnapshot {
    RuntimeRateContract rate{};
    std::uint64_t cycles = 0;
    double lastCycleMs = 0.0;
    double maxCycleMs = 0.0;
    double jitterMs = 0.0;
    std::uint64_t droppedWorldFrames = 0;
    std::uint64_t heldChipFrames = 0;
    const char* lastDropCause = "none";
    const char* lastHoldCause = "none";
    std::size_t worldQueueDepth = 0;
    std::size_t chipQueueDepth = 0;
    std::array<RuntimeStageRealtimeStats, kCanonicalRealtimeStageCount> stages{};
};

class RuntimeRealtimeTelemetry {
public:
    using clock = std::chrono::steady_clock;

    explicit RuntimeRealtimeTelemetry(RuntimeRateContract rate = RuntimeRateContract{}) noexcept;

    void beginCycle(clock::time_point now) noexcept;
    void endCycle(clock::time_point now) noexcept;
    void recordStage(CanonicalBciStage stage, double elapsedMs) noexcept;
    void recordQueueDepths(std::size_t worldQueueDepth, std::size_t chipQueueDepth) noexcept;
    void recordDroppedWorldFrame(const char* cause) noexcept;
    void recordHeldChipFrame(const char* cause) noexcept;

    RuntimeRealtimeSnapshot snapshot() const noexcept;
    const RuntimeRateContract& rate() const noexcept;

private:
    static constexpr std::size_t kWindow = 64;

    struct RollingWindow {
        std::array<double, kWindow> samples{};
        std::size_t count = 0;
        std::size_t next = 0;
    };

    static std::size_t stageIndex(CanonicalBciStage stage) noexcept;
    static double percentile(const RollingWindow& window, double q) noexcept;
    static void push(RollingWindow& window, double value) noexcept;

    RuntimeRateContract rate_{};
    std::uint64_t cycles_ = 0;
    clock::time_point cycleStart_{};
    clock::time_point previousCycleStart_{};
    bool hasPreviousCycleStart_ = false;
    double lastCycleMs_ = 0.0;
    double maxCycleMs_ = 0.0;
    double jitterMs_ = 0.0;
    std::uint64_t droppedWorldFrames_ = 0;
    std::uint64_t heldChipFrames_ = 0;
    const char* lastDropCause_ = "none";
    const char* lastHoldCause_ = "none";
    std::size_t worldQueueDepth_ = 0;
    std::size_t chipQueueDepth_ = 0;
    std::array<RollingWindow, kCanonicalRealtimeStageCount> stageWindows_{};
    std::array<RuntimeStageRealtimeStats, kCanonicalRealtimeStageCount> stageStats_{};
};

template <typename T, std::size_t Capacity>
class RuntimeBoundedLatestQueue {
public:
    static_assert(Capacity > 0, "RuntimeBoundedLatestQueue capacity must be non-zero");

    bool pushLatest(const T& item) noexcept {
        if (size_ == Capacity) {
            buffer_[tail_] = item;
            tail_ = (tail_ + 1U) % Capacity;
            head_ = tail_;
            ++dropped_;
            return false;
        }
        buffer_[tail_] = item;
        tail_ = (tail_ + 1U) % Capacity;
        ++size_;
        return true;
    }

    bool pop(T& out) noexcept {
        if (size_ == 0U) {
            return false;
        }
        out = buffer_[head_];
        head_ = (head_ + 1U) % Capacity;
        --size_;
        return true;
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return Capacity; }
    std::uint64_t droppedCount() const noexcept { return dropped_; }

private:
    std::array<T, Capacity> buffer_{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t size_ = 0;
    std::uint64_t dropped_ = 0;
};

} // namespace dnxr
