#include "NrmDataContainer.hpp"

#include "nrm/NetworkProfileRepository.hpp"

#include <cassert>
#include <iostream>

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

namespace
{
QJsonObject ReadFixture(const char* aName)
{
   QFile input(QString::fromStdString(
      std::string(NRM_SOURCE_DIR) + "/schemas/customer/v1/examples/" + aName));
   assert(input.open(QIODevice::ReadOnly));
   QJsonParseError error;
   const QJsonDocument document = QJsonDocument::fromJson(input.readAll(), &error);
   assert(error.error == QJsonParseError::NoError);
   assert(document.isObject());
   return document.object();
}

QString WriteMessage(const QTemporaryDir& aDirectory, const char* aFileName,
                     QJsonObject aRoot)
{
   const QString path = aDirectory.filePath(aFileName);
   QFile output(path);
   assert(output.open(QIODevice::WriteOnly | QIODevice::Truncate));
   assert(output.write(QJsonDocument(aRoot).toJson(QJsonDocument::Compact)) > 0);
   output.close();
   return path;
}

QJsonObject WithEnvelopeAndTime(QJsonObject aRoot, const char* aMessageId,
                                double aSimTime)
{
   aRoot.insert("messageId", aMessageId);
   QJsonObject data = aRoot.value("data").toObject();
   data.insert("runId", "run-data-container");
   data.insert("simTime", aSimTime);
   aRoot.insert("data", data);
   return aRoot;
}

void RequireLoad(WkNrm::DataContainer& aData, const QString& aPath)
{
   if (!aData.LoadCustomerJson(aPath.toStdString()))
   {
      for (const auto& error : aData.LastCustomerJsonResult().errors)
         std::cerr << error.code << " " << error.path << " " << error.message
                   << '\n';
      assert(false && "customer JSON must load");
   }
}
} // namespace

int main(int argc, char** argv)
{
   QCoreApplication application(argc, argv);
   QTemporaryDir temporaryDirectory;
   assert(temporaryDirectory.isValid());
   qputenv("NRM_OUTPUT_DIR", temporaryDirectory.path().toLocal8Bit());

   WkNrm::DataContainer data;

   QJsonObject resource = WithEnvelopeAndTime(
      ReadFixture("resource-report.example.json"), "resource-e2e-1", 10.0);
   const QString resourcePath = WriteMessage(
      temporaryDirectory, "resource-1.json", resource);
   assert(data.LoadCustomerJson(resourcePath.toStdString()));
   assert(data.GetSnapshot().networks.size() == 2);
   assert(data.GetSnapshot().endpoints.front().networkId == "net-l16");

   QJsonObject navigationA = WithEnvelopeAndTime(
      ReadFixture("navigation-report.example.json"), "navigation-e2e-a", 11.0);
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "navigation-a.json", navigationA)
         .toStdString()));

   QJsonObject navigationB = WithEnvelopeAndTime(
      ReadFixture("navigation-report.example.json"), "navigation-e2e-b", 12.0);
   QJsonObject navigationBData = navigationB.value("data").toObject();
   navigationBData.insert("platformId", "aircraft-02");
   navigationB.insert("data", navigationBData);
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "navigation-b.json", navigationB)
         .toStdString()));
   assert(data.GetSnapshot().navigation.platforms.size() == 2);
   assert(data.GetSnapshot().navigation.platforms.at(0).platformId == "aircraft-01");
   assert(data.GetSnapshot().navigation.platforms.at(1).platformId == "aircraft-02");

   QJsonObject environment = WithEnvelopeAndTime(
      ReadFixture("environment-report.example.json"), "environment-e2e", 13.0);
   QJsonObject environmentData = environment.value("data").toObject();
   environmentData.insert("applicationMode", "ALREADY_INCLUDED");
   environment.insert("data", environmentData);
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "environment.json", environment)
         .toStdString()));
   assert(data.GetSnapshot().environment.valid);
   assert(data.GetSnapshot().environment.interference.capacityScale.valid);

   QJsonObject assessment = ReadFixture("assessment-request.example.json");
   assessment.insert("messageId", "assessment-e2e");
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "assessment.json", assessment)
         .toStdString()));
   assert(data.HasAssessment());
   const QJsonObject assessmentResponse =
      QJsonDocument::fromJson(data.LastCustomerJsonResponse()).object();
   assert(assessmentResponse.value("schema") ==
          "nrm.customer.assessment_response.v1");
   assert(assessmentResponse.value("data").toObject().value("taskId") ==
          "task-001");

   nrm::CapabilityRequest capabilityRequest;
   capabilityRequest.requestId = "capability-e2e";
   capabilityRequest.sourcePlatform = "fighter-01";
   capabilityRequest.destinationPlatform = "command-01";
   capabilityRequest.requiredBandwidthBps = 1000.0;
   data.QueryCapability(capabilityRequest);
   assert(data.HasCapability());

   QJsonObject plan = ReadFixture("network-plan.example.json");
   plan.insert("messageId", "plan-e2e");
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "plan.json", plan).toStdString()));
   assert(data.HasNetworkPlan());
   assert(data.GetNetworkPlan()->configVersion ==
          nrm::NetworkProfileRepository::BuiltInDemo().ConfigVersion());
   data.ValidateNetworkPlan();
   assert(data.HasPlanValidation());
   data.EvaluateNetworkPlan();
   assert(data.HasPlanEvaluation());
   data.GenerateNetworkPlanPackage(temporaryDirectory.path().toStdString());
   assert(data.HasDistributionPackage());

   // Selecting an exported copy of the already active revision is a no-op:
   // prior validation/evaluation evidence must remain visible in the UI.
   const QString samePlanPath = temporaryDirectory.filePath("same-plan.nrm");
   const nrm::PlanRepositoryResult samePlanSave =
      nrm::NetworkPlanRepository::SaveDocumentAtomic(
         *data.GetNetworkPlan(), samePlanPath.toStdString(), true);
   assert(samePlanSave.success);
   assert(data.LoadNetworkPlan(samePlanPath.toStdString()));
   assert(data.HasPlanValidation());
   assert(data.HasPlanEvaluation());
   assert(data.HasDistributionPackage());

   QJsonObject demands = ReadFixture("resource-demand-request.example.json");
   demands.insert("messageId", "demands-e2e");
   RequireLoad(data, WriteMessage(temporaryDirectory, "demands.json", demands));
   assert(data.HasDemandMatching());

   resource.insert("messageId", "resource-e2e-2");
   QJsonObject resourceData = resource.value("data").toObject();
   resourceData.insert("simTime", 14.0);
   resource.insert("data", resourceData);
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "resource-2.json", resource)
         .toStdString()));

   // Same-run resource refresh retains independent domains but invalidates all
   // results derived from the previous snapshot version.
   assert(data.GetSnapshot().navigation.platforms.size() == 2);
   assert(data.GetSnapshot().environment.valid);
   assert(!data.HasAssessment());
   assert(!data.HasCapability());
   assert(!data.HasPlanValidation());
   assert(!data.HasPlanEvaluation());
   assert(!data.HasDistributionPackage());
   assert(!data.HasDemandMatching());
   assert(data.HasDemandMatchingHistory());
   assert(!data.GetDemandMatching().results.empty());

   const std::uint64_t versionBeforeAfsim = data.GetSnapshot().snapshotVersion;
   nrm::ResourceSnapshot afsim;
   afsim.snapshotVersion = 1;
   afsim.simTime = 20.0;
   afsim.providerId = "afsim-internal";
   afsim.origin = nrm::DataOrigin::cAFSIM_INTERNAL;
   nrm::NetworkSnapshot afsimNetwork;
   afsimNetwork.networkId = "afsim-authority-network";
   afsimNetwork.networkType = nrm::NetworkType::cLINK16;
   afsim.networks.push_back(afsimNetwork);
   data.SetSnapshot(afsim);
   assert(data.GetSnapshot().networks.size() == 1);
   assert(data.GetSnapshot().networks.front().networkId ==
          "afsim-authority-network");
   assert(data.GetSnapshot().navigation.platforms.size() == 2);
   assert(data.GetSnapshot().environment.valid);
   assert(data.GetSnapshot().snapshotVersion > versionBeforeAfsim);

   const std::uint64_t versionBeforeCustomerRefresh =
      data.GetSnapshot().snapshotVersion;
   resource.insert("messageId", "resource-e2e-3");
   resourceData = resource.value("data").toObject();
   resourceData.insert("simTime", 21.0);
   resource.insert("data", resourceData);
   assert(data.LoadCustomerJson(
      WriteMessage(temporaryDirectory, "resource-3.json", resource)
         .toStdString()));
   assert(data.GetSnapshot().networks.size() == 1);
   assert(data.GetSnapshot().networks.front().networkId ==
          "afsim-authority-network");
   assert(data.GetSnapshot().navigation.platforms.size() == 2);
   assert(data.GetSnapshot().environment.valid);
   assert(data.GetSnapshot().snapshotVersion > versionBeforeCustomerRefresh);
   data.UnloadResourceDemands();
   assert(!data.HasDemandMatchingHistory());
   return 0;
}
