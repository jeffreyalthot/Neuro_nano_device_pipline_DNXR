#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include <algorithm>
namespace dnxr { namespace { float b(float v){return std::max(0.0f,std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01,v));} }
NeuralTouchVector NeuralVirtualEpidermisEncoder::encode(const AvatarContactFrame& contacts,const PatientSomatosensoryMap& map,const TouchSafetyState& safety) const { NeuralTouchVector out{}; if(!safety.allowTouchStimulus) return out; out.contactPressure01=b(contacts.pressure01*map.contactGain01); out.contactArea01=b(contacts.area01); out.vibrationFlutter01=b(contacts.flutter01); out.vibrationTexture01=b(contacts.texture01); out.slip01=b(contacts.slip01); out.hardness01=b(contacts.hardness01); out.softness01=b(contacts.softness01); out.warmth01=b(contacts.warmth01); out.cold01=b(contacts.cold01); out.rightHandContact01=b(contacts.rightHand01); out.leftHandContact01=b(contacts.leftHand01); out.footGroundContact01=b(contacts.footGround01); out.bodyCollision01=b(contacts.bodyCollision01); out.painBlocked01=1.0f; out.burnBlocked01=1.0f; out.crushBlocked01=1.0f; out.shockBlocked01=1.0f; out.somatosensoryConfidence01=(contacts.requestsPain||contacts.requestsBurn||contacts.requestsCrush||contacts.requestsShock)?0.4f:0.9f; return out; }
}

namespace dnxr {
namespace {
float hzLow(float hz) { return std::max(0.0f, std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01, hz / 80.0f)); }
float hzHigh(float hz) { return std::max(0.0f, std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01, (hz - 80.0f) / 240.0f)); }
}

NeuralTouchVector NeuralVirtualEpidermisEncoder::encode(const std::vector<VirtualContactPatch>& patches,
                                                        const PatientSomatosensoryMap& map,
                                                        const TouchSafetyState& safety) const {
    AvatarContactFrame contacts;
    if (!safety.allowTouchStimulus) {
        return encode(contacts, map, safety);
    }

    float slipSum = 0.0f;
    float hardnessMax = 0.0f;
    float flutterMax = 0.0f;
    float textureMax = 0.0f;
    for (const auto& patch : patches) {
        contacts.pressure01 = std::max(contacts.pressure01, patch.pressure01);
        contacts.area01 += patch.area01;
        slipSum += patch.slip01;
        hardnessMax = std::max(hardnessMax, patch.hardness01);
        flutterMax = std::max(flutterMax, hzLow(patch.vibrationHz));
        textureMax = std::max(textureMax, hzHigh(patch.vibrationHz));
        contacts.warmth01 = std::max(contacts.warmth01, patch.temperature01 > 0.5f ? patch.temperature01 : 0.0f);
        contacts.cold01 = std::max(contacts.cold01, patch.temperature01 < 0.5f ? 1.0f - patch.temperature01 : 0.0f);
        if (patch.region == BodyRegion::RightHand) contacts.rightHand01 = std::max(contacts.rightHand01, patch.pressure01);
        if (patch.region == BodyRegion::LeftHand) contacts.leftHand01 = std::max(contacts.leftHand01, patch.pressure01);
        if (patch.region == BodyRegion::Feet) contacts.footGround01 = std::max(contacts.footGround01, patch.pressure01);
        if (patch.region == BodyRegion::Torso || patch.region == BodyRegion::Head) contacts.bodyCollision01 = std::max(contacts.bodyCollision01, patch.pressure01);
    }
    contacts.area01 = std::min(1.0f, contacts.area01);
    contacts.slip01 = patches.empty() ? 0.0f : slipSum / static_cast<float>(patches.size());
    contacts.hardness01 = hardnessMax;
    contacts.softness01 = 1.0f - hardnessMax;
    contacts.flutter01 = flutterMax;
    contacts.texture01 = textureMax;
    return encode(contacts, map, safety);
}
}
