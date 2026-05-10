# MMORPG Immersion Foundation (Phase 1)

Ce livrable priorise:
- menus MMO (normal/press) et commandes de base,
- états de perception (caméras + zones audio),
- maps Blender de démarrage,
- base avatar manga-réaliste adulte,
- personnalisation (cheveux, yeux, iris, peau),
- vêtements séparés et sélectionnables.

## Notes de conception

- Les personnages sont définis comme **adultes** et destinés à une représentation sécurisée en contexte jeu vidéo.
- Le moteur garde une séparation claire entre:
  - **apparence du personnage**,
  - **garde-robe/équipement**,
  - **rig de mouvement/animation**,
  - **capteurs de perception** (vision/audio).
- La sensation environnementale est simulée via:
  - cycle jour/nuit,
  - météo,
  - vent,
  - audio spatial.

## Assets attendus

- `assets/maps/*.blendmap` : maps jouables.
- `assets/avatars/wearables/*.mesh` : vêtements/cheveux.
- `assets/ui/*.png` : UI HUD, respawn, menus.

## Prochaines étapes

1. Import Blender des maps listées dans `VirtualWorldState`.
2. Génération des maillages de vêtements du `wardrobeCatalog`.
3. Liaison des collisions physiques par slot d’équipement.
4. Bind des touches dans la couche input runtime.
