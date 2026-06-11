#include "hil/HardwareBenchReadinessGate.h"
#include <cassert>
int main(){ dnxr::HardwareBenchReadinessGate gate("."); auto d=gate.evaluate(); assert(d.status==dnxr::HardwareBenchReadinessStatus::EvidenceIncomplete); assert(!d.missingReports.empty()); }
