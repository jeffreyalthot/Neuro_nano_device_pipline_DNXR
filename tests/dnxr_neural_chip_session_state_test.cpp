#include "neural_chip/NeuralChipSessionState.h"
#include <cassert>

int main(){ dnxr::NeuralChipSessionState s; s.acceptedFrames=3; dnxr::resetNeuralChipSessionState(s,42); assert(s.sessionId==42); assert(s.acceptedFrames==0); assert(s.realityAnchorStability01==1.0f); }
