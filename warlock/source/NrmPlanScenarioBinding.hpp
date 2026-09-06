/**
 * @file NrmPlanScenarioBinding.hpp
 * @brief Resolves an optional AFSIM scenario bound to a resource plan.
 */

#ifndef NRM_PLAN_SCENARIO_BINDING_HPP
#define NRM_PLAN_SCENARIO_BINDING_HPP

#include <QString>

namespace WkNrm
{
// New plans keep the binding inside the .nrm file as a comment directive:
// # NRM_SCENARIO "project/relative/mission.txt"
// Legacy <plan>.scenario sidecars remain readable for backward compatibility.
QString ResolvePlanScenarioPath(const QString& aPlanPath,
                                const QString& aSourceRoot);
}

#endif
