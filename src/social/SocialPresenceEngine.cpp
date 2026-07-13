#include "SocialPresenceEngine.h"

#include <cmath>

namespace dnxr {

SocialPresenceEngine::
SocialPresenceEngine() {

    initialize();
}

void SocialPresenceEngine::
initialize() {

    nextEntityId_ = 1;

    state_.globalPresence = 0.0f;

    state_.collectiveResonance = 0.0f;

    state_.subconsciousBelievability = 0.0f;

    state_.interactionCoherence = 1.0f;

    state_.emotionalSynchronization = 0.0f;
}

uint64_t
SocialPresenceEngine::
createEntity(
    const std::string& name,
    const SocialSignal& signal,
    const std::vector<float>& neuralSignature) {

    SocialEntity entity;

    entity.id = nextEntityId_++;

    entity.name = name;

    entity.signal = signal;

    entity.neuralSignature = neuralSignature;

    entity.realismFactor = 1.0f;

    entity.emotionalAttachment = 0.0f;

    entity.interactionStability = 1.0f;

    entities_[entity.id] = entity;

    return entity.id;
}

void
SocialPresenceEngine::
updateEntitySignal(
    uint64_t entityId,
    const SocialSignal& signal) {

    auto it = entities_.find(entityId);

    if(it == entities_.end())
        return;

    it->second.signal = signal;
}

void
SocialPresenceEngine::
computeGlobalPresence() {

    if(entities_.empty()) {

        state_.globalPresence = 0.0f;
        return;
    }

    float presence = 0.0f;

    for(const auto& pair : entities_) {

        presence +=
            pair.second.signal.presenceIntensity;
    }

    presence /=
        static_cast<float>(
            entities_.size());

    state_.globalPresence = presence;
}

void
SocialPresenceEngine::
computeCollectiveResonance() {

    float resonance = 0.0f;

    for(const auto& pair : entities_) {

        resonance +=
            pair.second.signal.emotionalProjection *
            pair.second.signal.vocalResonance;
    }

    if(!entities_.empty()) {

        resonance /=
            static_cast<float>(
                entities_.size());
    }

    state_.collectiveResonance =
        resonance;
}

void
SocialPresenceEngine::
computeBelievability() {

    state_.subconsciousBelievability =
        (state_.globalPresence * 0.4f) +
        (state_.collectiveResonance * 0.6f);
}

void
SocialPresenceEngine::
computeInteractionCoherence() {

    float coherence = 0.0f;

    for(const auto& pair : entities_) {

        coherence +=
            pair.second.signal.behavioralConsistency;
    }

    if(!entities_.empty()) {

        coherence /=
            static_cast<float>(
                entities_.size());
    }

    state_.interactionCoherence =
        coherence;
}

void
SocialPresenceEngine::
computeEmotionalSynchronization() {

    float sync = 0.0f;

    for(const auto& pair : entities_) {

        sync +=
            pair.second.signal.emotionalProjection;
    }

    if(!entities_.empty()) {

        sync /=
            static_cast<float>(
                entities_.size());
    }

    state_.emotionalSynchronization =
        sync;
}

void
SocialPresenceEngine::
stabilizeSocialMesh() {

    for(auto& pair : entities_) {

        pair.second.interactionStability =
            state_.interactionCoherence;

        pair.second.realismFactor =
            state_.subconsciousBelievability;
    }
}

void
SocialPresenceEngine::
update(
    float deltaTime) {

    (void)deltaTime;

    computeGlobalPresence();

    computeCollectiveResonance();

    computeBelievability();

    computeInteractionCoherence();

    computeEmotionalSynchronization();

    stabilizeSocialMesh();
}

const SocialState&
SocialPresenceEngine::
getState() const {

    return state_;
}

const std::unordered_map<
    uint64_t,
    SocialEntity>&
SocialPresenceEngine::
getEntities() const {

    return entities_;
}

}