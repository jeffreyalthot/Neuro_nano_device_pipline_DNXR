#pragma once

#include <string>

namespace dnxr {

enum class EvidenceReviewState {
    Missing,
    Draft,
    Generated,
    Reviewed,
    Signed,
    Rejected,
    Superseded
};

std::string toString(EvidenceReviewState state);
EvidenceReviewState evidenceReviewStateFromString(const std::string& value);

} // namespace dnxr
