#include "neural_chip/StepResultConsistencyVerifier.h"
#include <cassert>
int main(){ dnxr::PipelineStepResult ok{dnxr::PipelineStepOutcome::WaitingForAck,dnxr::PipelinePathKind::AwaitingAckWait,dnxr::NeuralChipPipelineState::AwaitingAck,false,true,"wait"}; assert(dnxr::StepResultConsistencyVerifier().verify(ok).valid); dnxr::PipelineStepResult bad{dnxr::PipelineStepOutcome::WaitingForAck,dnxr::PipelinePathKind::AwaitingAckWait,dnxr::NeuralChipPipelineState::AwaitingAck,true,true,"bad"}; assert(!dnxr::StepResultConsistencyVerifier().verify(bad).valid); }
