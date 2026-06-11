#include "neural_chip/StateMachineTransitionValidator.h"
#include <cassert>
int main(){
    dnxr::StateMachineTransitionValidator v;
    using S=dnxr::NeuralChipPipelineState;
    assert(v.allows(S::ColdStart,S::Running)); assert(!v.allows(S::ColdStart,S::AwaitingAck));
    assert(v.allows(S::Running,S::AwaitingAck)); assert(v.allows(S::Running,S::EmergencyLatched)); assert(v.allows(S::Running,S::Shutdown)); assert(!v.allows(S::Running,S::HoldNeutral));
    assert(v.allows(S::AwaitingAck,S::Running)); assert(v.allows(S::AwaitingAck,S::HoldDecompression)); assert(!v.allows(S::AwaitingAck,S::HoldMinimal));
    assert(v.allows(S::HoldDecompression,S::HoldNeutral)); assert(v.allows(S::HoldDecompression,S::ControlledResume)); assert(!v.allows(S::HoldDecompression,S::Running));
    assert(v.allows(S::HoldNeutral,S::HoldMinimal)); assert(v.allows(S::HoldNeutral,S::ControlledResume));
    assert(v.allows(S::HoldMinimal,S::ControlledResume)); assert(!v.allows(S::HoldMinimal,S::Running));
    assert(v.allows(S::ControlledResume,S::Running)); assert(v.allows(S::ControlledResume,S::EmergencyLatched));
    assert(v.allows(S::EmergencyLatched,S::ControlledResume)); assert(v.allows(S::EmergencyLatched,S::Shutdown)); assert(!v.allows(S::EmergencyLatched,S::Running));
    assert(v.allows(S::Shutdown,S::Shutdown)); assert(!v.allows(S::Shutdown,S::Running));
}
