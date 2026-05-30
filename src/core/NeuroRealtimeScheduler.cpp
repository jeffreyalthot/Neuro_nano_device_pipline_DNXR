// =============================================================================
// DOSSIER : src/core/
// FICHIER : NeuroRealtimeScheduler.cpp
//
// AMÉLIORATION APPLIQUÉE :
//   [3] Scheduler temps réel avec détection de dépassement de budget actif.
//
//       PROBLÈME ORIGINAL :
//         endTask() mesurait la durée mais ne faisait rien si le budget
//         était dépassé. Aucun signal n'était émis vers l'appelant.
//         Résultat : si "inference" dépassait 5 ms, "render" était retardé
//         silencieusement → cybersickness garanti en immersion profonde.
//
//       CORRECTION :
//         • endTask() retourne false si la tâche dépasse son targetMs
//           ET que abortIfOverBudget est activé.
//         • Le rapport comptabilise overBudgetTaskCount et lastAbortedTask.
//         • L'appelant (RuntimeController::runSingleCycle) peut alors décider
//           d'interrompre le rendu ou de passer en mode dégradé.
//         • Les tâches sans abortIfOverBudget continuent comme avant
//           (compatibilité ascendante).
// =============================================================================

#include "NeuroRealtimeScheduler.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

NeuroRealtimeScheduler::NeuroRealtimeScheduler(double frameBudgetMs)
    : frameBudgetMs_(std::max(1.0, frameBudgetMs)) {}

void NeuroRealtimeScheduler::registerTask(const SchedulerTaskBudget& task) {
    tasks_.push_back(task);
    std::sort(tasks_.begin(), tasks_.end(),
              [](const auto& a, const auto& b) { return a.priority > b.priority; });
}

SchedulerTickReport NeuroRealtimeScheduler::beginTick() {
    tickStart_ = std::chrono::steady_clock::now();
    return SchedulerTickReport{};
}

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 3] endTask avec détection active de dépassement
//
// Retour : true  → tâche dans son budget, continuer normalement
//          false → tâche hors budget ET abortIfOverBudget activé,
//                  l'appelant doit interrompre / passer en mode dégradé
// ─────────────────────────────────────────────────────────────────────────────
bool NeuroRealtimeScheduler::endTask(
    const std::string& taskName,
    const std::chrono::steady_clock::time_point& start,
    SchedulerTickReport& report)
{
    const auto now = std::chrono::steady_clock::now();
    const double taskMs = std::chrono::duration<double, std::milli>(now - start).count();

    report.maxTaskMs = std::max(report.maxTaskMs, taskMs);

    const double budget    = targetMsForTask(taskName);
    const bool   overBudget= (taskMs > budget);
    const bool   shouldAbort = overBudget && abortFlagForTask(taskName);

    if (overBudget) {
        ++report.overBudgetTaskCount;
    }

    if (shouldAbort) {
        report.lastAbortedTask = taskName;
        return false;  // signal d'abandon vers l'appelant
    }

    return true;  // dans le budget ou abandon non demandé
}

void NeuroRealtimeScheduler::endTick(SchedulerTickReport& report) {
    const auto now = std::chrono::steady_clock::now();
    report.totalMs      = std::chrono::duration<double, std::milli>(now - tickStart_).count();
    report.jitterMs     = std::abs(report.totalMs - previousTickMs_);
    report.withinBudget = (report.totalMs <= frameBudgetMs_);
    previousTickMs_     = report.totalMs;
}

double NeuroRealtimeScheduler::frameBudgetMs() const {
    return frameBudgetMs_;
}

// --- Helpers privés ---

double NeuroRealtimeScheduler::targetMsForTask(const std::string& taskName) const {
    for (const auto& t : tasks_) {
        if (t.name == taskName) return t.targetMs;
    }
    return frameBudgetMs_;  // budget global par défaut si tâche inconnue
}

bool NeuroRealtimeScheduler::abortFlagForTask(const std::string& taskName) const {
    for (const auto& t : tasks_) {
        if (t.name == taskName) return t.abortIfOverBudget;
    }
    return false;
}

} // namespace dnxr
