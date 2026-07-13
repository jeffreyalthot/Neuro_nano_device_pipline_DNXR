#include "SafetyPipeline.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

float safetyPipelineClamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

float average(const std::vector<float>& values) {
    if (values.empty()) {
        return 0.0f;
    }
    return safetyPipelineClamp01(std::accumulate(values.begin(), values.end(), 0.0f) / static_cast<float>(values.size()));
}

float sensoryLoad(const std::vector<float>& frame) {
    if (frame.empty()) {
        return 0.0f;
    }
    float total = 0.0f;
    for (float value : frame) {
        total += std::abs(value);
    }
    return safetyPipelineClamp01(total / static_cast<float>(frame.size()));
}

int actionSeverity(SafetyAction action) {
    switch (action) {
        case SafetyAction::Continue: return 0;
        case SafetyAction::ReduceIntensity: return 1;
        case SafetyAction::Decompress: return 2;
        case SafetyAction::RealityAnchor: return 3;
        case SafetyAction::EmergencyStop: return 4;
        case SafetyAction::Lockout: return 5;
    }
    return 0;
}

std::string actionName(SafetyAction action) {
    switch (action) {
        case SafetyAction::Continue: return "continue";
        case SafetyAction::ReduceIntensity: return "reduce_intensity";
        case SafetyAction::Decompress: return "decompress";
        case SafetyAction::RealityAnchor: return "reality_anchor";
        case SafetyAction::EmergencyStop: return "emergency_stop";
        case SafetyAction::Lockout: return "lockout";
    }
    return "unknown";
}

SafetyDecisionLevel decisionLevelForLayer(const std::string& layerName) {
    if (layerName == "StimulusEnvelopeLayer") { return SafetyDecisionLevel::LocalEncoder; }
    if (layerName == "PhysiologicalLayer") { return SafetyDecisionLevel::Modality; }
    if (layerName == "CognitiveRiskLayer" || layerName == "RealityAnchorLayer") { return SafetyDecisionLevel::CrossModal; }
    if (layerName == "ConsentLayer" || layerName == "AddictionRiskLayer") { return SafetyDecisionLevel::Session; }
    if (layerName == "SignalIntegrityLayer") { return SafetyDecisionLevel::TransportHardware; }
    return SafetyDecisionLevel::PostFeedback;
}

SafetyLayerDecision makeDecision(
    const std::string& layerName,
    SafetyAction action,
    float risk,
    const std::string& reason) {

    SafetyLayerDecision decision;
    decision.layerName = layerName;
    decision.level = decisionLevelForLayer(layerName);
    decision.action = action;
    decision.riskScore = safetyPipelineClamp01(risk);
    decision.reason = reason;
    decision.authority = "canonical:" + layerName;

    switch (action) {
        case SafetyAction::Continue:
            decision.intensityScale = 1.0f;
            break;
        case SafetyAction::ReduceIntensity:
            decision.intensityScale = 0.70f;
            break;
        case SafetyAction::Decompress:
            decision.intensityScale = 0.55f;
            break;
        case SafetyAction::RealityAnchor:
            decision.intensityScale = 0.40f;
            break;
        case SafetyAction::EmergencyStop:
        case SafetyAction::Lockout:
            decision.intensityScale = 0.0f;
            decision.allowRender = false;
            decision.requiresEmergencyStop = true;
            break;
    }

    return decision;
}

class HeuristicSafetyLayer final : public ISafetyLayer {
public:
    explicit HeuristicSafetyLayer(std::string layerName)
        : layerName_(std::move(layerName)) {}

    std::string name() const override {
        return layerName_;
    }

    SafetyLayerDecision evaluate(const SafetyGovernorInput& input) const override {
        if (layerName_ == "ConsentLayer") {
            const float autonomyRisk = safetyPipelineClamp01(1.0f - input.diagnostics.autonomyPreservation);
            if (input.diagnostics.autonomyPreservation < 0.20f) {
                return makeDecision(layerName_, SafetyAction::EmergencyStop, autonomyRisk, "autonomy_preservation_failed");
            }
            return makeDecision(layerName_, SafetyAction::Continue, autonomyRisk, "consent_nominal");
        }

        if (layerName_ == "SignalIntegrityLayer") {
            const float signalRisk = safetyPipelineClamp01(1.0f - input.diagnostics.neuralSignalQuality);
            if (input.diagnostics.neuralSignalQuality < 0.25f) {
                return makeDecision(layerName_, SafetyAction::Decompress, signalRisk, "low_neural_signal_quality");
            }
            return makeDecision(layerName_, SafetyAction::Continue, signalRisk, "signal_integrity_nominal");
        }

        if (layerName_ == "PhysiologicalLayer") {
            const float risk = average(input.normalizedTelemetryRisk);
            if (risk >= 0.90f) {
                return makeDecision(layerName_, SafetyAction::EmergencyStop, risk, "physiological_emergency");
            }
            if (risk >= 0.60f) {
                return makeDecision(layerName_, SafetyAction::ReduceIntensity, risk, "physiological_warning");
            }
            return makeDecision(layerName_, SafetyAction::Continue, risk, "physiology_nominal");
        }

        if (layerName_ == "CognitiveRiskLayer") {
            const float risk = safetyPipelineClamp01(
                (input.cognitive.stress * 0.45f) +
                (input.cognitive.fatigue * 0.30f) +
                ((1.0f - input.cognitive.attention) * 0.25f));
            if (risk >= 0.80f || input.inferredState.find("dissociation") != std::string::npos) {
                return makeDecision(layerName_, SafetyAction::EmergencyStop, risk, "cognitive_or_dissociation_risk");
            }
            if (risk >= 0.55f) {
                return makeDecision(layerName_, SafetyAction::Decompress, risk, "cognitive_decompression_needed");
            }
            return makeDecision(layerName_, SafetyAction::Continue, risk, "cognition_nominal");
        }

        if (layerName_ == "StimulusEnvelopeLayer") {
            const float load = sensoryLoad(input.sensoryFrame);
            if (load >= 0.80f) {
                return makeDecision(layerName_, SafetyAction::ReduceIntensity, load, "sensory_load_high");
            }
            return makeDecision(layerName_, SafetyAction::Continue, load, "stimulus_envelope_nominal");
        }

        if (layerName_ == "AddictionRiskLayer") {
            const float risk = safetyPipelineClamp01(input.diagnostics.addictionRisk);
            if (input.diagnostics.addictionMitigationActive || risk > 0.75f) {
                return makeDecision(layerName_, SafetyAction::RealityAnchor, risk, "addiction_reality_anchor_required");
            }
            return makeDecision(layerName_, SafetyAction::Continue, risk, "addiction_risk_nominal");
        }

        if (layerName_ == "RealityAnchorLayer") {
            const float drift = safetyPipelineClamp01(input.diagnostics.decisionDrift);
            if (drift > 0.85f) {
                return makeDecision(layerName_, SafetyAction::EmergencyStop, drift, "decision_drift_emergency");
            }
            if (drift > 0.60f) {
                return makeDecision(layerName_, SafetyAction::RealityAnchor, drift, "reality_anchor_recommended");
            }
            return makeDecision(layerName_, SafetyAction::Continue, drift, "reality_anchor_nominal");
        }

        if (layerName_ == "EmergencyLayer") {
            const float risk = std::max(average(input.normalizedTelemetryRisk), safetyPipelineClamp01(input.diagnostics.decisionDrift));
            if (input.diagnostics.safetyMode == "emergency_stop" || risk >= 0.95f) {
                return makeDecision(layerName_, SafetyAction::EmergencyStop, risk, "emergency_layer_latched");
            }
            return makeDecision(layerName_, SafetyAction::Continue, risk, "emergency_layer_nominal");
        }

        return makeDecision(layerName_, SafetyAction::Continue, 0.0f, "layer_unconfigured");
    }

private:
    std::string layerName_;
};

} // namespace

bool SafetyDecisionTrace::auditedEveryLayer() const {
    if (layerDecisions.empty()) {
        return false;
    }
    for (const auto& decision : layerDecisions) {
        if (decision.layerName.empty() || decision.reason.empty()) {
            return false;
        }
    }
    return true;
}

std::string SafetyDecisionTrace::summarize() const {
    std::ostringstream out;
    out << "safety_trace:final=" << actionName(finalDecision.action)
        << "|risk=" << finalDecision.riskScore
        << "|layers=" << layerDecisions.size();
    for (const auto& decision : layerDecisions) {
        out << '|' << decision.layerName << ':' << SafetyPipeline::decisionLevelName(decision.level)
            << ':' << actionName(decision.action) << ':' << decision.reason;
    }
    return out.str();
}

SafetyPipeline SafetyPipeline::createDefault() {
    SafetyPipeline pipeline;
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("ConsentLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("SignalIntegrityLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("PhysiologicalLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("CognitiveRiskLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("StimulusEnvelopeLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("AddictionRiskLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("RealityAnchorLayer"));
    pipeline.addLayer(std::make_unique<HeuristicSafetyLayer>("EmergencyLayer"));
    return pipeline;
}

void SafetyPipeline::addLayer(std::unique_ptr<ISafetyLayer> layer) {
    if (layer) {
        layers_.push_back(std::move(layer));
    }
}

SafetyDecisionTrace SafetyPipeline::evaluate(const SafetyGovernorInput& input) const {
    SafetyDecisionTrace trace;
    for (const auto& layer : layers_) {
        trace.layerDecisions.push_back(layer->evaluate(input));
    }
    trace.finalDecision = aggregate(trace.layerDecisions);
    return trace;
}

std::size_t SafetyPipeline::layerCount() const {
    return layers_.size();
}

std::string SafetyPipeline::decisionLevelName(SafetyDecisionLevel level) {
    switch (level) {
        case SafetyDecisionLevel::LocalEncoder: return "local_encoder";
        case SafetyDecisionLevel::Modality: return "modality";
        case SafetyDecisionLevel::CrossModal: return "cross_modal";
        case SafetyDecisionLevel::Session: return "session";
        case SafetyDecisionLevel::TransportHardware: return "transport_hardware";
        case SafetyDecisionLevel::PostFeedback: return "post_feedback";
    }
    return "unknown";
}

SafetyDecision SafetyPipeline::aggregate(const std::vector<SafetyLayerDecision>& decisions) {
    SafetyDecision finalDecision;
    finalDecision.reason = decisions.empty() ? "no_layers_configured" : "aggregated_layer_decisions";

    for (const auto& decision : decisions) {
        finalDecision.riskScore = std::max(finalDecision.riskScore, decision.riskScore);
        finalDecision.intensityScale = std::min(finalDecision.intensityScale, decision.intensityScale);
        finalDecision.allowRender = finalDecision.allowRender && decision.allowRender;
        finalDecision.requiresEmergencyStop = finalDecision.requiresEmergencyStop || decision.requiresEmergencyStop;
        if (actionSeverity(decision.action) > actionSeverity(finalDecision.action)) {
            finalDecision.action = decision.action;
            finalDecision.reason = SafetyPipeline::decisionLevelName(decision.level) + ":" + decision.layerName + ":" + decision.reason;
        }
    }

    std::ostringstream audit;
    audit << "safety_pipeline:first_cause=" << finalDecision.reason << ":final=" << actionName(finalDecision.action)
          << ":layers=" << decisions.size();
    for (const auto& decision : decisions) {
        audit << "|" << SafetyPipeline::decisionLevelName(decision.level) << "/" << decision.layerName
              << "=" << actionName(decision.action) << ":" << decision.reason;
    }
    finalDecision.auditRecord = audit.str();
    return finalDecision;
}

} // namespace dnxr
