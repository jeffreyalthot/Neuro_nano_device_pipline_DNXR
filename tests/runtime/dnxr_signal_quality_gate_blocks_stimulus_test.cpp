#include "../../src/runtime/RuntimeSignalStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; f.rawNeuralSignals={}; dnxr::RuntimeSignalStage::Input in; auto o=dnxr::RuntimeSignalStage::execute(f,in); assert(!o.continueCycle); assert(o.signalGateDecision.action==dnxr::NeuralSignalGateAction::BlockStimulus || o.signalGateDecision.action==dnxr::NeuralSignalGateAction::HoldLastSafeFrame); return 0; }
