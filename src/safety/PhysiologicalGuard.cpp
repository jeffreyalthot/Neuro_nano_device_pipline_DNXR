#include "PhysiologicalGuard.h"
namespace dnxr { bool PhysiologicalGuard::enforce(const std::vector<float>& telemetry) const { if(!monitor_.checkCardioRespiratoryWindow(telemetry)){ killSwitch_.trigger(); return false;} return true;} }
