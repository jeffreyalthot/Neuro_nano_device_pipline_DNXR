#include "neural_chip/DecisionTraceCoverageValidator.h"
#include <cassert>
int main(){ dnxr::NeuralChipPipelineDecisionTrace t; for(auto s: dnxr::DecisionTraceCoverageValidator::requiredStages()) t.record({1,s,"frame","","StageEvaluated",0,0,0}); assert(dnxr::DecisionTraceCoverageValidator().validate(t).complete); t.clear(); assert(!dnxr::DecisionTraceCoverageValidator().validate(t).complete); }
