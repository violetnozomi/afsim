/**
 * @file PlanScenarioBindingTest.cpp
 * @brief Verifies one-file plan-to-AFSIM scenario binding and legacy fallback.
 */

#include "NrmPlanScenarioBinding.hpp"

#include <cstdlib>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

namespace
{
void Require(bool aCondition)
{
   if (!aCondition) std::abort();
}

void WriteFile(const QString& aPath, const QString& aText)
{
   QFile file(aPath);
   Require(file.open(QIODevice::WriteOnly | QIODevice::Text));
   QTextStream stream(&file);
   stream << aText;
   stream.flush();
   Require(stream.status() == QTextStream::Ok);
}
}

int main()
{
   QTemporaryDir root;
   Require(root.isValid());
   const QString missionPath = QDir(root.path()).filePath("mission.txt");
   const QString planPath = QDir(root.path()).filePath("acceptance.nrm");
   WriteFile(missionPath, "# mission\n");
   WriteFile(planPath,
             "# NRM_SCENARIO \"mission.txt\"\n"
             "NRM_NETWORK_PLAN_V1 \"plan\" 1\n");
   Require(WkNrm::ResolvePlanScenarioPath(planPath, root.path()) ==
           QFileInfo(missionPath).canonicalFilePath());

   const QString legacyPlanPath = QDir(root.path()).filePath("legacy.nrm");
   WriteFile(legacyPlanPath, "NRM_NETWORK_PLAN_V1 \"legacy\" 1\n");
   WriteFile(legacyPlanPath + ".scenario", "mission.txt\n");
   Require(WkNrm::ResolvePlanScenarioPath(legacyPlanPath, root.path()) ==
           QFileInfo(missionPath).canonicalFilePath());

   const QString unboundPlanPath = QDir(root.path()).filePath("unbound.nrm");
   WriteFile(unboundPlanPath, "NRM_NETWORK_PLAN_V1 \"unbound\" 1\n");
   Require(WkNrm::ResolvePlanScenarioPath(unboundPlanPath, root.path()).isEmpty());
   return 0;
}
