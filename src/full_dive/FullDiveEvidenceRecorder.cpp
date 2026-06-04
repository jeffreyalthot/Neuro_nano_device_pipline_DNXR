#include "FullDiveEvidenceRecorder.h"
namespace dnxr { void FullDiveEvidenceRecorder::record(const FullDiveCapabilityEvidence& evidence){records_.push_back(evidence);} std::vector<FullDiveCapabilityEvidence> FullDiveEvidenceRecorder::records() const{return records_;} void FullDiveEvidenceRecorder::clear(){records_.clear();} }
