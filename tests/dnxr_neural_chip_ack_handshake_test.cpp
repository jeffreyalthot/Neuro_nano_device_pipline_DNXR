#include "neural_chip/NeuralChipAckHandshake.h"
#include <cassert>
int main(){ dnxr::NeuralChipAckHandshake h(10.0); h.expect(7, 100.0); assert(!h.shouldHold(105.0)); assert(h.shouldHold(111.0)); h.receiveAck(7, 0.8f); assert(!h.shouldHold(200.0)); assert(h.state().ackQuality01==0.8f); }
