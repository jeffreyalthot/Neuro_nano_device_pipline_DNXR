#include "HILEvidenceRecorder.h"
namespace dnxr { void HILEvidenceRecorder::record(HILEvidenceRecord r){records_.push_back(r);} std::vector<HILEvidenceRecord> HILEvidenceRecorder::records() const{return records_;} bool HILEvidenceRecorder::allPassed() const{ if(records_.empty()) return false; for(const auto& r:records_) if(!r.passed) return false; return true;} }
