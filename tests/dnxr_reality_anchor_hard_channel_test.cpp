#include "full_dive/safety/ChipRealityAnchorHardChannel.h"
#include <cassert>

int main() {
    dnxr::ChipRealityAnchorHardChannel channel;
    dnxr::FullDiveChipFrame frame;
    assert(channel.validates(frame.realityAnchor));

    frame.visual.globalLuminance01 = 0.5f;
    frame.motorIntent.moveForward01 = 0.5f;
    frame.motorIntent.stopIntent01 = 0.0f;
    frame.realityAnchor.orientation01 = 0.2f;
    const auto held = channel.enforce(frame);
    assert(!channel.validates(held.realityAnchor));
    assert(held.visual.globalLuminance01 == 0.0f);
    assert(held.motorIntent.moveForward01 == 0.5f);
    assert(held.motorIntent.stopIntent01 == 1.0f);
    assert(held.safety.aggregateRisk01 == 1.0f);
    return 0;
}
