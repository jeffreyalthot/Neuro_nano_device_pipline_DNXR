#include "AdverseEventRecorder.h"
namespace dnxr { void AdverseEventRecorder::record(const AdverseEventRecord& e){ events_.push_back(e); } const std::vector<AdverseEventRecord>& AdverseEventRecorder::events() const { return events_; } }
