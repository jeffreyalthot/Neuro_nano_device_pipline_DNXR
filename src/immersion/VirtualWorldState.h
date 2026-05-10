#pragma once

#include "SpatialAudioEngine.h"
#include "../mmorpg/QuestSystem.h"

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
    std::string style{"manga_realistic"};
    std::string bodyType{"adult"};
    std::string frameType{"balanced"};
    std::string eyeShape{"round"};
    std::string eyeColor{"brown"};
    std::string irisAccent{"amber"};
    std::string mouthStyle{"soft_smile"};
    std::string hairStyle{"short_layered"};
    std::string hairColor{"black"};
    std::string skinTone{"light_neutral"};
    std::string earShape{"normal"};
    std::string faceShape{"oval"};
    std::string beardStyle{"none"};
    std::string beardColor{"black"};
    std::string expressionPreset{"neutral"};
    std::string physiqueProfile{"realistic_anime_adult"};
    std::string skinMaterial{"soft_natural"};
};

struct WearableItem {
    std::string id;
    std::string slot;
    std::string label;
    std::string meshPath;
    std::string materialPreset;
};

struct AvatarRigProfile {
    std::string rigId;
    std::string locomotionRig;
    std::string combatRig;
    std::string idleRig;
    std::vector<std::string> blendshapePresets;
};

struct CharacterSelectionState {
    bool selectionMenuEnabled{true};
    std::string selectionMapId{"avatar_selection_lounge"};
    std::string selectedCharacterId{"avatar_male_adult_a"};
    std::vector<std::string> availableCharacterIds{"avatar_male_adult_a", "avatar_female_adult_a"};
    AvatarAppearance appearance{};
    std::vector<std::string> equippedStyleSlots{"head", "torso", "legs", "feet", "accessory"};
    std::vector<WearableItem> wardrobeCatalog{};
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
    float stamina{100.0F};
    std::vector<std::string> inventory{"starter_comm", "nano_compass", "basic_sword"};
    std::vector<std::string> skills{"walk", "listen", "interact_menu", "slash_basic"};
    std::vector<std::string> equippedItems{"cloth_tunic", "beginner_boots", "basic_sword"};
};

struct SpawnPoint { std::string id; std::string zoneName; float x; float y; float z; };

struct CameraRig {
    std::string id;
    std::string mode;
    bool isActive;
    float fovDegrees;
    float nearClip;
    float farClip;
    float anchorX{0.0F};
    float anchorY{1.65F};
    float anchorZ{0.0F};
};

struct AudioCaptureZone { std::string id; float x; float y; float z; float radiusMeters; };

struct GameplayInputBinding { std::string action; std::string keyboard; std::string mouse; std::string gamepad; };

struct MenuButtonVisualState {
    std::string normalPng;
    std::string hoverPng;
    std::string pressedPng;
    std::string disabledPng;
};

struct MenuScreenDefinition {
    std::string id;
    std::string title;
    std::vector<std::string> sections;
};

struct UIAssetSpec {
    std::string hpBarPng{"assets/ui/hud_hp_bar_1024x128.png"};
    std::string quickMenuPng{"assets/ui/quick_menu_panel_1024x1024.png"};
    std::string respawnMenuPng{"assets/ui/respawn_menu_960x540.png"};
    std::string quickMenuButtonNormalPng{"src/images/Images_Menu_Option/Bouton_3_Point_Normal_60x60.png"};
    std::string quickMenuButtonPressPng{"src/images/Images_Menu_Option/Bouton_3_Point_Press_60x60.png"};
    std::string quitButtonNormalPng{"src/images/Images_Menu_Option/Bouton_Frame_Quit_Normal_300x60.png"};
    std::string quitButtonPressPng{"src/images/Images_Menu_Option/Bouton_Frame_Quit_Press_300x60.png"};
    MenuButtonVisualState quickMenuButton{
        "src/images/Images_Menu_Option/Bouton_3_Point_Normal_60x60.png",
        "src/images/Images_Menu_Option/Bouton_3_Point_Normal_60x60.png",
        "src/images/Images_Menu_Option/Bouton_3_Point_Press_60x60.png",
        "src/images/Images_Menu_Option/Bouton_3_Point_Normal_60x60.png"};
    MenuButtonVisualState quitButton{
        "src/images/Images_Menu_Option/Bouton_Frame_Quit_Normal_300x60.png",
        "src/images/Images_Menu_Option/Bouton_Frame_Quit_Normal_300x60.png",
        "src/images/Images_Menu_Option/Bouton_Frame_Quit_Press_300x60.png",
        "src/images/Images_Menu_Option/Bouton_Base_Frame_Normal_300x60.png"};
    int hpBarWidth{1024}; int hpBarHeight{128}; int quickMenuWidth{1024}; int quickMenuHeight{1024};
    int respawnMenuWidth{960}; int respawnMenuHeight{540}; int quickMenuButtonWidth{60}; int quickMenuButtonHeight{60};
    int quitButtonWidth{300}; int quitButtonHeight{60};
};

struct EnvironmentRuntimeState {
    float dayNightTimeHours{12.0F};
    std::string weatherMode{"clear"};
    float windIntensity{0.25F};
    float moonVisibility{0.6F};
    bool spatialAudioEnabled{true};
    bool clothWindFeedbackEnabled{true};
};

struct VirtualWorldState {
    std::string sceneName{"BootChamber"};
    VirtualMapAsset activeMap{"default_blender_map","Default Nexus Atrium","blender-map-v1","assets/maps/default_nexus_atrium.blendmap"};
    std::vector<VirtualMapAsset> availableMaps{activeMap,{"avatar_selection_lounge","Avatar Selection Lounge","blender-map-v1","assets/maps/avatar_selection_lounge.blendmap"},{"mmorpg_open_world_alpha","MMORPG Open World Alpha","blender-map-v1","assets/maps/mmorpg_open_world_alpha.blendmap"},{"starter_town_hub","Starter Town Hub","blender-map-v1","assets/maps/starter_town_hub.blendmap"},{"nature_shard_fields","Nature Shard Fields","blender-map-v1","assets/maps/nature_shard_fields.blendmap"},{"low_level_combat_frontier","Low Level Combat Frontier","blender-map-v1","assets/maps/low_level_combat_frontier.blendmap"}};
    std::vector<SpatialAudioSource> audioSources{{"object-reactor", "environment", 4.0F, -25.0F, 0.45F},{"npc-guide", "character", 2.5F, 35.0F, 0.40F},{"player-proximity", "player", 3.5F, 150.0F, 0.35F}};
    UIAssetSpec uiAssets{};
    std::vector<MenuScreenDefinition> menuScreens{};
    std::vector<GameplayInputBinding> inputBindings{};
    std::vector<SpawnPoint> spawnPoints{};
    std::string activeSpawnPointId{"spawn_alpha"};
    std::vector<CameraRig> cameras{};
    std::vector<AudioCaptureZone> audioSensors{};
    std::vector<AvatarRigProfile> avatarRigProfiles{};
    EnvironmentRuntimeState environment{};

    float hpCurrent{100.0F}; float hpMax{100.0F}; float hpNormalized{1.0F};
    bool avatarTranslucent{false}; float avatarTransparency{1.0F}; bool respawnMenuVisible{false}; bool gameBridgeEnabled{false};
    CharacterSelectionState characterSelection{}; CharacterPersistence saveState{};
    std::string mapsDirectoryPath{"assets/maps/"};
    std::string secondaryWorldMapName{"MMORPG Open World Alpha"};
    std::string baseSkinDescription{"adult manga-realistic body + camera vision + hearing unit"};
    std::vector<Quest> questLog{};
    PlayerProgression progression{};

    float intensity{0.5F}; float environmentSyncPhase{0.0F}; bool quickMenuVisible{false}; bool quickMenuButtonPressed{false};
    bool quitButtonPressed{false}; bool safeExitRequested{false}; bool safeExitCompleted{false}; float safeExitComfortFade{1.0F};
};

} // namespace dnxr
