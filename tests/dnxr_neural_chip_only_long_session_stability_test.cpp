#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "neural_chip/NeuralVectorValidator.h"
#include <cassert>

int main(){ dnxr::NeuralChipBus bus; dnxr::NeuralChipOnlyFullDivePipeline p(bus); for(int i=1;i<=10000;++i){ dnxr::NeuralChipOnlyWorldFrame w; w.frameId=i; w.timestampMs=i; w.contacts.pressure01=(i%10)*0.01f; w.pose.ownership01=0.2f; bool ok=p.step(w); if(ok){ assert(dnxr::NeuralVectorValidator::validate(bus.lastAcceptedFrame()).accepted); assert(bus.lastAcceptedFrame().realityAnchor.externalWorldRecall01>=0.8f); assert(bus.lastAcceptedFrame().olfactory.odorPresence01==0.0f); assert(bus.lastAcceptedFrame().gustatory.sweet01==0.0f); } } assert(p.sessionState().acceptedFrames>0); }
