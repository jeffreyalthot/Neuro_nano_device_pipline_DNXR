#pragma once
#include <string>
#include <vector>
namespace dnxr { struct ClinicalReleaseInputs { bool testsPassed=false; bool risksAccepted=false; bool signedEvidenceComplete=false; bool requiredDocsComplete=false; bool sbomSigned=false; }; struct ClinicalReleaseDecision { bool releaseAllowed=false; std::vector<std::string> blockers; }; class ClinicalReleaseLock { public: ClinicalReleaseDecision evaluate(const ClinicalReleaseInputs& inputs) const; }; }
