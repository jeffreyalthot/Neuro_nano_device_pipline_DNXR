#pragma once

namespace dnxr {

class SystemKernel {
public:
    /// Boots the full runtime stack and executes a protected control cycle.
    void boot();
};

} // namespace dnxr
