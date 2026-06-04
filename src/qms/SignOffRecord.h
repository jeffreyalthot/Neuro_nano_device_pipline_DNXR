#pragma once

#include <string>

namespace dnxr::qms {

struct SignOffRecord {
    std::string id;
    std::string artifactId;
    std::string signer;
    std::string role;
    std::string date;
    std::string residualRiskDisposition;
    bool signedByIndependentReviewer = false;

    bool isValid() const;
    bool acceptsResidualRisk() const;
};

} // namespace dnxr::qms
