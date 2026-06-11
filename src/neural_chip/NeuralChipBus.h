#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class NeuralChipBus {
public:
    ChipReadFrame readNeuralActivity() const;
    bool submitSensoryFrame(const FullDiveChipFrame& frame);
    void injectSimulatedMotorReadFrame(const ChipReadFrame& frame);
    bool submitSafetyHold(const NeuralSafetyFrame& safety);
    ChipHealthStatus getHealthStatus() const;
    const FullDiveChipFrame& lastAcceptedFrame() const;

private:
    static bool isSafeForChip(const FullDiveChipFrame& frame);

    ChipHealthStatus health_{};
    ChipReadFrame readFrame_{};
    FullDiveChipFrame lastFrame_{};
};

} // namespace dnxr
