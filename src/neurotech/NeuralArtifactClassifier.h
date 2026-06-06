#pragma once
#include <string>
#include <vector>
namespace dnxr { struct NeuralArtifactReport { float muscle=0,eyeBlink=0,jawClench=0,electrodeNoise=0,signalDropout=0,motion=0,electricalInterference=0,saturation=0; float artifactProbability=0; std::string dominant="none"; }; class NeuralArtifactClassifier { public: NeuralArtifactReport classify(const std::vector<float>& signal) const; }; }
