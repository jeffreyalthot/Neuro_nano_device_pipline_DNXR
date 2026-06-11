#include "neural_chip/NeuralChipReplayPlayer.h"
#include <cassert>
#include <fstream>
int main(){ { std::ofstream o("/tmp/dnxr_player.jsonl"); o << "{\"frameId\":1,\"timestampMs\":1,\"commitAccepted\":true,\"safetyHold\":false,\"decisionTraceCount\":1}\n"; } auto r=dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_player.jsonl"); assert(r.valid); assert(r.framesRead==1); { std::ofstream o("/tmp/dnxr_player_bad.jsonl"); o << "{\"frameId\":1,\"timestampMs\":1,\"commitAccepted\":true,\"safetyHold\":false,\"decisionTraceCount\":1,\"payload\":1}\n"; } assert(!dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_player_bad.jsonl").valid); }
