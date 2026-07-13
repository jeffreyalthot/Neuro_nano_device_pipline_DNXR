#include "runtime/CanonicalNeuralBciPipeline.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    std::ostringstream out;
    out << input.rdbuf();
    return out.str();
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}
}

int main() {
    std::vector<dnxr::CanonicalBciStage> canonicalOrder(
        dnxr::kCanonicalBciStageOrder.begin(),
        dnxr::kCanonicalBciStageOrder.end());
    assert(dnxr::CanonicalNeuralBciPipeline::stageOrderIsCanonical(canonicalOrder));

    auto reordered = canonicalOrder;
    std::swap(reordered[16], reordered[17]);
    assert(!dnxr::CanonicalNeuralBciPipeline::stageOrderIsCanonical(reordered));

    dnxr::CanonicalBciCycleTransaction tx;
    tx.world.stamp.frameId = 42;
    tx.world.stamp.schemaVersion = dnxr::CanonicalNeuralBciPipeline::kSchemaVersion;
    tx.world.stamp.monotonicTimestampNs = 1000;
    tx.world.stamp.provenance = dnxr::CanonicalBciProvenanceKind::VirtualWorldEngine;
    tx.world.stamp.validity = dnxr::CanonicalBciSchemaValidity::Valid;
    tx.command.stamp.frameId = 42;
    tx.command.patientBindingVerified = true;
    tx.command.capabilitiesVerified = true;
    tx.command.safetyEnvelopeVerified = true;
    tx.command.lifecyclePolicyVerified = true;
    tx.command.ackPolicyVerified = true;
    tx.command.feedbackCapabilityVerified = true;
    tx.command.chemicalGateVerified = true;
    tx.feedback.autoAck = false;
    assert(dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));

    tx.feedback.autoAck = true;
    assert(!dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));
    tx.feedback.autoAck = false;
    tx.world.stamp.validity = dnxr::CanonicalBciSchemaValidity::Invalid;
    assert(!dnxr::CanonicalNeuralBciPipeline::dispatchPreconditionsSatisfied(tx));

    const std::string header = readFile("src/runtime/CanonicalNeuralBciPipeline.h");
    assert(!contains(header, "XRHeadsetInterface"));
    assert(!contains(header, "XRRenderer"));
    assert(!contains(header, "OpenXR"));
    assert(!contains(header, "SteamVR"));
    assert(!contains(header, std::string("Secondary") + "Simulation"));
    assert(!contains(header, std::string("Primary") + "Real"));

    const std::string cmake = readFile("CMakeLists.txt");
    assert(contains(cmake, "add_executable(distributed_neural_xr"));
    assert(!contains(cmake, "add_executable(dnxr_production"));

    return 0;
}
