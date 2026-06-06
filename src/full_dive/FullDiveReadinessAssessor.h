#pragma once
#include "FullDiveCapabilityMatrix.h"
#include <string>
#include <vector>
namespace dnxr {

enum class FullDiveReadinessStage {
    BenchOnly,
    AnimalOrPhantomPreclinical,
    FirstInHumanRestricted,
    HumanClinicalExpanded,
    NotApprovedForCommercialUse
};

const char* toString(FullDiveReadinessStage stage);

struct FullDiveReadinessReport {
    bool integratedSimulationReady=false;
    bool hilReady=false;
    FullDiveReadinessStage stage=FullDiveReadinessStage::BenchOnly;
    std::vector<std::string> gaps;
};

class FullDiveReadinessAssessor { public: FullDiveReadinessReport assess(const FullDiveCapabilityMatrix& matrix) const; };
}
