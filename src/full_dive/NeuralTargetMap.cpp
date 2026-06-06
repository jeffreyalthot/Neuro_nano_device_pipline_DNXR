#include "NeuralTargetMap.h"
#include <algorithm>
namespace dnxr {
NeuralTargetMap::NeuralTargetMap() {
    targets_["VISUAL_PRIMARY"] = {"VISUAL_PRIMARY", {FullDiveModality::Visual}, 0.55f, 90.0f, 1.0f, true, true};
    targets_["VISUAL_DEPTH"] = {"VISUAL_DEPTH", {FullDiveModality::Visual}, 0.35f, 60.0f, 1.0f, true, true};
    targets_["AUDITORY_SPATIAL"] = {"AUDITORY_SPATIAL", {FullDiveModality::Auditory}, 0.50f, 200.0f, 1.0f, true, true};
    targets_["TACTILE_PRESSURE"] = {"TACTILE_PRESSURE", {FullDiveModality::Somatosensory}, 0.45f, 120.0f, 1.0f, true, true};
    targets_["TACTILE_TEXTURE"] = {"TACTILE_TEXTURE", {FullDiveModality::Somatosensory}, 0.35f, 180.0f, 1.0f, true, true};
    targets_["THERMAL_WARMTH"] = {"THERMAL_WARMTH", {FullDiveModality::Somatosensory}, 0.20f, 20.0f, 1.0f, true, true};
    targets_["PROPRIOCEPTIVE_LIMB_POSITION"] = {"PROPRIOCEPTIVE_LIMB_POSITION", {FullDiveModality::Proprioceptive}, 0.35f, 90.0f, 1.0f, true, true};
    targets_["VESTIBULAR_ROTATION"] = {"VESTIBULAR_ROTATION", {FullDiveModality::Vestibular}, 0.20f, 90.0f, 1.0f, true, true};
    targets_["OLFACTORY_PATTERN"] = {"OLFACTORY_PATTERN", {FullDiveModality::Olfactory}, 0.12f, 5.0f, 1.0f, true, true};
    targets_["GUSTATORY_PATTERN"] = {"GUSTATORY_PATTERN", {FullDiveModality::Gustatory}, 0.08f, 2.0f, 1.0f, true, true};
    targets_["REALITY_ANCHOR_SIGNAL"] = {"REALITY_ANCHOR_SIGNAL", {FullDiveModality::RealityAnchor}, 1.0f, 90.0f, 1.0f, false, false};
}
const NeuralTargetDescriptor* NeuralTargetMap::targetFor(const std::string& targetId) const { auto it = targets_.find(targetId); return it == targets_.end() ? nullptr : &it->second; }
const NeuralTargetDescriptor* NeuralTargetMap::defaultTargetFor(FullDiveModality modality) const { for (const auto& [_, t] : targets_) if (std::find(t.supportedModalities.begin(), t.supportedModalities.end(), modality) != t.supportedModalities.end()) return &t; return nullptr; }
bool NeuralTargetMap::validates(const NeuroChannelCommand& command) const {
    const auto* target = targetFor(command.targetRegion);
    if (!target) return false;
    if (std::find(target->supportedModalities.begin(), target->supportedModalities.end(), command.modality) == target->supportedModalities.end()) return false;
    if (command.intensity > target->maxIntensity || command.frequencyHz > target->maxFrequency) return false;
    return !target->requiresCalibration || !command.calibrationProfileId.empty();
}
} // namespace dnxr
