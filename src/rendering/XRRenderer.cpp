// XRRenderer.cpp

#include "XRRenderer.h"

#include "../utils/Logger.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <mutex>
#include <numeric>
#include <thread>

namespace dnxr {

namespace {

std::mutex g_renderMutex;

bool isValidFrame(const std::vector<float>& frame) {
    if (frame.empty()) {
        return false;
    }

    return std::all_of(frame.begin(), frame.end(), [](float v) {
        return std::isfinite(v);
    });
}

float computeFrameEnergy(const std::vector<float>& frame) {
    float sum = 0.0f;
    for (float v : frame) {
        sum += std::abs(v);
    }
    return sum;
}

} // namespace

XRRenderer::XRRenderer()
    : frameDropThreshold_(0.001f),
      vsyncEnabled_(true),
      lastFrameEnergy_(0.0f),
      frameCounter_(0) {
}

void XRRenderer::present(const std::vector<float>& frame) const {

    std::lock_guard<std::mutex> lock(g_renderMutex);

    if (!isValidFrame(frame)) {
        Logger::warn("XRRenderer rejected invalid frame.");
        return;
    }

    float energy = computeFrameEnergy(frame);
    lastFrameEnergy_ = energy;
    ++frameCounter_;

    // Simulation pipeline de rendu
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    Logger::info(
        "XR frame presented | channels=" +
        std::to_string(frame.size()) +
        " | energy=" +
        std::to_string(energy) +
        " | frameId=" +
        std::to_string(frameCounter_));
}

float XRRenderer::getLastFrameEnergy() const {
    return lastFrameEnergy_;
}

uint64_t XRRenderer::getFrameCount() const {
    return frameCounter_;
}

void XRRenderer::setVsyncEnabled(bool enabled) {
    vsyncEnabled_ = enabled;
}

bool XRRenderer::isVsyncEnabled() const {
    return vsyncEnabled_;
}

void XRRenderer::setFrameDropThreshold(float t) {
    frameDropThreshold_ = std::max(0.0f, t);
}

float XRRenderer::getFrameDropThreshold() const {
    return frameDropThreshold_;
}

} // namespace dnxr