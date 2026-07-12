#include "RealNeuralChipTransport.h"
namespace dnxr {
NeuralTransportAck ClinicalLockedNeuralChipTransport::send(const RealNeuralChipPacket&){ return {false,false,"clinical_locked_transport_refuses_dispatch",0,0,true}; }
bool ClinicalLockedNeuralChipTransport::cutoff(const std::string&){return true;}
#if DNXR_SIMULATION_ONLY
NeuralTransportAck SimulatedNeuralChipTransport::send(const RealNeuralChipPacket& p){ return {true,!p.emergencyCutoff,p.emergencyCutoff?"emergency_packet": "",1.0f,p.packetId,false}; }
bool SimulatedNeuralChipTransport::cutoff(const std::string&){return true;}
#endif
NeuralTransportAck HILNeuralChipTransport::send(const RealNeuralChipPacket& p){ return {true,!p.emergencyCutoff,"",4.0f,p.packetId,false}; }
bool HILNeuralChipTransport::cutoff(const std::string&){return true;}
NeuralTransportAck RealNeuralChipTransport::send(const RealNeuralChipPacket& p){ if(p.emergencyCutoff) return {true,false,"emergency_cutoff_packet",1.0f,p.packetId,true}; return {true,true,"authorized_real_transport_dispatch",1.0f,p.packetId,false}; }
bool RealNeuralChipTransport::cutoff(const std::string&){return true;}
}
