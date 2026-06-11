#include "neural_chip/NeuralChipBus.h"
#include <cassert>
int main(){ dnxr::NeuralChipBus bus; dnxr::ChipReadFrame read; read.frameId=42; read.signalQuality01=0.7f; bus.injectSimulatedMotorReadFrame(read); assert(bus.submitSafetyHold({99,true,"hold"})); assert(bus.readNeuralActivity().frameId==42); assert(bus.lastSafetyHold().frameId==99); }
