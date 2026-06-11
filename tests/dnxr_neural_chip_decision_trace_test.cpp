#include "neural_chip/NeuralChipPipelineDecisionTrace.h"
#include <cassert>

int main(){ dnxr::NeuralChipPipelineDecisionTrace t; t.record({1,"Stage","Visual","Rule",0.8f,0.4f,0.2f}); assert(t.events().size()==1); t.clear(); assert(t.events().empty()); }
