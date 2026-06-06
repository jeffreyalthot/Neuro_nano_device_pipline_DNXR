#include "hardware/NeuralChipSafetySupervisor.h"
#include <cassert>
int main(){ dnxr::NeuralChipAck a; a.deviceTemperature=42; auto d=dnxr::NeuralChipSafetySupervisor().evaluate(a,true,0); assert(!d.allowed); return 0; }
