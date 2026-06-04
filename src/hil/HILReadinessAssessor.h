#pragma once
#include "HILEvidenceRecorder.h"
#include <string>
#include <vector>
namespace dnxr { struct HILReadinessReport { bool ready=false; std::vector<std::string> gaps; }; class HILReadinessAssessor { public: HILReadinessReport assess(const HILEvidenceRecorder& recorder) const; }; }
