#include "HostComputerBridge.h"
#include "../utils/Logger.h"

#include <stdexcept>
#include <random>
#include <thread>
#include <chrono>
#include <cmath>

namespace dnxr {

namespace {
float randomJitter(float amp) {
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-amp, amp);
    return dist(rng);
}
} // namespace

HostComputerBridge::HostComputerBridge(HostBridgeRuntimeConfig config)
    : config_(std::move(config))
{
    validateRuntimeConfig();

    Logger::info("[HostBridge] Initialized | up=" + config_.upstreamEndpoint +
                 " | down=" + config_.downstreamEndpoint +
                 (config_.simulationMode ? " | simulation ON" : " | hardware mode"));
}

HostComputerBridge::~HostComputerBridge() {
    stopAsyncLink();
}

std::vector<float> HostComputerBridge::uplink(const std::vector<float>& signal) const {
    validateRuntimeConfig();
    if (signal.empty()) {
        Logger::warn("[HostBridge] Empty uplink payload.");
        return {};
    }

    std::lock_guard<std::mutex> lock(bridgeMutex_);

    Logger::info("[HostBridge ↑] TX " + std::to_string(signal.size()) +
                 " samples → " + config_.upstreamEndpoint);

    if (config_.simulationMode) {
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(config_.linkDelayMs + randomJitter(config_.jitterAmplitude))));
    }

    auto processed = synthesizeResponse(signal, 1.02f);
    return processed;
}

std::vector<float> HostComputerBridge::downlink(const std::vector<float>& command) const {
    validateRuntimeConfig();
    if (command.empty()) {
        Logger::warn("[HostBridge] Empty downlink payload.");
        return {};
    }

    std::lock_guard<std::mutex> lock(bridgeMutex_);

    Logger::info("[HostBridge ↓] TX " + std::to_string(command.size()) +
                 " samples → " + config_.downstreamEndpoint);

    if (config_.simulationMode) {
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(config_.linkDelayMs + randomJitter(config_.jitterAmplitude))));
    }

    auto processed = synthesizeResponse(command, 0.98f);
    return processed;
}

std::vector<float> HostComputerBridge::synthesizeResponse(
    const std::vector<float>& input, float scale) const
{
    std::vector<float> out;
    out.reserve(input.size());
    for (float v : input) {
        out.push_back((v * scale) + randomJitter(0.008f));
    }
    return out;
}

void HostComputerBridge::validateRuntimeConfig() const {
    if (config_.upstreamEndpoint.empty())
        throw std::invalid_argument("HostBridge upstream endpoint missing");
    if (config_.downstreamEndpoint.empty())
        throw std::invalid_argument("HostBridge downstream endpoint missing");
    if (config_.authToken.empty())
        throw std::invalid_argument("HostBridge auth token missing");
}

void HostComputerBridge::startAsyncLink() {
    if (asyncRunning_) return;
    asyncRunning_ = true;

    asyncThread_ = std::thread(&HostComputerBridge::runBackgroundLink, this);
    Logger::info("[HostBridge] Async link started.");
}

void HostComputerBridge::stopAsyncLink() {
    if (!asyncRunning_) return;
    asyncRunning_ = false;

    if (asyncThread_.joinable())
        asyncThread_.join();

    Logger::info("[HostBridge] Async link stopped.");
}

bool HostComputerBridge::isLinkActive() const noexcept {
    return asyncRunning_.load();
}

void HostComputerBridge::setOnLinkEvent(const std::function<void(const std::string&)>& cb) {
    linkEventCallback_ = cb;
}

void HostComputerBridge::runBackgroundLink() {
    using namespace std::chrono;
    const auto interval = duration<double>(1.0 / config_.asyncRateHz);

    Logger::info("[HostBridge] Background link loop started.");
    while (asyncRunning_) {
        auto start = high_resolution_clock::now();

        // Exemple : transmet périodiquement un signal de synchronisation
        if (linkEventCallback_) {
            linkEventCallback_("heartbeat");
        }

        std::this_thread::sleep_for(duration_cast<milliseconds>(interval));
        auto elapsed = duration<double>(high_resolution_clock::now() - start).count();

        if (elapsed > (1.0 / config_.asyncRateHz) * 2.0)
            Logger::warn("[HostBridge] Warning: loop overrun detected.");
    }

    Logger::info("[HostBridge] Background link terminated.");
}

} // namespace dnxr
