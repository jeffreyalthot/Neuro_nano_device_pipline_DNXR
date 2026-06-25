#include "../../src/hardware_bench/HardwareBenchSession.h"
#include <cassert>
int main(){ auto m=dnxr::HardwareBenchSession::dryRunManifest(); m.traceRecordingEnabled=false; assert(!dnxr::HardwareBenchSession::isAccepted(m)); }
