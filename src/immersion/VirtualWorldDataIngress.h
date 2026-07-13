#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "immersion/ClinicalNeutralSceneController.h"
#include "immersion/SceneCapabilityContract.h"
#include "immersion/SceneRiskAnnotator.h"
#include "immersion/VirtualBodySafetyContract.h"
#include "immersion/VirtualWorldState.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace dnxr {

struct VirtualWorldAudioIngress {
  float volume01 = 0.0f;
  float lowFrequency01 = 0.0f;
  float midFrequency01 = 0.0f;
  float highFrequency01 = 0.0f;
  float attack01 = 0.0f;
  float sustain01 = 0.0f;
  float directionLeftRight01 = 0.5f;
  float directionFrontBack01 = 0.5f;
  float sourceDistance01 = 0.0f;
  float speechPresence01 = 0.0f;
  float dangerPriority01 = 0.0f;
};

struct VirtualWorldTouchIngress {
  float pressure01 = 0.0f;
  float contactArea01 = 0.0f;
  float vibration01 = 0.0f;
  float texture01 = 0.0f;
  float slip01 = 0.0f;
  float hardness01 = 0.0f;
  float softness01 = 0.0f;
  float warmth01 = 0.0f;
  float cold01 = 0.0f;
  float rightHandContact01 = 0.0f;
  float leftHandContact01 = 0.0f;
  float footGroundContact01 = 0.0f;
  float bodyCollision01 = 0.0f;
};

struct VirtualWorldBodyIngress {
  float headPose01 = 0.0f;
  float torsoPose01 = 0.0f;
  float rightArmPose01 = 0.0f;
  float leftArmPose01 = 0.0f;
  float rightLegPose01 = 0.0f;
  float leftLegPose01 = 0.0f;
  float handConfidence01 = 0.0f;
  float footConfidence01 = 0.0f;
  float avatarOwnership01 = 0.0f;
  float bodyMismatch01 = 0.0f;
};

struct VirtualWorldVestibularIngress {
  float linearAcceleration01 = 0.0f;
  float angularVelocity01 = 0.0f;
  float gravityDirection01 = 0.5f;
  float balanceError01 = 0.0f;
  float motionSicknessRisk01 = 0.0f;
  float visualVestibularMismatch01 = 0.0f;
};

struct VirtualWorldChemicalIngress {
  float odorPresence01 = 0.0f;
  float odorIdentity01 = 0.0f;
  float odorPleasantness01 = 0.0f;
  float freshness01 = 0.0f;
  float smokeWarning01 = 0.0f;
  float chemicalWarning01 = 0.0f;
  float decay01 = 0.0f;
  float sweet01 = 0.0f;
  float salty01 = 0.0f;
  float sour01 = 0.0f;
  float bitter01 = 0.0f;
  float umami01 = 0.0f;
  float mouthfeel01 = 0.0f;
};

struct VirtualWorldMotorIngress {
  float moveForward01 = 0.0f;
  float turnYaw01 = 0.0f;
  float postureBalance01 = 0.0f;
  float stopIntent01 = 1.0f;
  float confidence01 = 0.0f;
};

enum class VirtualWorldIngressTransport { LocalIpc, Network };

enum class VirtualWorldUnitSystem { Normalized01, MetersSeconds, Unspecified };

struct VirtualWorldAssetReference {
  std::string assetId;
  std::string sha256Digest;
  std::uint64_t byteSize = 0;
  bool resolved = false;
};

struct VirtualWorldSceneSemantics {
  std::string semanticSceneId;
  std::string coordinateFrame = "world_right_handed_y_up";
  VirtualWorldUnitSystem unitSystem = VirtualWorldUnitSystem::Normalized01;
  float contentSafetyRisk01 = 0.0f;
};

struct VirtualWorldEnvironmentIngress {
  float ambientLight01 = 0.0f;
  float weatherIntensity01 = 0.0f;
  float navigationRisk01 = 0.0f;
};

struct VirtualWorldEventIngress {
  std::string eventType;
  float priority01 = 0.0f;
};

struct VirtualWorldIngressFrame {
  std::uint64_t frameId = 0;
  double timestampMs = 0.0;
  VirtualViewFrame view;
  VisualFieldFrame visualField;
  VirtualWorldAudioIngress audio;
  VirtualWorldTouchIngress touch;
  VirtualWorldBodyIngress body;
  VirtualWorldVestibularIngress vestibular;
  VirtualWorldChemicalIngress chemical;
  VirtualWorldMotorIngress motor;
  NeuralRealityAnchorVector realityAnchor;
  VirtualWorldSceneSemantics scene;
  VirtualWorldEnvironmentIngress environment;
  std::vector<VirtualWorldEventIngress> events;
  std::vector<VirtualWorldAssetReference> assets;
};

struct CanonicalVirtualWorldMetadata {
  static constexpr std::uint32_t kCurrentSchemaVersion = 1;
  std::uint32_t schemaVersion = kCurrentSchemaVersion;
  std::uint64_t sequenceId = 0;
  double engineTimestampMs = 0.0;
  std::string engineProvenance;
  std::string contextId;
  std::string authenticationTag;
  std::uint64_t declaredSizeBytes = 0;
  std::string sessionId;
  std::string sourceId;
  std::string peerCertificateDigest;
  std::string messageDigest;
  double hostReceiptTimestampMs = 0.0;
  double deadlineTimestampMs = 0.0;
  VirtualWorldIngressTransport transport = VirtualWorldIngressTransport::LocalIpc;
};

struct CanonicalVirtualWorldModalities {
  VirtualViewFrame view;
  VisualFieldFrame visualField;
  VirtualWorldAudioIngress audio;
  VirtualWorldTouchIngress touch;
  VirtualWorldBodyIngress body;
  VirtualWorldVestibularIngress vestibular;
  VirtualWorldChemicalIngress chemical;
  VirtualWorldMotorIngress motor;
  NeuralRealityAnchorVector realityAnchor;
  VirtualWorldSceneSemantics scene;
  VirtualWorldEnvironmentIngress environment;
  std::vector<VirtualWorldEventIngress> events;
  std::vector<VirtualWorldAssetReference> assets;
  bool visualPresent = true;
  bool audioPresent = true;
  bool touchPresent = true;
  bool bodyPresent = true;
  bool vestibularPresent = true;
  bool chemicalPresent = false;
  bool motorPresent = true;
};

struct CanonicalVirtualWorldFrame {
  CanonicalVirtualWorldMetadata metadata;
  CanonicalVirtualWorldModalities modalities;
  SceneCapabilityContract sceneContract;
  SceneRiskAnnotation sceneRisk;
  ClinicalSceneContentFlags clinicalFlags;
  VirtualBodySafetyContract bodyContract;
};

struct VirtualWorldIngressPolicy {
  std::uint64_t maxFrameBytes = 4096;
  double maxFutureLeadMs = 250.0;
  double maxStaleLagMs = 1000.0;
  bool requireAuthentication = false;
  bool requireMutualAuthentication = false;
  std::string expectedSessionId;
  double minFrameIntervalMs = 0.0;
  double maxDeadlineLagMs = 0.0;
  bool chemicalChannelsEnabled = false;
  bool requireClinicalNeutralScene = true;
  bool requireBodySafety = true;
};

struct VirtualWorldIngressResult {
  bool accepted = false;
  bool holdFrame = false;
  bool usedLastSafeWorld = false;
  bool duplicate = false;
  bool stale = false;
  bool future = false;
  bool gap = false;
  bool corrupted = false;
  bool schemaIncompatible = false;
  CanonicalVirtualWorldFrame snapshot;
  std::string reason = "not_evaluated";
  NeuralChipOnlyWorldFrame world{};
};

class IWorldIngress {
public:
  virtual ~IWorldIngress() = default;
  virtual VirtualWorldIngressResult readWorldFrame(const std::string& expectedSessionId,
                                                   double cycleTimestampMs) = 0;
};

class IMotorWorldEgress {
public:
  virtual ~IMotorWorldEgress() = default;
  virtual bool publishMotorIntent(std::uint64_t frameId,
                                  const std::string& sessionId,
                                  const VirtualWorldMotorIngress& intent) = 0;
};

class VirtualWorldDataIngress {
public:
  explicit VirtualWorldDataIngress(VirtualWorldIngressPolicy policy = {});

  VirtualWorldIngressResult ingestCanonical(const CanonicalVirtualWorldFrame& input,
                                            double cycleTimestampMs);
  VirtualWorldIngressResult disconnectHold(std::uint64_t frameId, double cycleTimestampMs) const;
  NeuralChipOnlyWorldFrame ingest(const VirtualWorldIngressFrame &input) const;

private:
  NeuralChipOnlyWorldFrame buildWorld(const CanonicalVirtualWorldFrame& input) const;
  NeuralChipOnlyWorldFrame holdWorld(std::uint64_t frameId, double timestampMs) const;
  VirtualWorldIngressResult rejectWithHold(std::string reason, std::uint64_t frameId, double timestampMs) const;
  bool authenticated(const CanonicalVirtualWorldFrame& input) const;
  bool schemaAndContentValid(const CanonicalVirtualWorldFrame& input, std::string& reason) const;
  std::uint64_t estimateSizeBytes(const CanonicalVirtualWorldFrame& input) const;

  VirtualWorldIngressPolicy policy_;
  std::uint64_t lastSequenceId_ = 0;
  double lastTimestampMs_ = -1.0;
  NeuralChipOnlyWorldFrame lastSafeWorld_{};
  bool hasLastSafeWorld_ = false;
};

std::string canonicalVirtualWorldDigest(const CanonicalVirtualWorldFrame& input);

CanonicalVirtualWorldFrame makeCanonicalVirtualWorldFrame(const VirtualWorldIngressFrame& input,
                                                          std::string engineProvenance = "legacy_structured_ingress");

} // namespace dnxr
