#include "../src/runtime/CanonicalNeuralBciPipeline.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string readFile(const char* path) {
    std::ifstream in(path);
    assert(in.good());
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}
} // namespace

int main() {
    dnxr::CanonicalBciCycleTransaction tx;
    tx.world.stamp.validity = dnxr::CanonicalBciSchemaValidity::Valid;
    tx.command.patientBindingVerified = true;
    tx.command.capabilitiesVerified = true;
    tx.command.safetyEnvelopeVerified = true;
    tx.command.lifecyclePolicyVerified = true;
    tx.command.ackPolicyVerified = true;
    tx.command.feedbackCapabilityVerified = true;
    tx.feedback.autoAck = false;
    assert(dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));

    tx.feedback.autoAck = true;
    assert(!dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));
    tx.feedback.autoAck = false;
    tx.world.stamp.validity = dnxr::CanonicalBciSchemaValidity::Unchecked;
    assert(!dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));

    std::vector<dnxr::CanonicalBciStage> order(dnxr::kCanonicalBciStageOrder.begin(),
                                               dnxr::kCanonicalBciStageOrder.end());
    assert(dnxr::CanonicalNeuralBciPipeline::stageOrderIsCanonical(order));
    std::swap(order[16], order[17]);
    assert(!dnxr::CanonicalNeuralBciPipeline::stageOrderIsCanonical(order));

    const std::string canonicalHeader = readFile("src/runtime/CanonicalNeuralBciPipeline.h");
    const std::string canonicalImpl = readFile("src/runtime/CanonicalNeuralBciPipeline.cpp");
    const std::string runtimeApp = readFile("src/app/RuntimeApplication.cpp");

    assert(!contains(canonicalHeader, "XRHeadsetInterface"));
    assert(!contains(canonicalHeader, "OpenXR"));
    assert(!contains(canonicalHeader, "SteamVR"));
    assert(!contains(canonicalHeader, "HMD"));
    assert(!contains(canonicalImpl, "runLegacy"));
    assert(!contains(canonicalImpl, "LegacyRunInput"));
    assert(!contains(runtimeApp, "PipelineOrchestrator"));

    return 0;
}
