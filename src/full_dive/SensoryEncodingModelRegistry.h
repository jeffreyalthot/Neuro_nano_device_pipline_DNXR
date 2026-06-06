#pragma once

#include "FullDiveNeuralFrame.h"

#include <map>
#include <string>

namespace dnxr {

struct EncodingModelResult {
    NeuroChannelCommand command;
    std::string expectedPerception;
    float risk = 1.0f;
    float latencyMs = 1000.0f;
    bool calibrationValid = false;
};

class SensoryEncodingModel {
public:
    SensoryEncodingModel() = default;
    SensoryEncodingModel(std::string id, FullDiveModality modality, float maxIntensity, float nominalLatencyMs);
    EncodingModelResult encode(float intention, const std::string& targetRegion, const std::string& calibrationProfileId) const;
    std::string decodeExpectedPerception(float boundedIntensity) const;
    bool validatePatientCalibration(const std::string& calibrationProfileId) const;
    float estimateRisk(float boundedIntensity) const;
    float estimateLatency() const;
    const std::string& id() const;
    FullDiveModality modality() const;

private:
    std::string id_ = "unconfigured";
    FullDiveModality modality_ = FullDiveModality::Visual;
    float maxIntensity_ = 0.0f;
    float nominalLatencyMs_ = 1000.0f;
};

using VisualEncodingModel = SensoryEncodingModel;
using AuditoryEncodingModel = SensoryEncodingModel;
using TouchEncodingModel = SensoryEncodingModel;
using PainBlockedEncodingModel = SensoryEncodingModel;
using ThermalEncodingModel = SensoryEncodingModel;
using VestibularEncodingModel = SensoryEncodingModel;
using ProprioceptionEncodingModel = SensoryEncodingModel;
using OlfactoryEncodingModel = SensoryEncodingModel;
using GustatoryEncodingModel = SensoryEncodingModel;

class SensoryEncodingModelRegistry {
public:
    SensoryEncodingModelRegistry();
    void registerModel(const SensoryEncodingModel& model);
    const SensoryEncodingModel* modelFor(FullDiveModality modality) const;
    EncodingModelResult encode(FullDiveModality modality, float intention,
                               const std::string& targetRegion,
                               const std::string& calibrationProfileId) const;

private:
    std::map<FullDiveModality, SensoryEncodingModel> models_;
};

} // namespace dnxr
