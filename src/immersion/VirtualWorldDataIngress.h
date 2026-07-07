#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "immersion/VirtualWorldState.h"

#include <cstdint>

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
};

class VirtualWorldDataIngress {
public:
  NeuralChipOnlyWorldFrame ingest(const VirtualWorldIngressFrame &input) const;
};

} // namespace dnxr
