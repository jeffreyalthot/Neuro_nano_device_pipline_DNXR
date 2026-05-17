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
Le runtime actuel expose un contrat **compact** dans `src/immersion/VirtualWorldState.h` (scène + intensité + transitions).
Les éléments MMORPG riches listés ci-dessus servent de cible produit et ne sont pas encore tous matérialisés dans l'état runtime.
L'initialisation passe par `GameDesignBridge::integrateBaseGameplaySystems`.

## Exclusions
Ce lot est strictement SFW (pas de contenu sexuel explicite).
