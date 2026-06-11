#include "neural_chip/StateMachineTransitionValidator.h"
#include <cassert>
int main(){ dnxr::StateMachineTransitionValidator v; dnxr::StateTransitionRequest r{dnxr::NeuralChipPipelineState::EmergencyLatched,dnxr::NeuralChipPipelineState::ControlledResume,"generic",false,false}; assert(!v.allows(r)); r.auditedReset=true; assert(!v.allows(r)); r.operatorAuthorized=true; assert(v.allows(r)); }
