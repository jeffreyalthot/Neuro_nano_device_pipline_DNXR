#include "neural_chip/NeuralChipEmergencyLatch.h"
#include <cassert>

int main(){ dnxr::NeuralChipEmergencyLatch l; assert(!l.latched()); l.trigger("patient stop"); assert(l.latched()); assert(l.reason()[0]=='p'); l.resetWithOperatorAuthorization(); assert(!l.latched()); }
