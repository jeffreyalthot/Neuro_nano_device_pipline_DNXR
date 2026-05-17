// EmergencyShutdown.cpp

#include "EmergencyShutdown.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>

#include "../utils/Logger.h"

namespace dnxr {

namespace {

std::mutex g_shutdownMutex;
std::atomic<bool> g_emergencyActive{false};
std::atomic<uint64_t> g_triggerCount{0};

void simulatedStabilizationSequence() {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

} // namespace

EmergencyShutdown::EmergencyShutdown()
    : stabilizationTimeMs_(150),
      lastTriggerTime_(0),
      hardStopMode_(false) {
}

void EmergencyShutdown::trigger() const {
    engage();
}

void EmergencyShutdown::engage() const {

    std::lock_guard<std::mutex> lock(g_shutdownMutex);

    g_emergencyActive = true;
    ++g_triggerCount;

    Logger::warn(
        "EmergencyShutdown: safety protocol engaged.");

    simulatedStabilizationSequence();

    if (hardStopMode_) {
        Logger::error("EmergencyShutdown: HARD STOP engaged.");
    }

    Logger::info("EmergencyShutdown: system stabilized.");

    g_emergencyActive = false;
}

bool EmergencyShutdown::isStable() const noexcept {
    return !g_emergencyActive.load();
}

uint64_t EmergencyShutdown::getTriggerCount() const {
    return g_triggerCount.load();
}

void EmergencyShutdown::setHardStopMode(bool enabled) {
    hardStopMode_ = enabled;
}

bool EmergencyShutdown::isHardStopMode() const {
    return hardStopMode_;
}

void EmergencyShutdown::setStabilizationTime(int ms) {
    stabilizationTimeMs_ = std::max(1, ms);
}

int EmergencyShutdown::getStabilizationTime() const {
    return stabilizationTimeMs_;
}

} // namespace dnxr