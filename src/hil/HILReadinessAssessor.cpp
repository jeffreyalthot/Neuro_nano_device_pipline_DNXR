#include "HILReadinessAssessor.h"
namespace dnxr { HILReadinessReport HILReadinessAssessor::assess(const HILEvidenceRecorder& recorder) const { HILReadinessReport r; r.ready=recorder.allPassed(); if(!r.ready) r.gaps.push_back("complete passing HIL evidence records"); return r; } }
