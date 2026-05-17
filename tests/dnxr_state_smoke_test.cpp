#include "../src/immersion/VirtualWorldState.h"
#include "../src/immersion/GameDesignBridge.h"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    dnxr::VirtualWorldState world;
    dnxr::GameDesignBridge bridge;

    bridge.integrateBaseGameplaySystems(world);
    for (int i = 0; i < 300; ++i) {
        world.update(0.016);
    }
    assert(world.getSceneName() == "AvatarSelectionLounge");
    assert(std::fabs(world.getIntensity() - 0.45F) < 2e-2F);

    bridge.applyHealthState(world, 0.0F, 100.0F);
    assert(world.getSceneName() == "RecoveryChamber");

    bridge.processRespawn(world, true, 1U);
    for (int i = 0; i < 300; ++i) {
        world.update(0.016);
    }
    assert(world.getSceneName() == "AtriumSouth");
    assert(std::fabs(world.getIntensity() - 1.0F) < 2e-2F);

    bridge.processRespawn(world, true, 999U);
    assert(world.getSceneName() == "HubCenter");

    std::cout << "dnxr_state_smoke_test: OK\n";
    return 0;
}
