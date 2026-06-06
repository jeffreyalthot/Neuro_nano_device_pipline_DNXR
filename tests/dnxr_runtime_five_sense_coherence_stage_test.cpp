#include "runtime/RuntimeFiveSenseCoherenceStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; f.fullDiveNeuralFrame.safety.emergencyCutoffAvailable=true; auto o=dnxr::RuntimeFiveSenseCoherenceStage::execute(f); assert(o.continueCycle); return 0; }
