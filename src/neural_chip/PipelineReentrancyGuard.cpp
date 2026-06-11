#include "neural_chip/PipelineReentrancyGuard.h"

namespace dnxr {

bool PipelineReentrancyGuard::enter() {
    if (busy_) return false;
    busy_ = true;
    return true;
}

void PipelineReentrancyGuard::leave() { busy_ = false; }
bool PipelineReentrancyGuard::busy() const { return busy_; }

} // namespace dnxr
