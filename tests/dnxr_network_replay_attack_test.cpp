#include "../src/network/NeuralCryptoTransport.h"
#include "../src/network/PacketSigner.h"
#include "../src/network/ReplayProtectionWindow.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    dnxr::ReplayProtectionWindow replayWindow(4U);
    assert(replayWindow.accept(1U));
    assert(!replayWindow.accept(1U));
    assert(replayWindow.accept(3U));
    assert(replayWindow.accept(2U));
    assert(replayWindow.highestAcceptedCounter() == 3U);

    const std::vector<std::uint8_t> signedPayload{1U, 2U, 3U, 4U};
    const auto signature = dnxr::PacketSigner::sign("session", 7U, "nonce", signedPayload, "audit_key");
    assert(dnxr::PacketSigner::verify("session", 7U, "nonce", signedPayload, signature, "audit_key"));
    assert(!dnxr::PacketSigner::verify("session", 8U, "nonce", signedPayload, signature, "audit_key"));

    dnxr::SimulationCryptoTransport transport;
    auto sender = transport.createSession("avatar_alpha", "session-secret-001", 60000);
    auto receiver = transport.createSession("avatar_alpha", "session-secret-001", 60000);

    const std::vector<float> payload{0.25f, 0.5f, -0.75f, 1.0f};
    const auto packet = transport.seal(payload, sender);
    assert(packet.counter == 1U);
    assert(!packet.nonce.empty());
    assert(!packet.signature.empty());
    assert(!packet.cipherPayload.empty());

    std::vector<float> opened;
    assert(transport.open(packet, receiver, opened));
    assert(opened == payload);
    assert(receiver.highestAcceptedCounter() == 1U);

    std::vector<float> replayed;
    assert(!transport.open(packet, receiver, replayed));
    assert(replayed.empty());

    auto tampered = packet;
    tampered.counter = 2U;
    tampered.cipherPayload[0] ^= 0xffU;
    std::vector<float> tamperedOut;
    assert(!transport.open(tampered, receiver, tamperedOut));
    assert(tamperedOut.empty());

    const auto secondPacket = transport.seal(payload, sender);
    assert(secondPacket.counter == 2U);
    std::vector<float> secondOpened;
    assert(transport.open(secondPacket, receiver, secondOpened));
    assert(secondOpened == payload);

    auto wrongIdentityReceiver = transport.createSession("avatar_beta", "session-secret-001", 60000);
    std::vector<float> wrongIdentityOut;
    assert(!transport.open(secondPacket, wrongIdentityReceiver, wrongIdentityOut));

    return 0;
}
