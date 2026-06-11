#include "neural_chip/NeuralChipHoldCommitPolicy.h"
#include "neural_chip/NeuralVectorValidator.h"
#include <cassert>

static void assertHoldSafe(const dnxr::FullDiveChipFrame& f) {
    assert(dnxr::NeuralVectorValidator::validate(f).accepted);
    assert(f.olfactory.odorPresence01 == 0.0f);
    assert(f.gustatory.sweet01 == 0.0f);
    assert(f.motorIntent.stopIntent01 == 1.0f);
    assert(f.touch.painBlocked01 == 1.0f && f.touch.burnBlocked01 == 1.0f && f.touch.crushBlocked01 == 1.0f && f.touch.shockBlocked01 == 1.0f);
    assert(f.safety.simulationOnly);
    assert(f.safety.rawSensoryPayloadBlocked);
}

int main(){
    dnxr::NeuralChipHoldCommitPolicy policy; dnxr::SafeChipFrameHoldBuffer buffer; dnxr::FullDiveChipFrame accepted; accepted.frameId=1; accepted.timestampMs=1; accepted.visual.globalLuminance01=0.5f; buffer.rememberAccepted(accepted);
    dnxr::FullDiveChipFrame rejected; rejected.frameId=2; rejected.timestampMs=2;
    assertHoldSafe(policy.makeDecompressionHoldFrame(rejected, buffer, 0.25f));
    assertHoldSafe(policy.makeNeutralHoldFrame(rejected, buffer));
    assertHoldSafe(policy.makeMinimalHardHoldFrame(2, 2));
}
