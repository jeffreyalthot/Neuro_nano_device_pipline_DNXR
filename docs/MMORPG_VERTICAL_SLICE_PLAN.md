# MMORPG Vertical Slice Plan (SFW)

## Scope livré
- UI complète: écran titre, connexion/création, sélection personnage, inventaire, équipement, compétences, carte/quêtes, options, HUD combat.
- États de boutons: normal, hover, pressed, disabled.
- Bindings clavier/souris/manette: WASD, E, I, C, K, M, Esc.
- Personnages manga-réalistes adultes (masculin/féminin) avec rigs locomotion/combat/idle et blendshapes de base.
- Customisation: cheveux, yeux/iris, peau, slots vêtements séparés.
- Caméras: gameplay 3e personne, dialogue, cinématique.
- Capteurs perception: vision, audition, proximité.
- Monde de départ: hub ville, zone nature, zone combat bas niveau.
- Immersion: cycle jour/nuit, météo simple, vent, audio spatial, feedback physique non-sexuel.
- Combat/equipement/stats: HP, stamina, stats de base, armes + FX, compétences actives/passives.

## Notes implementation
Les structures de données runtime sont dans `src/immersion/VirtualWorldState.h` et initialisées via `GameDesignBridge::integrateBaseGameplaySystems`.

## Exclusions
Ce lot est strictement SFW (pas de contenu sexuel explicite).
