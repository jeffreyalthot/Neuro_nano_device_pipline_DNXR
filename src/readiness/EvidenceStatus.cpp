#include "readiness/EvidenceStatus.h"

namespace dnxr {

EvidenceStatus::EvidenceStatus(EvidenceReviewState state) : state_(state) {}
EvidenceReviewState EvidenceStatus::state() const { return state_; }
bool EvidenceStatus::exists() const { return state_ != EvidenceReviewState::Missing && state_ != EvidenceReviewState::Rejected; }
bool EvidenceStatus::reviewed() const { return state_ == EvidenceReviewState::Reviewed || state_ == EvidenceReviewState::Signed; }
bool EvidenceStatus::signedOff() const { return state_ == EvidenceReviewState::Signed; }

} // namespace dnxr
