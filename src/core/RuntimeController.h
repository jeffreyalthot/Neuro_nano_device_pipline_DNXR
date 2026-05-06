#pragma once

namespace dnxr {

class RuntimeController {
public:
    /// Runs one end-to-end pipeline iteration from sensing to stimulation.
    void runSingleCycle();
};

} // namespace dnxr
