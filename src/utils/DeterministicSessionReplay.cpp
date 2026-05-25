#include "DeterministicSessionReplay.h"

namespace dnxr {

void DeterministicSessionReplay::record(const ReplayFrame& frame) { frames_.push_back(frame); }
void DeterministicSessionReplay::resetCursor() { cursor_ = 0; }
bool DeterministicSessionReplay::hasNext() const { return cursor_ < frames_.size(); }
ReplayFrame DeterministicSessionReplay::next() { return hasNext() ? frames_[cursor_++] : ReplayFrame{}; }
std::size_t DeterministicSessionReplay::size() const { return frames_.size(); }

} // namespace dnxr
