# MMORPG 3D Neural Immersion Scaffold

Ce projet ajoute un squelette deployable pour un MMORPG neurally-assisted:
- systeme de quetes (acceptation, progression, recompense, XP, niveau)
- progression joueur persistable en memoire runtime
- points d'extension pour maps et assets 3D externes (Blender)

## Assets 3D manquants a produire
Consulte `assets/3d_placeholders/**` pour chaque objet/map attendu.

## Integration
- Module code: `src/mmorpg/QuestSystem.*`
- Etat runtime: `src/immersion/VirtualWorldState.h`
- Boucle immersive: `src/immersion/ImmersionCore.cpp`
