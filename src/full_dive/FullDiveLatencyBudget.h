#pragma once
namespace dnxr { struct FullDiveLatencyBudget { float visualBudgetMs=16, auditoryBudgetMs=12, hapticBudgetMs=8, vestibularBudgetMs=6, proprioceptionBudgetMs=8, olfactoryBudgetMs=500, gustatoryBudgetMs=750, safetyBudgetMs=5, emergencyStopBudgetMs=2; bool withinEmergency(float ms) const { return ms<=emergencyStopBudgetMs; } }; }
