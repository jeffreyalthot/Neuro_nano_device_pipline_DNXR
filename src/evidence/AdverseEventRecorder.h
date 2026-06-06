#pragma once
#include <string>
#include <vector>
namespace dnxr { struct AdverseEventRecord { std::string sessionId; std::string severity; std::string description; std::string correctiveAction; std::string utcTimestamp; }; class AdverseEventRecorder { public: void record(const AdverseEventRecord& event); const std::vector<AdverseEventRecord>& events() const; private: std::vector<AdverseEventRecord> events_; }; }
