#include "neural_chip/NeuralChipSessionSupervisor.h"
#include <cassert>
int main(){ dnxr::NeuralChipSessionSupervisor s; dnxr::NeuralChipSessionState st; dnxr::FullDiveChipFrame f; st.rejectedFrames=3; st.acceptedFrames=7; auto d=s.decide(st,f); assert(d.reduceAllModalities); f.realityAnchor.orientation01=0.5f; d=s.decide(st,f); assert(d.triggerEmergencyLatch && d.requireHold); }
