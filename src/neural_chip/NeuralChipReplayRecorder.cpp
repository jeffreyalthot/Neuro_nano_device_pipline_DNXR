#include "neural_chip/NeuralChipReplayRecorder.h"

namespace dnxr {

void NeuralChipReplayRecorder::record(const NeuralChipReplayRecord& record) { records_.push_back(record); }
const std::vector<NeuralChipReplayRecord>& NeuralChipReplayRecorder::records() const { return records_; }
void NeuralChipReplayRecorder::clear() { records_.clear(); }

} // namespace dnxr
