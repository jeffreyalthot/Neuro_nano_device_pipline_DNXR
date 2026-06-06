#include "safety/DecompressionStateMachine.h"
#include <cassert>
int main(){ dnxr::DecompressionStateMachine sm; assert(sm.request(false)==dnxr::DecompressionState::Requested); assert(sm.step()==dnxr::DecompressionState::RampDownVisual); return 0; }
