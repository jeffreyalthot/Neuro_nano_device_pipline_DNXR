#include "neural_chip/NeuralChipAckHandshake.h"
#include <cassert>
int main(){ dnxr::NeuralChipAckHandshake h(10.0); assert(!h.state().hasPendingAck); assert(!h.shouldHold(100.0)); h.expect(7,100.0); assert(h.state().hasPendingAck); assert(!h.shouldHold(105.0)); h.receiveAck(8,1.0f); assert(h.state().hasPendingAck); assert(h.state().wrongFrameIdRejected); assert(h.shouldHold(111.0)); h.receiveAck(7,0.2f); assert(h.state().hasPendingAck); assert(h.shouldHold(112.0)); h.receiveAck(7,0.8f); assert(!h.state().hasPendingAck); assert(!h.shouldHold(200.0)); }
