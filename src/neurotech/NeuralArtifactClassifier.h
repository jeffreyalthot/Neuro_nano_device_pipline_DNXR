#pragma once
#include "../full_dive/FullDiveNeuralFrame.h"
#include <string>
#include <vector>
namespace dnxr {
struct NeuralArtifactReport {
    float muscle=0,eyeBlink=0,jawClench=0,electrodeNoise=0,signalDropout=0,motion=0,electricalInterference=0,saturation=0;
    float electrodePop=0,lineNoise=0,flatline=0,packetDropout=0,highFrequencyBurst=0,lowFrequencyDrift=0,crossChannelCorrelationFailure=0,phaseDiscontinuity=0,signalClipping=0;
    float artifactProbability=0;
    std::string dominant="none";
    std::string artifactType="none";
    std::vector<std::size_t> affectedChannels;
    std::vector<FullDiveModality> affectedModalities;
    std::string recommendedAction="allow";
    float confidence=1.0f;
    int repeatedArtifactCount=0;
};
class NeuralArtifactClassifier { public: NeuralArtifactReport classify(const std::vector<float>& signal) const; };
}
