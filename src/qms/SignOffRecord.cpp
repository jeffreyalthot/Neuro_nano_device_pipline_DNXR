#include "qms/SignOffRecord.h"

namespace dnxr::qms {

bool SignOffRecord::acceptsResidualRisk() const {
    return residualRiskDisposition == "accepted" || residualRiskDisposition == "accepted_with_controls";
}

bool SignOffRecord::isValid() const {
    return !id.empty() && !artifactId.empty() && !signer.empty() && !role.empty() && !date.empty() &&
           signedByIndependentReviewer && acceptsResidualRisk();
}

} // namespace dnxr::qms
