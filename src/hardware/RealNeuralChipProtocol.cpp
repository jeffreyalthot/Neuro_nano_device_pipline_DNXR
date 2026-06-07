#include "RealNeuralChipProtocol.h"
#include "RealNeuralChipPacket.h"
#include "../security/ClinicalPacketSigner.h"
#include "../security/SessionNonceManager.h"
#include "../utils/TimeSync.h"

#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {
std::string deterministicHash(const std::string& value) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char c : value) {
        hash ^= c;
        hash *= 1099511628211ULL;
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}

std::string joinPayload(const FixedVector<float, DNXR_MAX_STIMULUS_CHANNELS>& payload) {
    std::ostringstream oss;
    for (float v : payload) oss << v << ';';
    return oss.str();
}
}

RealNeuralChipPacket RealNeuralChipProtocol::buildPacket(const SafeNeuralStimulusCommandFixed& c,const std::string& sid,const std::string& prev) const {
    static SessionNonceManager nonceManager;
    RealNeuralChipPacket p;
    p.protocolVersion=1;
    p.packetId=c.sequenceId;
    p.sequenceId=c.sequenceId;
    p.sessionId=sid;
    p.monotonicTimestamp=c.monotonicTimestampMs>0?c.monotonicTimestampMs:TimeSync::nowMs();
    p.antiReplayNonce = nonceManager.nextNonce();
    p.patientCalibrationHash=c.calibrationProfileId;
    p.commandType="safe_full_dive_stimulus_fixed";
    p.payload=c.boundedChannels;
    p.modalityIntensities=c.modalityIntensities;
    p.channelCount=static_cast<std::uint32_t>(p.payload.size());
    p.payloadLength=static_cast<std::uint32_t>(p.payload.size()*sizeof(float));
    p.previousCommandHash=prev;
    p.previousPacketHash=prev;
    p.safetyMode=c.safetyMode;
    p.emergencyCutoff=!c.allowStimulusDispatch;
    for (std::size_t i = 0; i < c.modalityIntensities.size(); ++i) {
        if (c.modalityIntensities[i] > 0.0f) p.modalityMask |= (1u << i);
    }
    p.channelMapHash=deterministicHash("fixed_channel_map_v1:" + std::to_string(p.channelCount));
    p.encodingModelSetHash=deterministicHash("encoding_set_v1:" + p.safetyMode);
    p.targetMapHash=deterministicHash("target_map_v1:" + p.patientCalibrationHash);
    p.calibrationDependencyHash=deterministicHash("calibration_dependencies:" + p.patientCalibrationHash);
    p.safetyDecisionHash=deterministicHash("safety:" + p.safetyMode + ':' + std::to_string(c.riskScore));
    p.payloadHash=deterministicHash(joinPayload(p.payload));
    p.commandHash=deterministicHash(p.commandType + p.payloadHash + p.patientCalibrationHash + p.channelMapHash + p.safetyDecisionHash);
    p.headerHash=deterministicHash(std::to_string(p.protocolVersion)+sid+std::to_string(p.sequenceId)+std::to_string(p.monotonicTimestamp)+std::to_string(p.antiReplayNonce)+p.previousPacketHash);
    p.packetHash=deterministicHash(p.headerHash + p.commandHash + p.safetyMode + (p.emergencyCutoff ? "cutoff" : "allow"));
    ClinicalPacketSigner signer;
    p.signatureAlgorithm = signer.algorithm();
    p.signature = signer.sign(p);

    p.header.protocolVersion=p.protocolVersion; p.header.packetId=p.packetId; p.header.sessionId=p.sessionId; p.header.sequenceId=p.sequenceId; p.header.monotonicTimestamp=p.monotonicTimestamp; p.header.antiReplayNonce=p.antiReplayNonce; p.header.previousPacketHash=p.previousPacketHash; p.header.headerHash=p.headerHash;
    p.body.commandType=p.commandType; p.body.channels=p.payload; p.body.modalityIntensities=p.modalityIntensities; p.body.modalityMask=p.modalityMask; p.body.channelCount=p.channelCount; p.body.payloadLength=p.payloadLength; p.body.payloadHash=p.payloadHash; p.body.commandHash=p.commandHash;
    p.safety.patientCalibrationHash=p.patientCalibrationHash; p.safety.deviceCertificateHash=p.deviceCertificateHash; p.safety.firmwareHash=p.firmwareHash; p.safety.safetyMode=p.safetyMode; p.safety.emergencyCutoff=p.emergencyCutoff;
    p.signatureBlock.signatureAlgorithm=p.signatureAlgorithm; p.signatureBlock.signature=p.signature; p.signatureBlock.packetHash=p.packetHash;
    return p;
}

bool RealNeuralChipProtocol::validateLockedClinicalPrerequisites(bool gate,bool att,bool evidence,bool stop) const { return gate&&att&&evidence&&stop; }

} // namespace dnxr
