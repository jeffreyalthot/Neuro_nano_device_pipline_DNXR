#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class NeuralDeviceStatus {

    DISCONNECTED,
    CONNECTING,
    SYNCHRONIZING,
    CONNECTED,
    ERROR_STATE
};

struct NeuralElectrodeSignal {

    uint32_t channelId;

    float voltage;

    float impedance;

    float synchronization;

    float noiseLevel;

    bool active;
};

struct NeuralDeviceFrame {

    std::vector<NeuralElectrodeSignal>
    electrodeSignals;

    float globalSignalStrength;

    float corticalCoverage;

    float synchronizationQuality;

    float latency;

    float packetIntegrity;
};

class NeuralLinkDeviceInterface {
public:

    NeuralLinkDeviceInterface();

    void initialize();

    void connectDevice(
        const std::string& deviceIdentifier);

    void disconnectDevice();

    void synchronizeDevice();

    void readElectrodeSignals();

    void computeSignalStrength();

    void computeCorticalCoverage();

    void computeSynchronizationQuality();

    void computeLatency();

    void validatePacketIntegrity();

    void stabilizeSignalNoise();

    void update(
        float deltaTime);

    NeuralDeviceStatus
    getStatus() const;

    const NeuralDeviceFrame&
    getFrame() const;

private:

    NeuralDeviceStatus
    status_;

    NeuralDeviceFrame
    frame_;

    std::string
    connectedDevice_;
};

}
