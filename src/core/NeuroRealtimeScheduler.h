// =============================================================================
// DOSSIER : src/core/
// FICHIER : NeuroRealtimeScheduler.h
//
// AMÉLIORATION APPLIQUÉE :
//   [3] Ajout d'un mécanisme de timeout actif par tâche.
//       SchedulerTaskBudget reçoit un champ abortIfOverBudget.
//       endTask() retourne maintenant un bool indiquant si la tâche
//       a dépassé son budget, permettant à l'appelant d'abandonner
//       le rendu ou de passer en mode dégradé au lieu de laisser
//       la frame s'accumuler indéfiniment.
// =============================================================================

#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace dnxr {

struct SchedulerTaskBudget {
    std::string name;
    int         priority{0};
    double      targetMs{0.0};

    /// [AMÉLIORATION 3] Si true, endTask() signale un dépassement de budget
    /// afin que l'appelant puisse interrompre la tâche courante et passer
    /// en mode dégradé plutôt que de retarder les tâches suivantes.
    bool abortIfOverBudget{false};
};

struct SchedulerTickReport {
    double totalMs{0.0};
    double maxTaskMs{0.0};
    double jitterMs{0.0};
    bool   withinBudget{true};

    /// [AMÉLIORATION 3] Nombre de tâches ayant dépassé leur budget individuel.
    int overBudgetTaskCount{0};

    /// [AMÉLIORATION 3] Nom de la dernière tâche ayant demandé un abandon.
    std::string lastAbortedTask;
};

class NeuroRealtimeScheduler {
public:
    explicit NeuroRealtimeScheduler(double frameBudgetMs = 20.0);

    void registerTask(const SchedulerTaskBudget& task);

    SchedulerTickReport beginTick();

    /// [AMÉLIORATION 3] endTask retourne true si la tâche est dans son budget,
    /// false si elle l'a dépassé ET que abortIfOverBudget est activé.
    /// L'appelant doit vérifier le retour et interrompre le traitement si false.
    bool endTask(
        const std::string& taskName,
        const std::chrono::steady_clock::time_point& start,
        SchedulerTickReport& report);

    void endTick(SchedulerTickReport& report);

    double frameBudgetMs() const;

private:
    /// Trouve le targetMs enregistré pour un nom de tâche donné.
    /// Retourne frameBudgetMs_ si la tâche n'est pas trouvée.
    double targetMsForTask(const std::string& taskName) const;

    /// Retourne true si abortIfOverBudget est activé pour cette tâche.
    bool abortFlagForTask(const std::string& taskName) const;

private:
    double frameBudgetMs_;
    std::vector<SchedulerTaskBudget> tasks_;
    std::chrono::steady_clock::time_point tickStart_;
    double previousTickMs_{0.0};
};

} // namespace dnxr
