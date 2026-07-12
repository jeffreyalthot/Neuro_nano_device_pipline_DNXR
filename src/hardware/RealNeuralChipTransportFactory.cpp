#include "RealNeuralChipTransportFactory.h"
namespace dnxr {
std::unique_ptr<INeuralChipTransport> RealNeuralChipTransportFactory::create(const RealTransportUnlockContext& c){
#if DNXR_SIMULATION_ONLY
    if(c.evidence.deploymentMode==RuntimeDeploymentMode::SimulationOnly) return std::make_unique<SimulatedNeuralChipTransport>();
#else
    if(c.evidence.deploymentMode==RuntimeDeploymentMode::SimulationOnly) return std::make_unique<ClinicalLockedNeuralChipTransport>();
#endif
    if(c.valid() && (c.evidence.deploymentMode==RuntimeDeploymentMode::HardwareBenchNoHuman || c.evidence.deploymentMode==RuntimeDeploymentMode::PreclinicalPhantom || c.evidence.deploymentMode==RuntimeDeploymentMode::HumanClinicalTrialAuthorized)) return std::make_unique<RealNeuralChipTransport>();
    return std::make_unique<ClinicalLockedNeuralChipTransport>();
}
}
