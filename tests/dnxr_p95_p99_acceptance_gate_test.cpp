#include "hil/P95P99AcceptanceGate.h"
#include <cassert>
int main(){ dnxr::P95P99AcceptanceGate gate; assert(gate.evaluate({}).status==dnxr::P95P99AcceptanceStatus::EvidenceIncomplete); dnxr::HILBenchLatencyEvidence e; e.present=true; e.ackLatencyP95=10; e.ackLatencyP99=20; e.holdLatencyP95=10; e.emergencyLatchP99=40; e.replayExportP99=40; assert(gate.evaluate(e).status==dnxr::P95P99AcceptanceStatus::Accepted); e.ackLatencyP99=100; assert(gate.evaluate(e).status==dnxr::P95P99AcceptanceStatus::Blocked); }
