#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>
#include <functional>
#include <thread>

namespace dnxr {

struct XRHeadsetRuntimeConfig {
    std::string endpoint;
    std::string deviceId;
    std::string authToken;

    bool simulationMode = true;
    std::uint32_t targetFramerate = 90;
    float telemetryUpdateRateHz = 120.0f;
    bool asyncTelemetry = true;     // Nouveau : thread background de télémétrie
};

/// Interface principale casque XR.
/// Gère la communication aller/retour avec le périphérique XR réel ou ses simulateurs.
class XRHeadsetInterface {
public:
    explicit XRHeadsetInterface(XRHeadsetRuntimeConfig config);
    ~XRHeadsetInterface();

    XRHeadsetInterface(const XRHeadsetInterface&) = delete;
    XRHeadsetInterface& operator=(const XRHeadsetInterface&) = delete;
    XRHeadsetInterface(XRHeadsetInterface&&) noexcept = default;
    XRHeadsetInterface& operator=(XRHeadsetInterface&&) noexcept = default;

    /// Envoie une frame sensorielle complète vers le casque XR.
    void pushSensoryFrame(const std::vector<float>& frame) const;

    /// Récupère la télémétrie biologique et motion tracking.
    std::vector<float> pullBioTelemetry() const;

    /// Récupère la dernière frame envoyée (en buffer local).
    std::vector<float> getLastSensoryFrame() const;

    /// Indicateurs d’état et méta‑données.
    bool isRuntimeReady() const noexcept;
    const std::string& endpoint() const noexcept;
    const std::string& deviceId() const noexcept;
    std::uint32_t targetFramerate() const noexcept;
    bool simulationModeEnabled() const noexcept;

    /// Callback optionnel pour télémétrie en direct (poussée automatique).
    void setTelemetryCallback(const std::function<void(const std::vector<float>&)>& cb);

private:
    void validateRuntimeConfig() const;
    void updateDynamicTelemetry() const;
    void runTelemetryThread();

private:
    XRHeadsetRuntimeConfig config_;
    std::atomic<bool> runtimeReady_;
    std::atomic<bool> telemetryThreadRunning_;

    mutable std::mutex frameMutex_;
    mutable std::vector<float> lastSensoryFrame_;

    mutable std::atomic<float> heartRate_;
    mutable std::atomic<float> stressIndex_;
    mutable std::atomic<float> focusIndex_;
    mutable std::atomic<float> neuralNoise_;
    mutable std::atomic<float> oxygenLevel_;
    mutable std::atomic<float> eyeStability_;
    mutable std::atomic<std::uint64_t> telemetryPhaseTicks_;

    std::function<void(const std::vector<float>&)> telemetryCallback_;
    std::thread telemetryThread_;
};

} // namespace dnxr
