#include "readiness/EvidenceReviewState.h"

namespace dnxr {

std::string toString(EvidenceReviewState state) {
    switch (state) {
    case EvidenceReviewState::Missing:
        return "Missing";
    case EvidenceReviewState::Draft:
        return "Draft";
    case EvidenceReviewState::Generated:
        return "Generated";
    case EvidenceReviewState::Reviewed:
        return "Reviewed";
    case EvidenceReviewState::Signed:
        return "Signed";
    case EvidenceReviewState::Rejected:
        return "Rejected";
    case EvidenceReviewState::Superseded:
        return "Superseded";
    }
    return "Missing";
}

EvidenceReviewState evidenceReviewStateFromString(const std::string& value) {
    if (value == "Draft") return EvidenceReviewState::Draft;
    if (value == "Generated") return EvidenceReviewState::Generated;
    if (value == "Reviewed") return EvidenceReviewState::Reviewed;
    if (value == "Signed") return EvidenceReviewState::Signed;
    if (value == "Rejected") return EvidenceReviewState::Rejected;
    if (value == "Superseded") return EvidenceReviewState::Superseded;
    return EvidenceReviewState::Missing;
}

} // namespace dnxr
