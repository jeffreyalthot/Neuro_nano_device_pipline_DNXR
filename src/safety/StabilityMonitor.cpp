// StabilityMonitor.cpp

#include "StabilityMonitor.h"

#include <algorithm>
#include <cmath>
#include <mutex>

namespace dnxr {

namespace {

struct Bounds {
    float bpmMin = 45.0f;
    float bpmMax = 130.0f;
    float rpmMin = 8.0f;
    float rpmMax = 28.0f;
};

Bounds g_bounds;

std::mutex g_boundsMutex;

bool inRange(float v, float minV, float maxV) {
    return std::isfinite(v) && v > minV && v < maxV;
}

float clamp(float v, float minV, float maxV) {
    return std::max(minV, std::min(maxV, v));
}

} // namespace

StabilityMonitor::StabilityMonitor()
    : strictMode_(true),
      anomalyTolerance_(0.08f),
      lastBpm_(0.0f),
      lastRpm_(0.0f) {
}

bool StabilityMonitor::checkCardioRespiratoryWindow(
    const std::vector<float>& telemetry) const {

    if (telemetry.size() < 2) {
        return false;
    }

    float bpm = telemetry[0];
    float rpm = telemetry[1];

    {
        std::lock_guard<std::mutex> lock(g_boundsMutex);
        bpm = clamp(bpm, g_bounds.bpmMin, g_bounds.bpmMax);
        rpm = clamp(rpm, g_bounds.rpmMin, g_bounds.rpmMax);
    }

    bool ok =
        inRange(bpm, g_bounds.bpmMin, g_bounds.bpmMax) &&
        inRange(rpm, g_bounds.rpmMin, g_bounds.rpmMax);

    if (ok) {
        lastBpm_ = bpm;
        lastRpm_ = rpm;
        return true;
    }

    return !strictMode_;
}

void StabilityMonitor::setBounds(float bpmMin, float bpmMax,
                                 float rpmMin, float rpmMax) {
    std::lock_guard<std::mutex> lock(g_boundsMutex);

    if (bpmMin < bpmMax) {
        g_bounds.bpmMin = bpmMin;
        g_bounds.bpmMax = bpmMax;
    }

    if (rpmMin < rpmMax) {
        g_bounds.rpmMin = rpmMin;
        g_bounds.rpmMax = rpmMax;
    }
}

float StabilityMonitor::getLastBpm() const {
    return lastBpm_;
}

float StabilityMonitor::getLastRpm() const {
    return lastRpm_;
}

void StabilityMonitor::setStrictMode(bool enabled) {
    strictMode_ = enabled;
}

bool StabilityMonitor::isStrictMode() const {
    return strictMode_;
}

void StabilityMonitor::setAnomalyTolerance(float t) {
    anomalyTolerance_ = std::clamp(t, 0.0f, 1.0f);
}

float StabilityMonitor::getAnomalyTolerance() const {
    return anomalyTolerance_;
}

} // namespace dnxr