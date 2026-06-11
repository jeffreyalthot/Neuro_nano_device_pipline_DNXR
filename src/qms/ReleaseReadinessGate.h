#pragma once
#include <string>
#include <vector>
namespace dnxr {
enum class ReleaseReadinessStatus { Blocked, InternalCandidate, RNDReleaseCandidate, MedicalReviewCandidate };
struct ReleaseReadinessEvidence { bool cmakeConfigureOk=false, fullBuildOk=false, ctestAllOk=false, staticChecksOk=false, qmsDocsOk=false, traceabilityOk=false, noDisabledBlockingTests=false, hilP95P99Present=false, safetyCasePresent=false, evidenceBundleGenerated=false, noHumanUseClaims=false, humanUseGateBlocked=true, independentReviewPlanned=false; };
struct ReleaseReadinessDecision { ReleaseReadinessStatus status=ReleaseReadinessStatus::Blocked; std::vector<std::string> gaps; bool allowHumanUse=false; };
class ReleaseReadinessGate { public: ReleaseReadinessDecision evaluate(const ReleaseReadinessEvidence& evidence) const; };
}
