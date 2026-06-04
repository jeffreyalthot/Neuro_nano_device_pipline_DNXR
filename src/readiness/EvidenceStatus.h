#pragma once

#include "readiness/EvidenceReviewState.h"

namespace dnxr {

class EvidenceStatus {
public:
    explicit EvidenceStatus(EvidenceReviewState state = EvidenceReviewState::Missing);
    EvidenceReviewState state() const;
    bool exists() const;
    bool reviewed() const;
    bool signedOff() const;

private:
    EvidenceReviewState state_;
};

} // namespace dnxr
