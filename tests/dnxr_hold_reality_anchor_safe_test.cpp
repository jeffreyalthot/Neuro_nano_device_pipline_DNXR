#include "neural_chip/NeuralChipHoldCommitPolicy.h"
#include <cassert>
int main(){ auto a=dnxr::NeuralChipHoldCommitPolicy::makeSafeHoldRealityAnchor(); assert(a.orientation01==1.0f); assert(a.identityContinuity01==1.0f); assert(a.sessionBoundary01==1.0f); assert(a.externalWorldRecall01==1.0f); dnxr::NeuralChipHoldCommitPolicy p; auto f=p.makeMinimalHardHoldFrame(1,1); assert(f.realityAnchor.orientation01==1.0f); assert(f.motorIntent.stopIntent01==1.0f); }
