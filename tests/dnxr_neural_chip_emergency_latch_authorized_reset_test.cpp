#include "neural_chip/NeuralChipEmergencyLatch.h"
#include <cassert>
int main(){ dnxr::NeuralChipEmergencyLatch l; l.trigger("x"); assert(!l.resetWithOperatorAuthorization({1,"","r",true,true})); assert(l.latched()); assert(l.resetWithOperatorAuthorization({1,"op","sim reset",true,true})); assert(!l.latched()); }
