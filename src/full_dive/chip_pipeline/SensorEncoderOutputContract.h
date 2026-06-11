#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct SensorEncoderContractResult { bool accepted=false; const char* encoder=""; const char* reason=""; };
class SensorEncoderOutputContract {
public:
    SensorEncoderContractResult validate(const NeuralVisualVector& v) const;
    SensorEncoderContractResult validate(const NeuralAuditoryVector& v) const;
    SensorEncoderContractResult validate(const NeuralTouchVector& v) const;
    SensorEncoderContractResult validate(const NeuralProprioceptionVector& v) const;
    SensorEncoderContractResult validate(const NeuralVestibularVector& v) const;
    SensorEncoderContractResult validate(const NeuralOlfactoryVector& v, bool enabled) const;
    SensorEncoderContractResult validate(const NeuralGustatoryVector& v, bool enabled) const;
};
}
