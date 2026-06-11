#include "hardware/BenchOnlyNeuralHardwareAdapter.h"
#include <cassert>
int main(){ dnxr::BenchOnlyNeuralHardwareAdapter a; assert(a.validate({}).accepted); dnxr::BenchOnlyHardwareRequest r; r.humanSubjectAttached=true; assert(!a.validate(r).accepted); r={}; r.allowStimulusDispatch=true; assert(!a.validate(r).accepted); r={}; r.missingAckLine=true; assert(!a.validate(r).accepted); }
