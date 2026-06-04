#pragma once
#include "FullDiveCapabilityMatrix.h"
#include <vector>
namespace dnxr {
class FullDiveEvidenceRecorder { public: void record(const FullDiveCapabilityEvidence& evidence); std::vector<FullDiveCapabilityEvidence> records() const; void clear(); private: std::vector<FullDiveCapabilityEvidence> records_; };
}
