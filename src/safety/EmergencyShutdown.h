#pragma once

namespace dnxr {

class EmergencyShutdown {
public:
    /// Immediately halts immersive operations during an unsafe condition.
    void trigger() const;
};

} // namespace dnxr
