#pragma once

#include "VirtualWorldState.h"

#include <string>

namespace dnxr {

class GameDesignBridge {
public:
    /// Integrates default game-design constraints to start virtual game conception quickly.
    void integrateBaseGameplaySystems(VirtualWorldState& world) const;

    /// Applies HP logic and translucent-avatar transition when HP reaches zero.
    void applyHealthState(VirtualWorldState& world, float hp, float hpMax) const;

    /// Handles the default respawn flow from visual menu to configured spawn points.
    void processRespawn(VirtualWorldState& world, bool respawnSelected, std::size_t spawnIndex) const;

private:
    static float clamp01(float value);
};

} // namespace dnxr
