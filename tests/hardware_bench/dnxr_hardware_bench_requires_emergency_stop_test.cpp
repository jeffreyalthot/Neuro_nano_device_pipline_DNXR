#include "../../src/hardware_bench/HardwareBenchSession.h"
#include <cassert>
int main(){ auto m=dnxr::HardwareBenchSession::dryRunManifest(); m.emergencyStopVerified=false; assert(!dnxr::HardwareBenchSession::isAccepted(m)); }
