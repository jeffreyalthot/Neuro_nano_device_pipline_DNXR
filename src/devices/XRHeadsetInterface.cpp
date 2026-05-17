#include "XRHeadsetInterface.h"
#include "../utils/Logger.h"

#include <random>
#include <cmath>
#include <thread>
#include <chrono>
#include <stdexcept>

namespace dnxr {

namespace {
float generateNoise(float amplitude) {
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-amplitude, amplitude);
    return dist(rng);
}
} // namespace

XRHeadsetInterface::XRHeadsetInterface(XRHeadsetRuntimeConfig config)
    : config_(std::move(config)),
      runtimeReady_(false),
      telemetryThreadRunning_(false),
      heartRate_(72.0f),
      stressIndex_(0.15f),
      focusIndex_(0.85f),
      neuralNoise_(0.02f),
      oxygenLevel_(0.98f),
      eyeStability_(0.90f) {

    validateRuntimeConfig();
    runtimeReady_ = true;

    Logger::info("[XRHeadset] Initialized | device=" + config_.deviceId +
                 " | endpoint=" + config_.endpoint +
                 (config_.simulationMode ? " | simulation ON" : " | hardware mode"));

    if (config_.asyncTelemetry) {
        telemetryThreadRunning_ = true;
        telemetryThread_ = std::thread(&XRHeadsetInterface::runTelemetryThread, this);
    }
}

XRHeadsetInterface::~XRHeadsetInterface() {
    telemetryThreadRunning_ = false;
    if (telemetryThread_.joinable()) {
        telemetryThread_.join();
    }
}

void XRHeadsetInterface::pushSensoryFrame(const std::vector<float>& frame) const {
    if (!runtimeReady_) {
        Logger::warn("[XRHeadset] Not ready, frame ignored.");
        return;
    }
    if (frame.empty()) {
        Logger::warn("[XRHeadset] Empty frame rejected.");
        return;
    }

    {
        std::lock_guard<std::mutex> lock(frameMutex_);
        lastSensoryFrame_ = frame;
    }

    // En mode matériel, ici un vrai transfert data → casque
    if (config_.simulationMode) {
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }

    Logger::info("[XRHeadset] Frame sent | samples=" + std::to_string(frame.size()));
}

std::vector<float> XRHeadsetInterface::pullBioTelemetry() const {
    if (!runtimeReady_) {
        Logger::warn("[XRHeadset] Runtime not ready.");
        return {};
    }

    updateDynamicTelemetry();

    std::vector<float> telemetry = {
        heartRate_.load(),
        stressIndex_.load(),
        focusIndex_.load(),
        neuralNoise_.load(),
        oxygenLevel_.load(),
        eyeStability_.load()
    };

    {
        std::lock_guard<std::mutex> lock(frameMutex_);
        telemetry.push_back(static_cast<float>(lastSensoryFrame_.size()));
        telemetry.push_back(lastSensoryFrame_.empty() ? 0.0f : lastSensoryFrame_.front());
    }

    Logger::info("[XRHeadset] Telemetry pulled.");
    return telemetry;
}

void XRHeadsetInterface::updateDynamicTelemetry() const {
    static float phase = 0.0f;
    phase += 0.035f;

    heartRate_ = 72.0f + std::sin(phase * 0.8f) * 4.0f + generateNoise(1.0f);
    stressIndex_ = 0.15f + std::sin(phase * 0.3f) * 0.04f + generateNoise(0.005f);
    focusIndex_ = 0.85f + std::cos(phase * 0.2f) * 0.03f + generateNoise(0.005f);
    neuralNoise_ = 0.02f + std::abs(std::sin(phase * 0.5f)) * 0.01f;
    oxygenLevel_ = 0.98f + generateNoise(0.0015f);
    eyeStability_ = 0.90f + std::cos(phase * 0.4f) * 0.015f;
}

void XRHeadsetInterface::runTelemetryThread() {
    using namespace std::chrono;
    const auto interval = duration<double>(1.0 / config_.telemetryUpdateRateHz);

    while (telemetryThreadRunning_) {
        if (!runtimeReady_) {
            std::this_thread::sleep_for(milliseconds(50));
            continue;
        }

        updateDynamicTelemetry();

        if (telemetryCallback_) {
            std::vector<float> data = {
                heartRate_.load(),
                stressIndex_.load(),
                focusIndex_.load(),
                neuralNoise_.load(),
                oxygenLevel_.load(),
                eyeStability_.load()
            };
            telemetryCallback_(data);
        }

        std::this_thread::sleep_for(std::chrono::duration_cast<milliseconds>(interval));
    }
}

std::vector<float> XRHeadsetInterface::getLastSensoryFrame() const {
    std::lock_guard<std::mutex> lock(frameMutex_);
    return lastSensoryFrame_;
}

bool XRHeadsetInterface::isRuntimeReady() const noexcept { return runtimeReady_.load(); }
const std::string& XRHeadsetInterface::endpoint() const noexcept { return config_.endpoint; }
const std::string& XRHeadsetInterface::deviceId() const noexcept { return config_.deviceId; }
std::uint32_t XRHeadsetInterface::targetFramerate() const noexcept { return config_.targetFramerate; }
bool XRHeadsetInterface::simulationModeEnabled() const noexcept { return config_.simulationMode; }

void XRHeadsetInterface::setTelemetryCallback(const std::function<void(const std::vector<float>&)>& cb) {
    telemetryCallback_ = cb;
}

void XRHeadsetInterface::validateRuntimeConfig() const {
    if (config_.endpoint.empty())
        throw std::invalid_argument("XR endpoint empty");
    if (config_.deviceId.empty())
        throw std::invalid_argument("XR deviceId empty");
    if (config_.authToken.empty())
        throw std::invalid_argument("XR authToken empty");
}

} // namespace dnxr
