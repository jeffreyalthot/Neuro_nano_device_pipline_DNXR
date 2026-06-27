#include "hil/HardwareBenchReadinessGate.h"
#include <cassert>
int main(){ dnxr::HardwareBenchReadinessGate gate("__dnxr_missing_hardware_bench_evidence_root__"); auto d=gate.evaluate(); assert(d.status==dnxr::HardwareBenchReadinessStatus::EvidenceIncomplete); assert(!d.missingReports.empty()); }
