#include "../src/network/OpenNeuralInteropProtocol.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::OpenNeuralInteropProtocol proto;
    const std::vector<float> payload{0.1f, 0.2f, 0.3f, -0.4f};
    const std::string vendor = "vendor_xr";

    const auto packet = proto.encodeStandardPacket(payload, vendor);
    assert(proto.validateStandardPacket(packet, vendor));

    dnxr::OpenNeuralPacket decoded;
    assert(proto.decodeStandardPacket(packet, vendor, decoded));
    assert(decoded.payload.size() == payload.size());

    auto tampered = packet;
    tampered[4] += 0.25f;
    assert(!proto.validateStandardPacket(tampered, vendor));

    auto badLen = packet;
    badLen[2] = 99.0f;
    assert(!proto.validateStandardPacket(badLen, vendor));

    return 0;
}
