# COMMANDE_SECTION_GAME_DEVICE

Ce fichier sert de point de départ pour la conception du jeu virtuel en bridge avec les systèmes neuronaux, audio, visuel et sécurité.

## Commandes / Fonctions de base à appeler

- `dnxr::ImmersionCore::buildImmersiveFrame(calibratedSignal)`
  - Construit une frame immersive (visuel + audio spatial).
  - Applique le bridge gameplay de base (menu, HP, caméra, zones audio, spawn).

- `dnxr::ImmersionCore::handleUserQuickMenuInput(quickMenuButtonPressed, safeQuitSelected)`
  - Gère l’apparition du menu rapide dans le champ visuel.
  - Active le safe-exit progressif obligatoire.

- `dnxr::ImmersionCore::updateUserCognitiveState(inferredState)`
  - Synchronise l’intensité avec l’état cognitif (focus, calm, stress, deep immersion).

- `dnxr::ImmersionCore::comfortGate()`
  - Retourne un coefficient de confort neuronal pour normaliser les vecteurs.

## Fonctions bridge gameplay (nouvelles)

- `dnxr::GameDesignBridge::integrateBaseGameplaySystems(world)`
  - Prépare:
    - Spawn points (`spawn_alpha`, `spawn_beta`, `spawn_gamma`)
    - Caméras (`cam_fp`, `cam_tp`)
    - Capteurs sonores (`ear_left`, `ear_right`, `torso_zone`)
    - Assets UI PNG (menu, HP, respawn)

- `dnxr::GameDesignBridge::applyHealthState(world, hp, hpMax)`
  - Met à jour la barre HP.
  - Si HP == 0: personnage translucide + menu de réapparition affiché.

- `dnxr::GameDesignBridge::processRespawn(world, respawnSelected, spawnIndex)`
  - Réapparition sur spawn sélectionné.
  - HP restaurée, skin redevient opaque.

## Assets PNG UI à créer/intégrer

- `assets/ui/hud_hp_bar_1024x128.png` (1024x128)
- `assets/ui/quick_menu_panel_1024x1024.png` (1024x1024)
- `assets/ui/respawn_menu_960x540.png` (960x540)

## Intégration Blender / monde virtuel

- Carte active par défaut:
  - `assets/maps/default_nexus_atrium.blendmap`
- Format déclaré:
  - `blender-map-v1`

## Sécurité neural node / normalisation / adoucissement

- Soft-normalization audio: `dnxr::SpatialAudioEngine::softNormalize`.
- Adoucissement sortie immersion (safe-exit): fade progressif `safeExitComfortFade`.
- Safe-exit obligatoire via menu visuel + validation utilisateur.

## Départ rapide conception jeu

1. Brancher vos scènes Blender sur `VirtualWorldState::availableMaps`.
2. Importer les PNG UI ci-dessus.
3. Piloter HP et respawn via `applyHealthState` et `processRespawn`.
4. Piloter la caméra active dans `VirtualWorldState::cameras`.
5. Garder `handleUserQuickMenuInput` actif pour sécurité de sortie.

