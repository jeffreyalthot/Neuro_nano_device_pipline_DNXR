#include "neural_chip/EmergencyResetAuditTrail.h"
namespace dnxr {
void EmergencyResetAuditTrail::record(const EmergencyResetAuditRecord& record) { records_.push_back(record); }
const std::vector<EmergencyResetAuditRecord>& EmergencyResetAuditTrail::records() const { return records_; }
} // namespace dnxr
