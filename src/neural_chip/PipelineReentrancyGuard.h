#pragma once

namespace dnxr {

class PipelineReentrancyGuard {
public:
    bool enter();
    void leave();
    bool busy() const;
private:
    bool busy_ = false;
};

} // namespace dnxr
