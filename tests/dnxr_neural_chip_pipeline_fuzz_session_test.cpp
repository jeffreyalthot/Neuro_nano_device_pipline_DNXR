#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "neural_chip/NeuralVectorValidator.h"
#include <cassert>
#include <random>
int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); std::mt19937 rng(580); std::uniform_real_distribution<float> u(0.0f,1.0f); double t=0; for(int i=0;i<1000;++i){ dnxr::NeuralChipOnlyWorldFrame w; w.frameId=i+1; t += (i%97==0 ? -0.5 : 1.0); w.timestampMs=t; w.scene.luminance01=u(rng); w.motion.angularVelocity01=u(rng); w.contacts.pressure01=u(rng); if(i%211==0) w.realityAnchor.orientation01=0.0f; p.step(w); const auto& f=bus.lastAcceptedFrame(); assert(dnxr::NeuralVectorValidator::validate(f).accepted); assert(f.safety.simulationOnly); assert(f.safety.rawSensoryPayloadBlocked); } }
