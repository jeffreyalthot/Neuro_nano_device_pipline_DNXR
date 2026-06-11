#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include <cassert>
#include <vector>

int main() {
    std::vector<dnxr::VirtualContactPatch> patches = {
        {dnxr::BodyRegion::RightHand, 0.7f, 0.2f, 0.1f, 40.0f, 0.8f, 0.7f},
        {dnxr::BodyRegion::Feet, 0.5f, 0.3f, 0.2f, 180.0f, 0.4f, 0.3f},
    };
    const auto touch = dnxr::NeuralVirtualEpidermisEncoder{}.encode(patches, {}, {});
    assert(touch.contactPressure01 == 0.7f);
    assert(touch.contactArea01 > 0.49f);
    assert(touch.rightHandContact01 == 0.7f);
    assert(touch.footGroundContact01 == 0.5f);
    assert(touch.vibrationFlutter01 > 0.0f);
    assert(touch.vibrationTexture01 > 0.0f);
    assert(touch.painBlocked01 == 1.0f);
    assert(touch.burnBlocked01 == 1.0f);
    assert(touch.crushBlocked01 == 1.0f);
    assert(touch.shockBlocked01 == 1.0f);
    return 0;
}
