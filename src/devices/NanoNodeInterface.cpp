#include "NanoNodeInterface.h"
#include "../utils/Logger.h"

#include <stdexcept>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

namespace dnxr {

namespace {
float generateNoise(float amp) {
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-amp, amp);
    return dist(rng);
}
} // namespace

NanoNodeInterface::NanoNodeInterface(NanoNodeRuntimeConfig config)
    : config_(std::move(config))
{
    validateRuntimeConfig();
    lastNodeBuffer_.resize(config_.expectedNodeCount, 0.0f);

    Logger::info("[NanoNode] Initialized | endpoint=" + config_.endpoint +
                 " | nodes=" + std::to_string(config_.expectedNodeCount) +
                 (config_.simulationMode ? " | simulation ON" : " | hardware mode"));
}

NanoNodeInterface::~NanoNodeInterface() {
    stopAsyncStream();
}

std::vector<float> NanoNodeInterface::readNeuralNodeSignals() const {
    validateRuntimeConfig();

    std::lock_guard<std::mutex> lock(bufferMutex_);

    if (config_.simulationMode) {
        simulateNodeStream(lastNodeBuffer_, config_.expectedNodeCount);
    } else {
        // Point d’intégration pour lecture matérielle future :
        // Exemple : socket, gRPC, ou SDK matériel NeuralBridge
        // (appel non implémenté ici par sécurité)
    }

    Logger::info("[NanoNode] Read " + std::to_string(lastNodeBuffer_.size()) + " samples.");
    return lastNodeBuffer_;
}

void NanoNodeInterface::writeNeuralStimulus(const std::vector<float>& vec) const {
    validateRuntimeConfig();

    if (vec.empty()) {
        Logger::warn("[NanoNode] Empty stimulus ignored.");
        return;
    }

    float energy = 0.0f;
    for (float v : vec) energy += std::abs(v);

    Logger::info("[NanoNode] Stimulus dispatched | " +
                 std::to_string(vec.size()) + " samples | energy=" +
                 std::to_string(energy));

    if (config_.simulationMode)
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    else {
        // À implémenter : envoi réseau / matériel
    }
}

void NanoNodeInterface::simulateNodeStream(std::vector<float>& out, std::size_t count) const {
    out.resize(count);
    static thread_local float phase = 0.0f;
    phase += 0.0003f;

    for (std::size_t i = 0; i < count; ++i) {
        float t = phase + static_cast<float>(i) * 0.00015f;
        out[i] = std::sin(t * 8.0f) * 0.35f
               + std::cos(t * 2.6f) * 0.22f
               + generateNoise(0.04f);
    }
}

void NanoNodeInterface::validateRuntimeConfig() const {
    if (config_.endpoint.empty())
        throw std::invalid_argument("NanoNode endpoint missing");
    if (config_.authToken.empty())
        throw std::invalid_argument("NanoNode authToken missing");
    if (config_.expectedNodeCount == 0)
        throw std::invalid_argument("NanoNode expectedNodeCount invalid");
}

void NanoNodeInterface::startAsyncStream() {
    if (streamRunning_) return;
    streamRunning_ = true;
    streamThread_ = std::thread(&NanoNodeInterface::runBackgroundLoop, this);
    Logger::info("[NanoNode] Async stream started.");
}

void NanoNodeInterface::stopAsyncStream() {
    if (!streamRunning_) return;
    streamRunning_ = false;
    if (streamThread_.joinable())
        streamThread_.join();
    Logger::info("[NanoNode] Async stream stopped.");
}

bool NanoNodeInterface::isStreamActive() const noexcept {
    return streamRunning_.load();
}

bool NanoNodeInterface::simulationModeEnabled() const noexcept {
    return config_.simulationMode;
}

void NanoNodeInterface::setOnSignalUpdate(
    const std::function<void(const std::vector<float>&)>& cb)
{
    onSignalUpdate_ = cb;
}

void NanoNodeInterface::runBackgroundLoop() {
    using namespace std::chrono;
    const double intervalSec = 1.0 / config_.updateRateHz;

    while (streamRunning_) {
        auto start = high_resolution_clock::now();

        auto signals = readNeuralNodeSignals();
        if (onSignalUpdate_) {
            onSignalUpdate_(signals);
        }

        auto elapsed = duration<double>(high_resolution_clock::now() - start).count();
        double sleepSec = std::max(0.0, intervalSec - elapsed);
        std::this_thread::sleep_for(duration<double>(sleepSec));
    }
}

} // namespace dnxr
