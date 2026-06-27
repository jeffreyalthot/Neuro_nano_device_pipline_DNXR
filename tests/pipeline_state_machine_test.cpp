#include "core/PipelineStateMachine.h"
#include <cassert>
int main(){ dnxr::PipelineStateMachine sm; assert(sm.transition(dnxr::PipelineState::Ready,"boot")); assert(sm.transition(dnxr::PipelineState::Running,"run")); assert(!sm.transition(dnxr::PipelineState::Booting,"invalid")); return 0; }
