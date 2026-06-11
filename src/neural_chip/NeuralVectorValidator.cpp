#include "neural_chip/NeuralVectorValidator.h"
#include "neural_chip/NeuralVectorFieldRegistry.h"

#include <cmath>
#include <string>

namespace dnxr {

bool NeuralVectorValidator::isUnitInterval(float value) {
    return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

NeuralVectorValidationResult NeuralVectorValidator::validate(const FullDiveChipFrame& frame) {
    const char* badModality = "";
    const char* badField = "";
    NeuralVectorFieldRegistry::forEachAllFrameFields(frame, [&](const char* modality, const char* field, float value, bool) {
        if (!badField[0] && !NeuralVectorValidator::isUnitInterval(value)) {
            badModality = modality;
            badField = field;
        }
    });
    if (badField[0]) {
        static thread_local std::string reason;
        reason = std::string(badModality) + "." + badField + " outside [0,1] or non-finite";
        return {false, reason.c_str()};
    }
    if (frame.safety.activeCeiling01 > DNXR_NEURAL_CHIP_ACTIVE_CEILING_01) return {false, "activeCeiling01 exceeds neural chip ceiling"};
    if (!frame.safety.rawSensoryPayloadBlocked) return {false, "raw sensory payload is not blocked"};
    if (!frame.safety.simulationOnly) return {false, "frame is not simulation-only"};
    if (!frame.safety.painMappingBlocked) return {false, "pain mapping is not blocked"};
    return {true, "accepted"};
}

} // namespace dnxr
