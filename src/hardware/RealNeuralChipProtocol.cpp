#include "RealNeuralChipProtocol.h"
#include "RealNeuralChipPacket.h"
#include "../security/ClinicalPacketSigner.h"
#include "../security/SessionNonceManager.h"
#include "../utils/TimeSync.h"

#include <functional>
#include <sstream>

namespace dnxr {
namespace {
std::string hashString(const std::string& value) { return std::to_string(std::hash<std::string>{}(value)); }
std::string joinPayload(const std::vector<float>& payload) { std::ostringstream oss; for (float v : payload) oss << v << ';'; return oss.str(); }
}

RealNeuralChipPacket RealNeuralChipProtocol::buildPacket(const SafeNeuralStimulusCommand& c,const std::string& sid,const std::string& prev) const {
    static SessionNonceManager nonceManager;
    RealNeuralChipPacket p;
    p.protocolVersion=1;
    p.packetId=c.sequenceId;
    p.sequenceId=c.sequenceId;
    p.sessionId=sid;
    p.monotonicTimestamp=c.monotonicTimestampMs>0?c.monotonicTimestampMs:TimeSync::nowMs();
    p.antiReplayNonce = nonceManager.nextNonce();
    p.patientCalibrationHash=c.calibrationProfileId;
    p.commandType="safe_full_dive_stimulus";
    p.payload=c.boundedChannels;
    p.channelCount=static_cast<std::uint32_t>(p.payload.size());
    p.payloadLength=static_cast<std::uint32_t>(p.payload.size()*sizeof(float));
    p.previousCommandHash=prev;
    p.previousPacketHash=prev;
    p.safetyMode=c.safetyMode;
    p.emergencyCutoff=!c.allowStimulusDispatch;
    p.payloadHash=hashString(joinPayload(p.payload));
    p.commandHash=hashString(p.commandType + p.payloadHash + p.patientCalibrationHash);
    p.headerHash=hashString(std::to_string(p.protocolVersion)+sid+std::to_string(p.sequenceId)+std::to_string(p.monotonicTimestamp)+std::to_string(p.antiReplayNonce)+p.previousPacketHash);
    p.packetHash=hashString(p.headerHash + p.commandHash + p.safetyMode + (p.emergencyCutoff ? "cutoff" : "allow"));
    ClinicalPacketSigner signer;
    p.signatureAlgorithm = signer.algorithm();
    p.signature = signer.sign(p);

    p.header.protocolVersion=p.protocolVersion; p.header.packetId=p.packetId; p.header.sessionId=p.sessionId; p.header.sequenceId=p.sequenceId; p.header.monotonicTimestamp=p.monotonicTimestamp; p.header.antiReplayNonce=p.antiReplayNonce; p.header.previousPacketHash=p.previousPacketHash; p.header.headerHash=p.headerHash;
    p.body.commandType=p.commandType; p.body.channels=p.payload; p.body.channelCount=p.channelCount; p.body.payloadLength=p.payloadLength; p.body.payloadHash=p.payloadHash; p.body.commandHash=p.commandHash;
    p.safety.patientCalibrationHash=p.patientCalibrationHash; p.safety.deviceCertificateHash=p.deviceCertificateHash; p.safety.firmwareHash=p.firmwareHash; p.safety.safetyMode=p.safetyMode; p.safety.emergencyCutoff=p.emergencyCutoff;
    p.signatureBlock.signatureAlgorithm=p.signatureAlgorithm; p.signatureBlock.signature=p.signature; p.signatureBlock.packetHash=p.packetHash;
    return p;
}

bool RealNeuralChipProtocol::validateLockedClinicalPrerequisites(bool gate,bool att,bool evidence,bool stop) const { return gate&&att&&evidence&&stop; }

} // namespace dnxr
