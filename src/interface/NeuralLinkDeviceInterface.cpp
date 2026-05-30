#include "NeuralLinkDeviceInterface.h"

#include <cmath>

namespace neuroxr {

NeuralLinkDeviceInterface::
NeuralLinkDeviceInterface() {

    initialize();
}

void NeuralLinkDeviceInterface::
initialize() {

    status_ =
        NeuralDeviceStatus::DISCONNECTED;

    frame_.globalSignalStrength =
        0.0f;

    frame_.corticalCoverage =
        0.0f;

    frame_.synchronizationQuality =
        0.0f;

    frame_.latency =
        0.0f;

    frame_.packetIntegrity =
        1.0f;
}

void NeuralLinkDeviceInterface::
connectDevice(
    const std::string& deviceIdentifier) {

    connectedDevice_ =
        deviceIdentifier;

    status_ =
        NeuralDeviceStatus::CONNECTING;

    synchronizeDevice();
}

void NeuralLinkDeviceInterface::
disconnectDevice() {

    connectedDevice_.clear();

    status_ =
        NeuralDeviceStatus::DISCONNECTED;
}

void NeuralLinkDeviceInterface::
synchronizeDevice() {

    status_ =
        NeuralDeviceStatus::SYNCHRONIZING;

    frame_.electrodeSignals.clear();

    for(uint32_t i = 0;
        i < 4096;
        ++i) {

        NeuralElectrodeSignal signal;

        signal.channelId = i;

        signal.voltage = 0.5f;

        signal.impedance = 0.1f;

        signal.synchronization = 1.0f;

        signal.noiseLevel = 0.01f;

        signal.active = true;

        frame_.electrodeSignals
            .push_back(signal);
    }

    status_ =
        NeuralDeviceStatus::CONNECTED;
}

void NeuralLinkDeviceInterface::
readElectrodeSignals() {

    for(auto& signal :
        frame_.electrodeSignals) {

        signal.voltage =
            std::sin(
                static_cast<float>(
                    signal.channelId)
                * 0.01f);

        signal.synchronization =
            1.0f -
            signal.noiseLevel;
    }
}

void NeuralLinkDeviceInterface::
computeSignalStrength() {

    float strength = 0.0f;

    for(const auto& signal :
        frame_.electrodeSignals) {

        strength +=
            std::abs(signal.voltage);
    }

    if(!frame_.electrodeSignals.empty()) {

        strength /=
            static_cast<float>(
                frame_.electrodeSignals.size());
    }

    frame_.globalSignalStrength =
        strength;
}

void NeuralLinkDeviceInterface::
computeCorticalCoverage() {

    uint32_t activeCount = 0;

    for(const auto& signal :
        frame_.electrodeSignals) {

        if(signal.active) {

            ++activeCount;
        }
    }

    if(!frame_.electrodeSignals.empty()) {

        frame_.corticalCoverage =

            static_cast<float>(
                activeCount)

            /

            static_cast<float>(
                frame_.electrodeSignals.size());
    }
}

void NeuralLinkDeviceInterface::
computeSynchronizationQuality() {

    float sync = 0.0f;

    for(const auto& signal :
        frame_.electrodeSignals) {

        sync +=
            signal.synchronization;
    }

    if(!frame_.electrodeSignals.empty()) {

        sync /=
            static_cast<float>(
                frame_.electrodeSignals.size());
    }

    frame_.synchronizationQuality =
        sync;
}

void NeuralLinkDeviceInterface::
computeLatency() {

    frame_.latency =
        0.001f *
        (1.0f -
        frame_.synchronizationQuality);
}

void NeuralLinkDeviceInterface::
validatePacketIntegrity() {

    frame_.packetIntegrity =

        frame_.synchronizationQuality *

        frame_.globalSignalStrength;
}

void NeuralLinkDeviceInterface::
stabilizeSignalNoise() {

    for(auto& signal :
        frame_.electrodeSignals) {

        signal.noiseLevel *=
            0.99f;
    }
}

void NeuralLinkDeviceInterface::
update(
    float deltaTime) {

    if(status_ !=
        NeuralDeviceStatus::CONNECTED) {

        return;
    }

    readElectrodeSignals();

    computeSignalStrength();

    computeCorticalCoverage();

    computeSynchronizationQuality();

    computeLatency();

    validatePacketIntegrity();

    stabilizeSignalNoise();
}

NeuralDeviceStatus
NeuralLinkDeviceInterface::
getStatus() const {

    return status_;
}

const NeuralDeviceFrame&
NeuralLinkDeviceInterface::
getFrame() const {

    return frame_;
}

}