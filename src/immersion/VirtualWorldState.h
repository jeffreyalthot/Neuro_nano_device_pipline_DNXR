#pragma once
// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : VirtualWorldState.h
//
// OBJECTIF : Pilotage de l'environnement 3D et des 5 sens EXCLUSIVEMENT
//            depuis le neural chip via CognitiveState + XRFullBodyCommand.
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] applyNeuralChipInput() — méthode principale : reçoit CognitiveState
//       + XRFullBodyCommand directement depuis runInferenceTick() et
//       runFullBodyControlTick(). Remplace l'ancien applyEmbodiedInput()
//       qui attendait des valeurs de contrôleurs physiques externes.
//   [B] SenseFrame — structure portant les 5 sens calculés depuis le chip :
//       vision (clarté/profondeur), audition (immersion sonore), toucher
//       (haptique), odorat et goût (stimulation chimiosensorielle).
//   [C] computeNeuralSenses() — mappe CognitiveState → SenseFrame :
//       chaque sens est une fonction du vecteur d'état cognitif neural.
//   [D] NeuralSceneDescriptor — décrit la scène 3D générée par le chip :
//       luminosité, distance de rendu, densité de particules, profondeur.
//   [E] buildNeuralSceneDescriptor() — produit NeuralSceneDescriptor depuis
//       CognitiveState pour piloter le moteur de rendu 3D neural.
//   [F] EmbodiedInput conservé pour compatibilité ascendante uniquement.
// =============================================================================

#include <string>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>
#include <vector>

namespace dnxr {

// Forward declarations
struct CognitiveState;
struct XRFullBodyCommand;
struct XRAdaptationFrame;

// ─────────────────────────────────────────────────────────────────────────────
// [B] SenseFrame — les 5 sens + proprioception calculés depuis le neural chip
// Chaque champ est en [0, 1] sauf indication contraire.
// ─────────────────────────────────────────────────────────────────────────────
struct SenseFrame {
    // Canal visuel : clarté optique, profondeur perçue, luminosité de scène
    float visualClarity     = 0.0f;  // netteté globale de l'image mentale
    float visualDepth       = 0.0f;  // perception de profondeur / relief 3D
    float visualBrightness  = 0.0f;  // intensité lumineuse de l'environnement

    // Canal auditif : immersion sonore, directionnalité, confort fréquentiel
    float auditoryImmersion = 0.0f;  // richesse du paysage sonore neural
    float auditoryDirection = 0.0f;  // précision de la spatialisation audio

    // Canal haptique : densité de stimulation tactile, vibration, pression
    float hapticDensity     = 0.0f;  // intensité du retour haptique neural
    float hapticFrequency   = 0.0f;  // fréquence des impulsions tactiles (Hz normalisé)

    // Canal olfactif : intensité de la stimulation chimiosensorielle
    float olfactoryIntensity = 0.0f; // force de l'odeur générée neuralement

    // Canal gustatif : stimulation de la perception du goût
    float gustatoryIntensity = 0.0f; // force du goût généré neuralement

    // Canal proprioceptif : conscience du corps dans l'espace virtuel
    float proprioceptiveLoad = 0.0f; // charge proprioceptive (orientation/équilibre)
    float vestibularStability = 0.0f; // stabilité du système vestibulaire neural
};

// ─────────────────────────────────────────────────────────────────────────────
// [D] NeuralSceneDescriptor — paramètres de la scène 3D générée par le chip
// Piloté intégralement depuis CognitiveState, sans entrée physique externe.
// ─────────────────────────────────────────────────────────────────────────────
struct NeuralSceneDescriptor {
    float ambientLuminosity  = 0.5f;  // luminosité ambiante de la scène [0,1]
    float renderDistance     = 0.5f;  // distance de rendu normalisée [0,1]
    float particleDensity    = 0.0f;  // densité des effets particules [0,1]
    float fogDensity         = 0.0f;  // brouillard / profondeur atmosphérique [0,1]
    float colorSaturation    = 0.7f;  // saturation des couleurs de la scène [0,1]
    float temporalCoherence  = 1.0f;  // cohérence temporelle (anti-flicker neural) [0,1]
    std::string biomeName;            // biome généré ("deep_forest", "crystal_void", …)
    std::string lightingMode;         // mode d'éclairage ("neural_dawn", "focus_zenith", …)
};


// ─────────────────────────────────────────────────────────────────────────────
// VirtualViewFrame — vue caméra/stéréo de l'environnement virtuel
// Ce frame est la passerelle concrète vers une image XR rendable : il combine
// pose de caméra, projection, atmosphère, palette et stabilité perceptive.
// ─────────────────────────────────────────────────────────────────────────────
struct VirtualViewFrame {
    float cameraX = 0.0f;
    float cameraY = 1.65f;
    float cameraZ = 0.0f;
    float yawDeg = 0.0f;
    float pitchDeg = 0.0f;
    float fieldOfViewDeg = 90.0f;
    float nearPlane = 0.05f;
    float farPlane = 80.0f;
    float stereoSeparation = 0.064f;
    float stereoConvergence = 4.0f;
    float exposure = 0.5f;
    float contrast = 0.5f;
    float saturation = 0.7f;
    float fogDensity = 0.0f;
    float particleDensity = 0.0f;
    float horizonDepth = 0.5f;
    float frameStability = 1.0f;
    std::string sceneName;
    std::string biomeName;
    std::string lightingMode;

    std::vector<float> toRenderVector() const;
};


// ─────────────────────────────────────────────────────────────────────────────
// VisualFieldFrame — champ visuel vécu depuis l'intérieur du monde virtuel
// Il décrit la portion réellement visible par l'utilisateur: frustum binoculaire,
// zone fovéale, vision périphérique, profondeur et stabilité rétinienne.
// ─────────────────────────────────────────────────────────────────────────────
struct VisualFieldFrame {
    float horizontalFovDeg = 100.0f;
    float verticalFovDeg = 90.0f;
    float leftEyeYawOffsetDeg = -1.5f;
    float rightEyeYawOffsetDeg = 1.5f;
    float focalDepth = 4.0f;
    float fovealRadius = 0.22f;
    float peripheralAwareness = 0.75f;
    float peripheralBlur = 0.20f;
    float vignetteStrength = 0.10f;
    float motionComfort = 0.85f;
    float nearDepthLayer = 0.25f;
    float midDepthLayer = 0.55f;
    float farDepthLayer = 0.80f;
    float gazeFocusX = 0.5f;
    float gazeFocusY = 0.5f;
    float retinalStability = 1.0f;

    std::vector<float> toRenderVector() const;
};

class VirtualWorldState {
public:
    // Conservé pour compatibilité ascendante uniquement.
    // [F] Utiliser applyNeuralChipInput() pour tout pilotage neural chip.
    struct EmbodiedInput {
        float moveForward = 0.0f;
        float moveRight   = 0.0f;
        float lookYaw     = 0.0f;
        float lookPitch   = 0.0f;
        bool  interact    = false;
        float vision      = 1.0f;
        float audition    = 1.0f;
        float touch       = 1.0f;
        float smell       = 0.5f;
        float taste       = 0.5f;
    };

    VirtualWorldState();

    // ─────────────────────────────────────────────────────────────────────
    // [A] NOUVELLE API PRINCIPALE — pilotage neural chip only
    //
    // Reçoit le CognitiveState de runInferenceTick() et le XRFullBodyCommand
    // de runFullBodyControlTick() pour piloter en temps réel :
    //   • La position/orientation du joueur dans le monde 3D
    //   • L'intensité et le biome de la scène
    //   • Les 5 sens (SenseFrame)
    //   • Le NeuralSceneDescriptor pour le moteur de rendu
    // ─────────────────────────────────────────────────────────────────────
    void applyNeuralChipInput(
        const CognitiveState&     cognitive,
        const XRFullBodyCommand&  bodyCmd,
        const XRAdaptationFrame&  adaptation,
        double                    deltaTime = 0.016);

    // ─────────────────────────────────────────────────────────────────────
    // [C] Calcul des 5 sens depuis le CognitiveState neural chip
    // ─────────────────────────────────────────────────────────────────────
    static SenseFrame computeNeuralSenses(const CognitiveState& cognitive);

    // ─────────────────────────────────────────────────────────────────────
    // [E] Génération du descripteur de scène 3D depuis le chip
    // ─────────────────────────────────────────────────────────────────────
    static NeuralSceneDescriptor buildNeuralSceneDescriptor(
        const CognitiveState&    cognitive,
        const XRAdaptationFrame& adaptation);

    // API de base (inchangée)
    void setScene(const std::string& newScene);
    void setIntensity(float value);
    void update(double deltaTime = 0.016);
    std::string getSceneName() const;
    float getIntensity() const;
    bool isDirty() const;
    void setOnStateChanged(const std::function<void(const std::string&, float)>& cb);

    // API cognitive (conservée)
    void integrateCognitiveInput(float emotionalLevel, float focusLevel);
    void applyEmbodiedInput(const EmbodiedInput& input, double deltaTime = 0.016);
    std::string getAvatarPoseSummary() const;
    float getFiveSenseBlend() const;

    // Accès au dernier SenseFrame, NeuralSceneDescriptor et à la vue XR produits
    SenseFrame             getLastSenseFrame()  const;
    NeuralSceneDescriptor  getLastSceneDescriptor() const;
    VirtualViewFrame       buildVirtualViewFrame() const;
    VisualFieldFrame       buildVisualFieldFrame() const;

private:
    void notifyObservers() const;
    void updateAvatarFromBodyCommand(const XRFullBodyCommand& cmd, float dt);
    void updateSceneFromCognitive(const CognitiveState& cognitive,
                                  const XRAdaptationFrame& adaptation);
    static std::string selectBiome(const CognitiveState& cognitive);
    static std::string selectLightingMode(const CognitiveState& cognitive);
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

    // Avatar position/orientation dans le monde 3D — pilotés par neural chip
    float posX_;
    float posY_;
    float posZ_;
    float yawDeg_;
    float pitchDeg_;
    bool  interactionActive_;
    float senseBlend_;

    // Derniers outputs neural chip
    SenseFrame            lastSenseFrame_;
    NeuralSceneDescriptor lastSceneDescriptor_;
};

} // namespace dnxr
