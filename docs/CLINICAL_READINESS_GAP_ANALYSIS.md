# Analyse d'écart — simulation logicielle vs usage implant neuronal humain

## Statut actuel (au 17 mai 2026)
Ce projet est un **scaffold de simulation logicielle modulaire**.
Il ne constitue pas une pile clinique/réglementaire/temps-réel certifiée pour une utilisation humaine.

## Ce que le dépôt fait aujourd'hui
- Pipeline modulaire C++ (signal, cognition, immersion, safety, rendering).
- Données synthétiques et modes « simulation ON » sur les interfaces devices/network.
- Boucle runtime démonstrative avec logs et garde-fous logiciels.
- Test de fumée CTest sur l'état d'immersion (`dnxr_state_smoke_test`) pour vérifier les invariants de base scène/intensité.
- Tests unitaires initiaux (signal/safety/cognition) + test d'intégration pipeline.
- Observabilité runtime initiale: latence, jitter, pics latence, comptage événements safety.
- Safety durcie: état d'urgence codifié (`Nominal`, `Warning`, `EmergencyLatched`) + raison d'urgence.
- Protocole interop versionné v1 avec validation stricte du paquet.

## Ce qui manque pour un usage humain réel
1. **Conformité réglementaire**
   - Processus qualité type IEC 62304 / ISO 14971 / ISO 13485.
   - Traçabilité exigences↔tests↔risques.
2. **Sûreté clinique & validation**
   - Validation pré-clinique/clinique indépendante.
   - Protocoles d'essais, critères d'arrêt, supervision médicale.
3. **Temps-réel déterministe**
   - Garanties de latence worst-case et budgets timing bout-en-bout.
   - Stratégies watchdog/rollback certifiables.
4. **Cybersécurité & privacy**
   - Chiffrement matériel, gestion clés, attestations, hardening.
   - Gouvernance donnée neurophysiologique.
5. **Intégration matériel certifiée**
   - Drivers et protocoles validés sur matériel réel.
   - Verification/validation système sur bancs instrumentés.

## Plan d'avancement recommandé
- **Phase A (simulation robuste)**: tests unitaires/intégration, contrats d'API, scénarios de faute.
- **Phase B (pré-certification software process)**: exigences formelles, matrice de risque, CI qualité.
- **Phase C (pré-clinique)**: HIL (hardware-in-the-loop), mesures latence/jitter, audits sécurité.
- **Phase D (clinique/réglementaire)**: uniquement via partenaires cliniques et autorités compétentes.

## Position du projet
Ce dépôt doit rester traité comme base R&D simulation tant que ces phases ne sont pas menées et auditées.
