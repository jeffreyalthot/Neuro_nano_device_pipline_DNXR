#include "runtime/CanonicalNeuralBciPipeline.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string readFile(const char* path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream out;
    out << input.rdbuf();
    return out.str();
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}
} // namespace

int main() {
    assert(dnxr::CanonicalNeuralBciPipeline::stageRegistryIsValid());

    std::vector<dnxr::CanonicalBciStage> prefix{
        dnxr::CanonicalBciStage::BootAndAttestation,
        dnxr::CanonicalBciStage::AcquireWorld,
        dnxr::CanonicalBciStage::SanitizeWorld,
        dnxr::CanonicalBciStage::AcquireBci};
    assert(dnxr::CanonicalNeuralBciPipeline::stageCoverageAccepts(prefix));

    auto duplicate = prefix;
    duplicate.push_back(dnxr::CanonicalBciStage::AcquireBci);
    assert(!dnxr::CanonicalNeuralBciPipeline::stageCoverageAccepts(duplicate));

    std::vector<dnxr::CanonicalBciStage> skipped{
        dnxr::CanonicalBciStage::BootAndAttestation,
        dnxr::CanonicalBciStage::SanitizeWorld};
    assert(!dnxr::CanonicalNeuralBciPipeline::stageCoverageAccepts(skipped));

    for (const auto& descriptor : dnxr::kCanonicalBciStageRegistry) {
        assert(!contains(descriptor.owner, "Render"));
        assert(!contains(descriptor.owner, "XRRenderer"));
        assert(descriptor.budgetNs > 0U);
    }

    const std::string impl = readFile("src/runtime/CanonicalNeuralBciPipeline.cpp");
    const std::string header = readFile("src/runtime/CanonicalNeuralBciPipeline.h");
    const std::string sources = readFile("cmake/dnxr_sources.cmake");
    assert(!contains(header, "RuntimePipelineRunner.h"));
    assert(!contains(header, "compatibilityAdapter_"));
    assert(!contains(impl, "compatibilityAdapter_"));
    assert(!contains(impl, "RuntimePipelineRunner compatibility"));
    assert(!contains(impl, "adapterMs /"));
    assert(!contains(impl, "RuntimeRenderStage::execute"));
    assert(contains(impl, "RuntimeAcquisitionStage::execute"));
    assert(contains(impl, "RuntimeSafetyStage::execute"));
    assert(contains(impl, "certified_transport_not_bound_prepared_commit_held"));
    assert(!contains(sources, "src/runtime/RuntimePipelineRunner.cpp"));

    return 0;
}
