#include "hardware/NeuralChipSafetySupervisor.h"
#include <cassert>
int main(){ dnxr::NeuralChipAck a; a.latencyMeasuredMs=25; auto d=dnxr::NeuralChipSafetySupervisor().evaluate(a,true,0); assert(!d.allowed || d.action==dnxr::NeuralChipSafetyAction::HoldLastSafeFrame); return 0; }
