#pragma once
#include "FullDiveNeuralFrame.h"
#include <map>
#include <string>
#include <vector>
namespace dnxr {
struct NeuralTargetDescriptor {
    std::string targetId;
    std::vector<FullDiveModality> supportedModalities;
    float maxIntensity = 0.0f;
    float maxFrequency = 0.0f;
    float maxPulseWidth = 0.0f;
    bool requiresCalibration = true;
    bool requiresPhysicianApproval = true;
};
class NeuralTargetMap {
public:
    NeuralTargetMap();
    const NeuralTargetDescriptor* targetFor(const std::string& targetId) const;
    const NeuralTargetDescriptor* defaultTargetFor(FullDiveModality modality) const;
    bool validates(const NeuroChannelCommand& command) const;
private:
    std::map<std::string, NeuralTargetDescriptor> targets_;
};
} // namespace dnxr
