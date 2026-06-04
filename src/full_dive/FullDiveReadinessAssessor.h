#pragma once
#include "FullDiveCapabilityMatrix.h"
#include <string>
#include <vector>
namespace dnxr {
struct FullDiveReadinessReport { bool integratedSimulationReady=false; bool hilReady=false; std::vector<std::string> gaps; };
class FullDiveReadinessAssessor { public: FullDiveReadinessReport assess(const FullDiveCapabilityMatrix& matrix) const; };
}
