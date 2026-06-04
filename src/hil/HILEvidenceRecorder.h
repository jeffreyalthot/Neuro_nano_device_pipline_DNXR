#pragma once
#include <string>
#include <vector>
namespace dnxr { struct HILEvidenceRecord { std::string scenario; bool passed=false; std::string artifact; }; class HILEvidenceRecorder { public: void record(HILEvidenceRecord record); std::vector<HILEvidenceRecord> records() const; bool allPassed() const; private: std::vector<HILEvidenceRecord> records_; }; }
