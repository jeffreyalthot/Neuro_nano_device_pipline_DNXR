#include "NeuralMeshProtocol.h"
#include "../utils/Logger.h"

#include <chrono>
#include <cmath>
#include <numeric>
#include <random>

namespace dnxr {

NeuralMeshProtocol::NeuralMeshProtocol()
    : compressionEnabled_(true),
      integrityCheck_(true),
      packetCounter_(0),
      simulationMode_(true) {}

uint64_t NeuralMeshProtocol::timestamp() const {
    using clock = std::chrono::high_resolution_clock;
    return static_cast<uint64_t>(
        clock::now().time_since_epoch().count());
}

float NeuralMeshProtocol::jitter(float amp) const {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(-amp, amp);
    return dist(gen);
}

void NeuralMeshProtocol::sanitize(std::vector<float>& data) const {
    for (float& v : data)
        if (!std::isfinite(v)) v = 0.0f;
}

std::vector<float> NeuralMeshProtocol::compressPayload(const std::vector<float>& in) const {
    if (in.empty()) return {};
    std::vector<float> out;
    out.reserve(in.size());
    for (float v : in)
        out.push_back(v + jitter(0.0008f));
    return out;
}

uint32_t NeuralMeshProtocol::computeCRC(const std::vector<float>& data) const {
    // CRC simple basé sur une somme modulée (suffisant pour détection erreurs logicielles)
    uint32_t crc = 0;
    for (float v : data) {
        auto bits = static_cast<uint32_t>(std::fabs(v) * 100000.0f);
        crc ^= bits + 0x9e3779b9 + (crc << 6) + (crc >> 2);
    }
    return crc;
}

MeshPacket NeuralMeshProtocol::encode(
    const std::string& src,
    const std::string& dst,
    const std::vector<float>& signal) const
{
    std::lock_guard<std::mutex> lock(packetMutex_);

    MeshPacket packet;
    packet.source = src;
    packet.destination = dst;
    packet.timestamp = timestamp();
    packet.packetId = ++packetCounter_;

    std::vector<float> payload = signal;
    sanitize(payload);

    if (compressionEnabled_)
        payload = compressPayload(payload);

    packet.payload = std::move(payload);
    packet.crc = computeCRC(packet.payload);

    if (simulationMode_)
        Logger::info("[NeuralMeshProtocol] Encoded packet #" + std::to_string(packet.packetId));

    if (onPacketEvent_)
        onPacketEvent_(packet, true);

    return packet;
}

std::vector<float> NeuralMeshProtocol::decode(const MeshPacket& packet) const {
    std::lock_guard<std::mutex> lock(packetMutex_);
    std::vector<float> out = packet.payload;
    sanitize(out);

    bool valid = validate(packet);
    if (onPacketEvent_)
        onPacketEvent_(packet, valid);

    if (simulationMode_)
        Logger::info("[NeuralMeshProtocol] Decoded packet #" + std::to_string(packet.packetId) +
                     (valid ? " [OK]" : " [ERROR]"));

    return out;
}

bool NeuralMeshProtocol::validate(const MeshPacket& packet) const {
    if (packet.source.empty() || packet.destination.empty() || packet.payload.empty())
        return false;

    if (!integrityCheck_) return true;
    uint32_t crcCheck = computeCRC(packet.payload);
    return crcCheck == packet.crc;
}

uint64_t NeuralMeshProtocol::getPacketCount() const { return packetCounter_; }

void NeuralMeshProtocol::enableCompression(bool enabled) {
    compressionEnabled_ = enabled;
}
bool NeuralMeshProtocol::isCompressionEnabled() const { return compressionEnabled_; }

void NeuralMeshProtocol::enableSimulationMode(bool enabled) {
    simulationMode_ = enabled;
    Logger::info(std::string("[NeuralMeshProtocol] Simulation mode ") + (enabled ? "ON" : "OFF"));
}
bool NeuralMeshProtocol::simulationModeEnabled() const noexcept { return simulationMode_; }

void NeuralMeshProtocol::setOnPacketEvent(
    const std::function<void(const MeshPacket&, bool valid)>& cb)
{
    onPacketEvent_ = cb;
}

} // namespace dnxr
