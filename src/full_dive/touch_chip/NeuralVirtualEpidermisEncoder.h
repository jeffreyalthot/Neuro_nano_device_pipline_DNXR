#pragma once
#include "neural_chip/NeuralChipFrame.h"
#include <vector>
namespace dnxr {
enum class BodyRegion { RightHand, LeftHand, Feet, Torso, Head, Other };
struct VirtualContactPatch { BodyRegion region=BodyRegion::Other; float pressure01=0.0f; float area01=0.0f; float slip01=0.0f; float vibrationHz=0.0f; float hardness01=0.0f; float temperature01=0.5f; };
struct AvatarContactFrame { float pressure01=0.0f, area01=0.0f, flutter01=0.0f, texture01=0.0f, slip01=0.0f, hardness01=0.0f, softness01=0.0f, warmth01=0.0f, cold01=0.0f, rightHand01=0.0f, leftHand01=0.0f, footGround01=0.0f, bodyCollision01=0.0f; bool requestsPain=false, requestsBurn=false, requestsCrush=false, requestsShock=false; };
struct PatientSomatosensoryMap { float contactGain01=1.0f; };
struct TouchSafetyState { bool allowTouchStimulus=true; };
class NeuralVirtualEpidermisEncoder { public: NeuralTouchVector encode(const AvatarContactFrame& contacts,const PatientSomatosensoryMap& map,const TouchSafetyState& safety) const; NeuralTouchVector encode(const std::vector<VirtualContactPatch>& patches,const PatientSomatosensoryMap& map,const TouchSafetyState& safety) const; };
}
