#include "neural_chip/NeuralChipReplayPlayer.h"
#include "neural_chip/NeuralChipReplayRecorder.h"
#include <cassert>
#include <fstream>
int main(){
    {
        std::ofstream o("/tmp/dnxr_player.jsonl");
        o << "{\"schemaVersion\":\"" << dnxr::DNXR_REPLAY_SCHEMA_VERSION
          << "\",\"frameId\":1,\"timestampMs\":1,\"pipelineState\":\"AwaitingAck\",\"pipelinePathKind\":\"NormalCommit\",\"stepOutcome\":\"NormalAccepted\",\"noOp\":false,\"busSubmitted\":true,\"candidateAccepted\":true,\"normalFrameSubmitted\":true,\"holdFrameSubmitted\":false,\"safetyHold\":false,\"traceCoverageComplete\":true,\"traceCoverageMissingStage\":\"\",\"sessionDigestHash\":1,\"sessionDigestFrameCount\":1,\"hasCandidateFrame\":true,\"hasSubmittedFrame\":true}\n";
    }
    auto r=dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_player.jsonl"); assert(r.valid); assert(r.framesRead==1);
    {
        std::ofstream o("/tmp/dnxr_player_bad.jsonl");
        o << "{\"schemaVersion\":\"" << dnxr::DNXR_REPLAY_SCHEMA_VERSION << "\",\"frameId\":1,\"payload\":1}\n";
    }
    assert(!dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_player_bad.jsonl").valid);
}
