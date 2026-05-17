#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <mutex>

namespace dnxr {

struct MeshPacket {
    std::string source;
    std::string destination;
    std::vector<float> payload;

    uint64_t timestamp = 0;
    uint64_t packetId = 0;
    uint32_t crc = 0;  ///< simple intégrité logicielle
};

/// Gère l'encodage, la compression et la validation des paquets neuronaux du maillage XR.
class NeuralMeshProtocol {
public:
    NeuralMeshProtocol();

    /// Encode un flux en paquet routable.
    MeshPacket encode(const std::string& src,
                      const std::string& dst,
                      const std::vector<float>& signal) const;

    /// Décode et nettoie la charge utile.
    std::vector<float> decode(const MeshPacket& packet) const;

    /// Vérifie l’intégrité d’un paquet (cohérence CRC / champs).
    bool validate(const MeshPacket& packet) const;

    /// Statistiques globales.
    uint64_t getPacketCount() const;

    /// Activation/désactivation de la compression logicielle.
    void enableCompression(bool enabled);
    bool isCompressionEnabled() const;

    /// Simulation ou exécution réelle.
    void enableSimulationMode(bool enabled);
    bool simulationModeEnabled() const noexcept;

    /// Callback pour diagnostic réseau / trafic.
    void setOnPacketEvent(const std::function<void(const MeshPacket&, bool valid)>& cb);

private:
    uint32_t computeCRC(const std::vector<float>& data) const;
    void sanitize(std::vector<float>& data) const;
    std::vector<float> compressPayload(const std::vector<float>& input) const;
    uint64_t timestamp() const;
    float jitter(float amp) const;

private:
    bool compressionEnabled_;
    bool integrityCheck_;
    mutable uint64_t packetCounter_;
    bool simulationMode_;

    std::function<void(const MeshPacket&, bool)> onPacketEvent_;
    mutable std::mutex packetMutex_;
};

} // namespace dnxr
