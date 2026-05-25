# Plan d'amélioration logiciel — Neural Chip pour immersion profonde XR

## Périmètre
Ce plan impose un mode **neural chip only** pour l'immersion XR profonde: la boucle primaire d'entrée est neuronale et non dépendante d'un périphérique d'entrée XR classique.

## 1) Pile temps réel neuro→XR (<20 ms)
### Objectif
Garantir une boucle fermée déterministe: acquisition stable → filtrage adaptatif → décodage intentionnel → rendu XR.

### Livrables
- Scheduler temps réel à priorités strictes (threads séparés: acquisition, décodage, safety, rendu).
- Pipeline asynchrone zero-copy (ring buffers lock-free, allocateurs pool préchauffés).
- Monitoring de jitter et compensation dynamique (resync horodatage + adaptation QoS).

### KPI
- Latence bout-en-bout p95 < 20 ms, p99 < 25 ms.
- Jitter p95 < 2 ms.
- Drop frame neuro→XR < 0.5%.

## 2) Décodage sémantique robuste
### Objectif
Passer du signal neural brut à des commandes XR fiables, personnalisées et robustes à la dérive.

### Livrables
- Modèles multimodaux par utilisateur (few-shot + calibration continue).
- Détection de dérive temporelle (fatigue, stress, déplacement capteur).
- Score de confiance natif sur chaque intention avec fallback automatique.

### KPI
- Taux de faux positifs d'intention < 1% en tâche critique.
- Temps de recalibration utilisateur ≤ 90 s.
- Dégradation de performance après 60 min < 10%.

## 3) Langage d'intention vectoriel standardisé
### Objectif
Normaliser la grammaire qui mappe les vecteurs neuraux vers les actions XR.

### Livrables
- Schéma d'embedding versionné (`neuro.intent.embedding.vX`).
- Ontologie d'actions XR (prendre, tourner, interrompre, annuler, composer).
- Protocole d'interop runtime↔moteur↔device (contrats d'API et compatibilité de version).

### KPI
- Compatibilité ascendante des schémas > 95%.
- Résolution d'ambiguïté < 50 ms dans le runtime.

## 4) Boucle de feedback neuro-adaptative
### Objectif
Adapter l'immersion en continu selon l'état neuronal en minimisant charge cognitive et cybersickness.

### Livrables
- Moteur d'adaptation cognitive (charge mentale, attention, inconfort).
- Modulation automatique rendu/interactions basée état neuronal.
- Apprentissage en ligne sécurisé avec garde-fous anti-dérive.

### KPI
- Réduction cybersickness mesurée ≥ 20% vs baseline statique.
- Amélioration de présence subjective ≥ 15%.

## 5) Sécurité fonctionnelle & neurorights by design
### Objectif
Rendre la plateforme sûre par défaut pour toute action pilotée par puce neurale.

### Livrables
- Consentement granulaire (quoi/quand/pourquoi lecture neurale).
- Chiffrement bout-en-bout des flux neuraux + rotation de clés.
- Séparation stricte identité vs signaux neuraux (domaines de données isolés).
- Kill switch logiciel et mode dégradé déterministe.

### KPI
- MTTR sécurité < 5 s vers mode dégradé.
- 100% des flux neuraux chiffrés en transit et au repos.

## 6) OS/Runtime XR orienté neuro-entrée
### Objectif
Émuler un socle OS XR nativement compatible entrée neuronale primaire.

### Livrables
- API système `NeuralInput` standard (events, permissions, policy hooks).
- Permissions neuro de niveau caméra/micro (scopes explicites).
- Sandbox d'isolation pour apps manipulant des vecteurs neuraux.

### KPI
- 0 accès non autorisé aux vecteurs neuraux en test d'intrusion interne.

## 7) Métriques d'immersion validées cliniquement
### Objectif
Mesurer l'immersion réelle au-delà de FPS/latence.

### Livrables
- KPI combinés: présence subjective + corrélats neuraux + performance tâche.
- Protocoles d'évaluation reproductibles inter-sites.
- Bancs d'essai inter-lab avec traçabilité complète.

### KPI
- Reproductibilité inter-lab (écart métrique) ≤ 10%.

## 8) Tooling développeur industriel
### Objectif
Permettre l'industrialisation avec outillage complet.

### Livrables
- Simulateur de signaux neuraux (datasets synthétiques paramétrables).
- Debugger des flux vectoriels (projection latent space + timeline).
- Replay déterministe des sessions neuro-XR.
- CI/CD avec tests de latence neuro→XR et seuils bloquants.

### KPI
- Couverture des tests pipeline critique > 85%.
- Régression latence détectée en CI en < 10 min.

## Feuille de route pragmatique (ordre recommandé)
1. MVP pipeline temps réel acquisition→décodage→action simple.
2. Ontologie vectorielle d'intention + versioning.
3. Calibration continue utilisateur + scoring confiance.
4. Fallbacks sûrs (voix/regard/gestuelle minimale).
5. SDK dev + simulateur + tests automatisés.
6. Cadre conformité sécurité/éthique avant scale.

## Risques majeurs à anticiper
- Latence cognitive perçue malgré bonne latence technique.
- Faux positifs d'intention (actions non voulues).
- Fatigue mentale sur sessions longues.
- Dérive modèle sans recalibrage fréquent.
- Blocage réglementaire si gouvernance neurale absente.

## Intégration recommandée dans ce dépôt
- `src/core`: scheduler temps réel + orchestration des priorités.
- `src/signal`: filtrage adaptatif + monitoring jitter.
- `src/cognition`: scoring confiance + gestion de dérive.
- `src/network`: protocole d'intention versionné et interop.
- `src/safety`: kill switch, consentement, mode dégradé.
- `tests/`: suites latence/dérive/faux positifs/replay déterministe.
