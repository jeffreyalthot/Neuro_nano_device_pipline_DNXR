#include "SensoryEncodingModelRegistry.h"

#include <algorithm>
#include <utility>

namespace dnxr {

SensoryEncodingModel::SensoryEncodingModel(std::string id, FullDiveModality modality, float maxIntensity, float nominalLatencyMs)
    : id_(std::move(id)), modality_(modality), maxIntensity_(std::max(0.0f, maxIntensity)), nominalLatencyMs_(std::max(0.0f, nominalLatencyMs)) {}

EncodingModelResult SensoryEncodingModel::encode(float intention, const std::string& targetRegion, const std::string& calibrationProfileId) const {
    const float bounded = std::clamp(intention, 0.0f, maxIntensity_);
    EncodingModelResult result;
    result.command.modality = modality_;
    result.command.channelId = id_ + ":" + targetRegion;
    result.command.targetRegion = targetRegion;
    result.command.encodingModelId = id_;
    result.command.intensity = bounded;
    result.command.frequencyHz = bounded * 90.0f;
    result.command.durationMs = 11;
    result.command.rampUpMs = bounded > 0.25f ? 25U : 10U;
    result.command.rampDownMs = 25;
    result.command.safetyLimit = maxIntensity_;
    result.command.calibrationProfileId = calibrationProfileId;
    result.command.confidence = validatePatientCalibration(calibrationProfileId) ? 0.85f : 0.0f;
    result.command.latencyBudgetMs = nominalLatencyMs_;
    result.expectedPerception = decodeExpectedPerception(bounded);
    result.risk = estimateRisk(bounded);
    result.latencyMs = estimateLatency();
    result.calibrationValid = validatePatientCalibration(calibrationProfileId);
    return result;
}

std::string SensoryEncodingModel::decodeExpectedPerception(float boundedIntensity) const {
    if (boundedIntensity <= 0.01f) return "neutral";
    if (boundedIntensity < maxIntensity_ * 0.35f) return "low_intensity_perception";
    if (boundedIntensity < maxIntensity_ * 0.75f) return "moderate_intensity_perception";
    return "clipped_high_intensity_perception";
}

bool SensoryEncodingModel::validatePatientCalibration(const std::string& calibrationProfileId) const { return !calibrationProfileId.empty(); }
float SensoryEncodingModel::estimateRisk(float boundedIntensity) const { return maxIntensity_ <= 0.0f ? 1.0f : std::clamp(boundedIntensity / maxIntensity_, 0.0f, 1.0f); }
float SensoryEncodingModel::estimateLatency() const { return nominalLatencyMs_; }
const std::string& SensoryEncodingModel::id() const { return id_; }
FullDiveModality SensoryEncodingModel::modality() const { return modality_; }

SensoryEncodingModelRegistry::SensoryEncodingModelRegistry() {
    registerModel({"visual_v1", FullDiveModality::Visual, 0.55f, 12.0f});
    registerModel({"auditory_v1", FullDiveModality::Auditory, 0.50f, 10.0f});
    registerModel({"touch_pressure_pain_blocked_v1", FullDiveModality::Somatosensory, 0.45f, 8.0f});
    registerModel({"proprioception_v1", FullDiveModality::Proprioceptive, 0.35f, 8.0f});
    registerModel({"vestibular_limited_v1", FullDiveModality::Vestibular, 0.20f, 6.0f});
    registerModel({"olfactory_persistent_v1", FullDiveModality::Olfactory, 0.12f, 250.0f});
    registerModel({"gustatory_discrete_v1", FullDiveModality::Gustatory, 0.08f, 350.0f});
    registerModel({"reality_anchor_v1", FullDiveModality::RealityAnchor, 1.0f, 5.0f});
}
void SensoryEncodingModelRegistry::registerModel(const SensoryEncodingModel& model) { models_[model.modality()] = model; }
const SensoryEncodingModel* SensoryEncodingModelRegistry::modelFor(FullDiveModality modality) const { auto it = models_.find(modality); return it == models_.end() ? nullptr : &it->second; }
EncodingModelResult SensoryEncodingModelRegistry::encode(FullDiveModality modality, float intention, const std::string& targetRegion, const std::string& calibrationProfileId) const {
    const auto* model = modelFor(modality);
    return model ? model->encode(intention, targetRegion, calibrationProfileId) : EncodingModelResult{};
}

} // namespace dnxr
