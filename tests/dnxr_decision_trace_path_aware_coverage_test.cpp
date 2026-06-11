#include "neural_chip/DecisionTraceCoverageValidator.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipPipelineDecisionTrace trace;
    trace.record({1, "WorldFrameSanitizer", "world", "sanitize", "bad", 1, 0, 0});
    trace.record({1, "NeuralChipFrameCommitter", "commit", "minimal", "hold", 1, 0, 0});
    DecisionTraceCoverageValidator v;
    assert(v.validate(trace, PipelinePathKind::WorldFrameReject).complete);
    assert(!v.validate(trace, PipelinePathKind::NormalCommit).complete);
    NeuralChipPipelineDecisionTrace ack;
    ack.record({2, "NeuralChipAckHandshake", "ack", "timeout", "missing", 1, 0, 0});
    ack.record({2, "NeuralChipFrameCommitter", "commit", "decompression", "hold", 1, 0, 0});
    assert(v.validate(ack, PipelinePathKind::AckMissingHold).complete);
    return 0;
}
