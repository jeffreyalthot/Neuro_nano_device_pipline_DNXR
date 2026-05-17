#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

namespace dnxr {

/// Représente l'état courant du monde virtuel XR.
/// Sert de tampon de cohérence entre les données neuronales et la scène immersive.
class VirtualWorldState {
public:
    struct EmbodiedInput {
        float moveForward = 0.0f;
        float moveRight = 0.0f;
        float lookYaw = 0.0f;
        float lookPitch = 0.0f;
        bool interact = false;
        float vision = 1.0f;
        float audition = 1.0f;
        float touch = 1.0f;
        float smell = 0.5f;
        float taste = 0.5f;
    };

    VirtualWorldState();

    /// Définit la scène XR active.
    void setScene(const std::string& newScene);

    /// Ajuste la luminance / intensité environnementale cible (entre 0 et 1).
    void setIntensity(float value);

    /// Met à jour les transitions temporelles (à appeler sur chaque frame).
    void update(double deltaTime = 0.016);

    /// Récupération d'informations.
    std::string getSceneName() const;
    float getIntensity() const;
    bool isDirty() const;

    /// Configure un callback déclenché sur modification d’état.
    void setOnStateChanged(const std::function<void(const std::string&, float)>& cb);

    /// Application contextuelle externe (par exemple via état cognitif).
    void integrateCognitiveInput(float emotionalLevel, float focusLevel);
    void applyEmbodiedInput(const EmbodiedInput& input, double deltaTime = 0.016);
    std::string getAvatarPoseSummary() const;
    float getFiveSenseBlend() const;

private:
    void notifyObservers() const;
    static float clamp01(float v);
    static float smoothBlend(float a, float b, float t);

private:
    mutable std::mutex mtx_;
    std::string sceneName_;

    std::atomic<float> intensity_;
    std::atomic<float> targetIntensity_;
    float transitionSpeed_;

    std::atomic<bool> dirtyFlag_;
    std::chrono::steady_clock::time_point lastUpdate_;

    std::function<void(const std::string&, float)> onStateChanged_;
    float posX_;
    float posY_;
    float posZ_;
    float yawDeg_;
    float pitchDeg_;
    bool interactionActive_;
    float senseBlend_;
};

} // namespace dnxr
