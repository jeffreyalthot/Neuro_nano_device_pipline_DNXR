#pragma once

#include "SpatialAudioEngine.h"

#include <string>
#include <vector>

namespace dnxr {

struct VirtualMapAsset {
    std::string mapId;
    std::string displayName;
    std::string sourceFormat;
    std::string sourcePath;
};

struct AvatarAppearance {
    std::string style{"manga"};
    std::string gender{"neutral"};
    std::string eyeShape{"round"};
    std::string eyeColor{"brown"};
    std::string mouthStyle{"soft_smile"};
    std::string hairStyle{"short"};
    std::string hairColor{"black"};
    std::string earShape{"normal"};
    std::string faceShape{"oval"};
    std::string beardStyle{"none"};
    std::string beardColor{"black"};
};

struct CharacterSelectionState {
    bool selectionMenuEnabled{true};
    std::string selectionMapId{"avatar_selection_lounge"};
    std::string selectedCharacterId{"base_sphere_skin"};
    AvatarAppearance appearance{};
};

struct CharacterPersistence {
    std::string saveSlotId{"slot_001"};
    std::string lastKnownMapId{"default_blender_map"};
    std::string lastKnownSpawnPointId{"spawn_alpha"};
    float posX{0.0F};
    float posY{0.0F};
    float posZ{2.0F};
    int level{1};
    float experience{0.0F};
    float hp{100.0F};
    std::vector<std::string> inventory{"starter_comm", "nano_compass"};
    std::vector<std::string> skills{"walk", "listen", "interact_menu"};
    std::vector<std::string> equippedItems{"sphere_shell", "spatial_audio_unit"};
};

struct SpawnPoint {
    std::string id;
    std::string zoneName;
    float x;
    float y;
    float z;
};

struct CameraRig {
    std::string id;
    std::string mode;
    bool isActive;
    float fovDegrees;
    float nearClip;
    float farClip;
};

struct AudioCaptureZone {
    std::string id;
    float x;
    float y;
    float z;
    float radiusMeters;
};

struct UIAssetSpec {
    std::string hpBarPng{"assets/ui/hud_hp_bar_1024x128.png"};
    std::string quickMenuPng{"assets/ui/quick_menu_panel_1024x1024.png"};
    std::string respawnMenuPng{"assets/ui/respawn_menu_960x540.png"};
    std::string quickMenuButtonNormalPng{"src/images/Images_Menu_Option/Bouton_3_Point_Normal_60x60.png"};
    std::string quickMenuButtonPressPng{"src/images/Images_Menu_Option/Bouton_3_Point_Press_60x60.png"};
    std::string quitButtonNormalPng{"src/images/Images_Menu_Option/Bouton_Frame_Quit_Normal_300x60.png"};
    std::string quitButtonPressPng{"src/images/Images_Menu_Option/Bouton_Frame_Quit_Press_300x60.png"};
    int hpBarWidth{1024};
    int hpBarHeight{128};
    int quickMenuWidth{1024};
    int quickMenuHeight{1024};
    int respawnMenuWidth{960};
    int respawnMenuHeight{540};
    int quickMenuButtonWidth{60};
    int quickMenuButtonHeight{60};
    int quitButtonWidth{300};
    int quitButtonHeight{60};
};

struct VirtualWorldState {
    std::string sceneName{"BootChamber"};
    VirtualMapAsset activeMap{
        "default_blender_map",
        "Default Nexus Atrium",
        "blender-map-v1",
        "assets/maps/default_nexus_atrium.blendmap"};
    std::vector<VirtualMapAsset> availableMaps{activeMap,
                                               {"avatar_selection_lounge",
                                                "Avatar Selection Lounge",
                                                "blender-map-v1",
                                                "assets/maps/avatar_selection_lounge.blendmap"},
                                               {"mmorpg_open_world_alpha",
                                                "MMORPG Open World Alpha",
                                                "blender-map-v1",
                                                "assets/maps/mmorpg_open_world_alpha.blendmap"}};
    std::vector<SpatialAudioSource> audioSources{
        {"object-reactor", "environment", 4.0F, -25.0F, 0.45F},
        {"npc-guide", "character", 2.5F, 35.0F, 0.40F},
        {"player-proximity", "player", 3.5F, 150.0F, 0.35F}
    };

    UIAssetSpec uiAssets{};
    std::vector<SpawnPoint> spawnPoints{};
    std::string activeSpawnPointId{"spawn_alpha"};
    std::vector<CameraRig> cameras{};
    std::vector<AudioCaptureZone> audioSensors{};

    float hpCurrent{100.0F};
    float hpMax{100.0F};
    float hpNormalized{1.0F};
    bool avatarTranslucent{false};
    float avatarTransparency{1.0F};
    bool respawnMenuVisible{false};
    bool gameBridgeEnabled{false};
    CharacterSelectionState characterSelection{};
    CharacterPersistence saveState{};
    std::string mapsDirectoryPath{"assets/maps/"};
    std::string secondaryWorldMapName{"MMORPG Open World Alpha"};
    std::string baseSkinDescription{"sphere-body + camera-vision + hearing-unit"};

    float intensity{0.5F};
    float environmentSyncPhase{0.0F};
    bool quickMenuVisible{false};
    bool quickMenuButtonPressed{false};
    bool quitButtonPressed{false};
    bool safeExitRequested{false};
    bool safeExitCompleted{false};
    float safeExitComfortFade{1.0F};
};

} // namespace dnxr
