#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"

#include <cassert>
#include <cstring>

using namespace dnxr;

namespace {
StimulusBySpikeInput baseInput() {
  StimulusBySpikeInput input;
  input.frameId = 42;
  input.patientMap = PatientSpecificNeuralMap::simulationDefaults();
  input.lastAck.packetAccepted = true;
  input.readFrame.signalQuality01 = 1.0f;
  input.safety.simulationOnly = false;
  input.safety.patientStopSignalAlive = true;
  input.safety.emergencyCutoffAvailable = true;
  input.safety.painMappingBlocked = true;
  input.safety.rawSensoryPayloadBlocked = true;
  input.deploymentSafety.deploymentMode = RuntimeDeploymentMode::HardwareBenchNoHuman;
  input.deploymentSafety.humanUseLocked = true;
  return input;
}
}

int main() {
  auto missingEvidence = baseInput();
  const auto rejected = StimulusBySpikeRuntime{}.process(missingEvidence);
  assert(!rejected.accepted);
  assert(rejected.holdRequired);
  assert(!rejected.safetyPreflightAccepted);
  assert(std::strcmp(rejected.reason,
                     "stimulus_by_spike_safety_preflight_hold") == 0);

  auto attested = baseInput();
  attested.deploymentSafety.hardwareIdentityVerified = true;
  attested.deploymentSafety.firmwareVerified = true;
  attested.deploymentSafety.patientMapVerified = true;
  attested.deploymentSafety.calibrationVerified = true;
  attested.deploymentSafety.feedbackChannelVerified = true;
  attested.deploymentSafety.hardwareCutoffVerified = true;
  attested.deploymentSafety.operatorAuthorizationVerified = true;
  attested.deploymentSafety.sessionConsentVerified = true;
  const auto allowedToProceed = StimulusBySpikeRuntime{}.process(attested);
  assert(allowedToProceed.safetyPreflightAccepted);
  assert(allowedToProceed.executedStageCount == DNXR_STIMULUS_BY_SPIKE_STAGE_COUNT);

  auto humanMode = attested;
  humanMode.deploymentSafety.deploymentMode = RuntimeDeploymentMode::HumanClinicalTrialAuthorized;
  const auto humanRejected = StimulusBySpikeRuntime{}.process(humanMode);
  assert(!humanRejected.safetyPreflightAccepted);
  assert(humanRejected.holdRequired);
}
