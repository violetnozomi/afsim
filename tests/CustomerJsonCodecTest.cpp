#include "NrmCustomerJsonCodec.hpp"
#include "NrmCustomerPlanIngest.hpp"

#include "nrm/NetworkPlanRepository.hpp"
#include "nrm/NetworkProfileRepository.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
class RejectAllValidation final : public WkNrm::CustomerJsonValidationLayer
{
public:
   WkNrm::CustomerJsonDecodeResult Validate(const QByteArray&) const override
   {
      WkNrm::CustomerJsonDecodeResult result;
      result.errors.push_back(
         {"TEST_VALIDATION_REJECTED", "/", "统一运行时校验层拒绝消息"});
      return result;
   }
};

QByteArray ValidEnvelope()
{
   return R"({"schema":"nrm.customer.assessment_request.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{}})";
}

QByteArray ReadFixture(const char* aName)
{
   std::ifstream input(std::string(NRM_SOURCE_DIR) +
                       "/schemas/customer/v1/examples/" + aName);
   std::ostringstream text;
   text << input.rdbuf();
   assert(input.good() || input.eof());
   return QByteArray::fromStdString(text.str());
}

QJsonObject FixtureRoot(const char* aName)
{
   QJsonParseError error;
   const QJsonDocument document = QJsonDocument::fromJson(ReadFixture(aName), &error);
   assert(error.error == QJsonParseError::NoError);
   assert(document.isObject());
   return document.object();
}

QByteArray Compact(const QJsonObject& aRoot)
{
   return QJsonDocument(aRoot).toJson(QJsonDocument::Compact);
}

void ExpectFirstError(const WkNrm::CustomerJsonDecodeResult& aResult,
                      const char* aCode, const char* aPath)
{
   assert(!aResult.valid);
   assert(!aResult.errors.empty());
   if (aResult.errors.front().code != aCode ||
       aResult.errors.front().path != aPath)
      std::cerr << "expected " << aCode << " " << aPath << ", got "
                << aResult.errors.front().code << " "
                << aResult.errors.front().path << '\n';
   assert(aResult.errors.front().code == aCode);
   assert(aResult.errors.front().path == aPath);
}
}

int main()
{
   WkNrm::CustomerJsonCodec codec;
   assert(std::string(codec.AdapterId()) == "nrm.customer.json.v1");
   assert(std::string(codec.AdapterVersion()) == "1.0");
   assert(codec.SupportedRequestSchemas().size() == 8);
   assert(codec.SupportedResponseSchemas().size() == 5);
   const WkNrm::CustomerJsonDecodeResult valid = codec.Inspect(ValidEnvelope());
   assert(valid.valid);
   assert(valid.envelope.schema == "nrm.customer.assessment_request.v1");
   assert(valid.envelope.messageId == "msg-001");
   assert(valid.envelope.source == "CUSTOMER");
   const RejectAllValidation rejectAll;
   const WkNrm::CustomerJsonCodec rejectingCodec(rejectAll);
   ExpectFirstError(rejectingCodec.Inspect(ValidEnvelope()),
                    "TEST_VALIDATION_REJECTED", "/");

   ExpectFirstError(codec.Inspect("{"), "INVALID_JSON", "/");
   ExpectFirstError(codec.Inspect("[]"), "SCHEMA_VALIDATION_FAILED", "/");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER"})"),
      "REQUIRED_FIELD_MISSING", "/data");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.unknown.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{}})"),
      "SCHEMA_UNSUPPORTED", "/schema");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"msg-unknown-field","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{"unexpected":true}})"),
      "SCHEMA_VALIDATION_FAILED", "/data/unexpected");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"bad id!","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{}})"),
      "SCHEMA_VALIDATION_FAILED", "/messageId");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"wrong-source","timestamp":"2026-08-13T10:00:00+08:00","source":"NRM","data":{}})"),
      "SCHEMA_VALIDATION_FAILED", "/source");

   WkNrm::CustomerJsonEnvelope envelope;
   envelope.messageId = "msg-009";
   const QByteArray errorJson = codec.EncodeError(
      envelope, {{"REQUIRED_FIELD_MISSING", "/data/platformId", "缺少平台编号"}});
   QJsonParseError parseError;
   const QJsonDocument document = QJsonDocument::fromJson(errorJson, &parseError);
   assert(parseError.error == QJsonParseError::NoError);
   const QJsonObject errorRoot = document.object();
   assert(errorRoot.value("schema").toString() == "nrm.customer.error.v1");
   assert(errorRoot.value("messageId").toString() == "msg-009");
   assert(errorRoot.value("source").toString() == "NRM");
   const QJsonArray errors = errorRoot.value("data").toObject().value("errors").toArray();
   assert(errors.size() == 1);
   assert(errors.at(0).toObject().value("path").toString() == "/data/platformId");

   nrm::NavigationSample navigation;
   assert(codec.DecodeNavigation(ReadFixture("navigation-report.example.json"),
                                 navigation).valid);
   assert(navigation.platformId == "aircraft-01");
   assert(navigation.platformName == "aircraft-01");
   assert(navigation.mode == nrm::NavigationMode::cGPS_ACTIVE);
   assert(navigation.truthLatitudeDeg.valid);
   assert(navigation.truthLatitudeDeg.unit == "deg");
   assert(navigation.totalPositionErrorM.valid);

   nrm::NavigationSample minimalNavigation;
   const QByteArray minimalNavigationJson =
      R"({"schema":"nrm.customer.navigation_report.v1","messageId":"nav-minimal","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{"runId":"run-demo","simTime":11.0,"platformId":"aircraft-02","navigationType":"INS","truthPosition":{"latitudeDeg":34.1,"longitudeDeg":108.9,"altitudeM":8000},"perceivedPosition":{"latitudeDeg":34.1,"longitudeDeg":108.9,"altitudeM":8000}}})";
   assert(codec.DecodeNavigation(minimalNavigationJson, minimalNavigation).valid);
   assert(minimalNavigation.navigationType == "INS");
   assert(!minimalNavigation.totalPositionErrorM.valid);

   {
      QJsonObject root = FixtureRoot("navigation-report.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("truthPosition", QJsonObject{{"x", 34.1}, {"y", 108.9},
                                                {"z", 8000.0}});
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNavigation(Compact(root), navigation),
                       "SCHEMA_VALIDATION_FAILED", "/data/truthPosition/x");
   }
   {
      QJsonObject root = FixtureRoot("navigation-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonObject truth = data.value("truthPosition").toObject();
      truth.insert("latitudeDeg", "34.1");
      data.insert("truthPosition", truth);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNavigation(Compact(root), navigation),
                       "SCHEMA_VALIDATION_FAILED", "/data/truthPosition");
   }

   nrm::EnvironmentSnapshot environment;
   nrm::EnvironmentContext environmentContext;
   assert(codec.DecodeEnvironment(ReadFixture("environment-report.example.json"),
                                  environment, environmentContext).valid);
   assert(environment.valid);
   assert(environment.weather.available);
   assert(environment.weather.windSpeedMps.value == 8.0);
   assert(environment.weather.cloudPercent.value == 40.0);
   assert(environment.interference.capacityScale.value == 0.9);
   assert(environment.interference.affectedLinkIds.size() == 1);
   assert(environment.interference.affectedLinkIds.front() == "link-l16-1");
   assert(environmentContext.applicationMode ==
          nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT);
   assert(environmentContext.applyParameterizedEffects);
   assert(environmentContext.customerProvidedDomains.size() == 3);
   assert(std::find(environmentContext.customerProvidedDomains.begin(),
                    environmentContext.customerProvidedDomains.end(),
                    nrm::EnvironmentDomain::cTERRAIN) !=
          environmentContext.customerProvidedDomains.end());
   assert(std::find(environmentContext.customerProvidedDomains.begin(),
                    environmentContext.customerProvidedDomains.end(),
                    nrm::EnvironmentDomain::cWEATHER) !=
          environmentContext.customerProvidedDomains.end());
   assert(std::find(environmentContext.customerProvidedDomains.begin(),
                    environmentContext.customerProvidedDomains.end(),
                    nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE) !=
          environmentContext.customerProvidedDomains.end());
   for (const auto& mode : {
           std::pair<const char*, nrm::EnvironmentApplicationMode>{
              "INFORMATION_ONLY",
              nrm::EnvironmentApplicationMode::cINFORMATION_ONLY},
           {"ALREADY_INCLUDED",
            nrm::EnvironmentApplicationMode::cALREADY_INCLUDED},
           {"CANDIDATE_ADJUSTMENT",
            nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT}})
   {
      QJsonObject root = FixtureRoot("environment-report.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("applicationMode", mode.first);
      root.insert("data", data);
      nrm::EnvironmentSnapshot decodedEnvironment;
      nrm::EnvironmentContext decodedContext;
      assert(codec.DecodeEnvironment(Compact(root), decodedEnvironment,
                                     decodedContext).valid);
      assert(decodedContext.applicationMode == mode.second);
      assert(decodedContext.applyParameterizedEffects ==
             (mode.second == nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT));
   }
   {
      QJsonObject root = FixtureRoot("environment-report.example.json");
      QJsonObject data = root.value("data").toObject();
      data.remove("terrain");
      data.remove("interference");
      root.insert("data", data);
      nrm::EnvironmentSnapshot partialEnvironment;
      nrm::EnvironmentContext partialContext;
      assert(codec.DecodeEnvironment(Compact(root), partialEnvironment,
                                     partialContext).valid);
      assert(partialContext.customerProvidedDomains.size() == 1);
      assert(partialContext.customerProvidedDomains.front() ==
             nrm::EnvironmentDomain::cWEATHER);
   }
   {
      QJsonObject root = FixtureRoot("environment-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonObject weather = data.value("weather").toObject();
      weather.insert("windSpeedMps", "fast");
      data.insert("weather", weather);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeEnvironment(Compact(root), environment,
                                               environmentContext),
                       "SCHEMA_VALIDATION_FAILED", "/data/weather/windSpeedMps");
   }
   {
      QJsonObject root = FixtureRoot("environment-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonObject terrain = data.value("terrain").toObject();
      terrain.insert("schemaUndefinedField", true);
      data.insert("terrain", terrain);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeEnvironment(Compact(root), environment,
                                               environmentContext),
                       "SCHEMA_VALIDATION_FAILED",
                       "/data/terrain/schemaUndefinedField");
   }

   nrm::ResourceSnapshot resources;
   assert(codec.DecodeResources(ReadFixture("resource-report.example.json"),
                                resources).valid);
   assert(resources.networks.size() == 2);
   assert(resources.endpoints.size() == 3);
   assert(resources.links.size() == 1);
   assert(resources.endpoints.front().networkId == "net-l16");
   assert(resources.endpoints.front().platformId == "fighter-01");
   assert(resources.links.front().networkId == "net-l16");
   assert(resources.links.front().bandwidthBps.value == 238000.0);
   assert(resources.operationalArea.areaId == "demo-area");
   assert(resources.operationalArea.points.size() == 4);
   assert(resources.platformAttitudes.size() == 1);
   assert(resources.platformAttitudes.front().platformName == "fighter-01");
   assert(resources.links.front().protocolResource.kind == "TIMESLOT");
   assert(resources.links.front().protocolResource.capacity == 16);
   assert(resources.links.front().protocolResource.used == 4);
   assert(resources.links.front().coverage.maximumRangeM.value == 500000.0);
   assert(resources.links.front().supportedBusinessTypes.size() == 2);
   assert(resources.alarms.size() == 1);
   assert(resources.alarms.front().reasonCode == "QUEUE_HIGH");
   assert(resources.resourceProxies.size() == 1);
   assert(resources.resourceProxies.front().proxyId == "proxy-link16");
   assert(resources.routes.size() == 1);
   assert(resources.routes.front().hops.size() == 2);
   assert(resources.flows.size() == 1);
   assert(resources.flows.front().businessType == "C2");
   assert(resources.gateways.size() == 1);
   assert(resources.gateways.front().platformId == "command-01");

   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray networks = data.value("networks").toArray();
      QJsonObject network = networks.at(0).toObject();
      network.insert("name", "");
      networks.replace(0, network);
      data.insert("networks", networks);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResources(Compact(root), resources),
                       "SCHEMA_VALIDATION_FAILED", "/data/networks");
   }

   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray networks = data.value("networks").toArray();
      QJsonObject secondNetwork = networks.at(0).toObject();
      secondNetwork.insert("networkId", "net-l16-backup");
      secondNetwork.insert("name", "Link-16 Backup");
      networks.push_back(secondNetwork);
      data.insert("networks", networks);

      QJsonArray members = data.value("members").toArray();
      QJsonObject backupA = members.at(0).toObject();
      backupA.insert("memberId", "ep-backup-a");
      backupA.insert("platformId", "backup-a");
      backupA.insert("networkId", "net-l16-backup");
      members.push_back(backupA);
      QJsonObject backupB = members.at(1).toObject();
      backupB.insert("memberId", "ep-backup-b");
      backupB.insert("platformId", "backup-b");
      backupB.insert("networkId", "net-l16-backup");
      members.push_back(backupB);
      data.insert("members", members);

      QJsonArray links = data.value("links").toArray();
      QJsonObject backupLink = links.at(0).toObject();
      backupLink.insert("linkId", "link-l16-backup");
      backupLink.insert("networkId", "net-l16-backup");
      backupLink.insert("sourceMemberId", "ep-backup-a");
      backupLink.insert("destinationMemberId", "ep-backup-b");
      links.push_back(backupLink);
      data.insert("links", links);
      root.insert("data", data);
      nrm::ResourceSnapshot separated;
      assert(codec.DecodeResources(Compact(root), separated).valid);
      assert(separated.networks.size() == 3);
      assert(separated.networks.at(0).endpointCount == 2);
      assert(separated.networks.at(1).endpointCount == 1);
      assert(separated.networks.at(2).endpointCount == 2);
      assert(separated.networks.at(0).activeLinks == 1);
      assert(separated.networks.at(1).activeLinks == 0);
      assert(separated.networks.at(2).activeLinks == 1);
      assert(separated.endpoints.at(3).networkId == "net-l16-backup");
      assert(separated.links.at(1).networkId == "net-l16-backup");

      QJsonObject crossNetworkRoot = root;
      QJsonObject crossNetworkData = crossNetworkRoot.value("data").toObject();
      QJsonArray crossNetworkLinks = crossNetworkData.value("links").toArray();
      QJsonObject crossNetworkLink = crossNetworkLinks.at(0).toObject();
      crossNetworkLink.insert("destinationMemberId", "ep-backup-a");
      crossNetworkLinks.replace(0, crossNetworkLink);
      crossNetworkData.insert("links", crossNetworkLinks);
      crossNetworkRoot.insert("data", crossNetworkData);
      ExpectFirstError(codec.DecodeResources(Compact(crossNetworkRoot), separated),
                       "RESOURCE_SEMANTIC_INVALID", "/data/links/0/networkId");
   }

   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray members = data.value("members").toArray();
      QJsonObject member = members.at(0).toObject();
      member.insert("state", "BROKEN_ENUM");
      members.replace(0, member);
      data.insert("members", members);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResources(Compact(root), resources),
                       "SCHEMA_VALIDATION_FAILED", "/data/members/state");
   }
   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray links = data.value("links").toArray();
      QJsonObject link = links.at(0).toObject();
      link.insert("pdrPercent", 101.0);
      links.replace(0, link);
      data.insert("links", links);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResources(Compact(root), resources),
                       "SCHEMA_VALIDATION_FAILED", "/data/links/pdrPercent");
   }
   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray links = data.value("links").toArray();
      QJsonObject link = links.at(0).toObject();
      QJsonObject protocol = link.value("protocolResource").toObject();
      protocol.insert("capacity", -1);
      link.insert("protocolResource", protocol);
      links.replace(0, link);
      data.insert("links", links);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResources(Compact(root), resources),
                       "SCHEMA_VALIDATION_FAILED", "/data/links/protocolResource");
   }
   {
      QJsonObject root = FixtureRoot("resource-report.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray links = data.value("links").toArray();
      QJsonObject link = links.at(0).toObject();
      QJsonObject protocol = link.value("protocolResource").toObject();
      protocol.insert("capacity", 4);
      protocol.insert("used", 5);
      link.insert("protocolResource", protocol);
      links.replace(0, link);
      data.insert("links", links);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResources(Compact(root), resources),
                       "RESOURCE_SEMANTIC_INVALID", "/data/links/0/protocolResource");
   }

   WkNrm::CustomerProviderHello hello;
   assert(codec.DecodeProviderHello(ReadFixture("provider-hello.example.json"),
                                    hello).valid);
   assert(hello.providerId == "customer-afsim-link-adapter");
   assert(hello.softwareVersion == "1.0.0");
   assert(hello.supportedSchemas.size() == 2);
   assert(hello.supportedNetworkTypes.size() == 2);
   {
      QJsonObject root = FixtureRoot("provider-hello.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray schemas = data.value("supportedSchemas").toArray();
      schemas.push_back(schemas.at(0));
      data.insert("supportedSchemas", schemas);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeProviderHello(Compact(root), hello),
                       "SCHEMA_VALIDATION_FAILED", "/data/supportedSchemas");
   }

   WkNrm::CustomerJsonEnvelope ackEnvelope;
   ackEnvelope.messageId = "resource-001";
   const QJsonObject ackRoot = QJsonDocument::fromJson(
      codec.EncodeIngestAck(ackEnvelope, WkNrm::CustomerIngestStatus::cACCEPTED,
                            "资源快照已接收")).object();
   assert(ackRoot.value("schema") == "nrm.customer.ingest_ack.v1");
   assert(ackRoot.value("messageId") == "resource-001");
   assert(ackRoot.value("data").toObject().value("status") == "ACCEPTED");

   const nrm::ResourceSnapshot preserved = resources;
   const auto invalidResource = codec.DecodeResources(
      R"({"schema":"nrm.customer.resource_report.v1","messageId":"x","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{"runId":"r","simTime":1,"networks":[],"members":[],"links":[{"linkId":"l","networkId":"missing","sourceMemberId":"a","destinationMemberId":"b","state":"ONLINE"}]}})",
      resources);
   assert(!invalidResource.valid);
   assert(resources.networks.size() == preserved.networks.size());
   assert(resources.endpoints.size() == preserved.endpoints.size());
   assert(resources.links.size() == preserved.links.size());

   nrm::AssessmentTask task;
   assert(codec.DecodeAssessment(ReadFixture("assessment-request.example.json"),
                                 task).valid);
   assert(task.taskId == "task-001");
   assert(task.sourcePlatform == "fighter-01");
   assert(task.destinationPlatform == "command-01");
   assert(task.businessType == "C2");
   assert(task.requiredBandwidthBps == 64000.0);
   assert(task.maximumDelayMs == 300.0);
   assert(task.requireDelayMetricForFeasibility);
   assert(task.minimumPdrPercent == 90.0);
   assert(task.allowedNetworks.size() == 1);
   assert(task.allowedNetworks.front() == nrm::NetworkType::cLINK16);
   {
      QJsonObject root = FixtureRoot("assessment-request.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("maximumDelayMs", 0.0);
      root.insert("data", data);
      nrm::AssessmentTask unconstrainedTask;
      assert(codec.DecodeAssessment(Compact(root), unconstrainedTask).valid);
      assert(unconstrainedTask.maximumDelayMs == 0.0);
      assert(!unconstrainedTask.requireDelayMetricForFeasibility);
      data.insert("maximumDelayMs", 100.0);
      root.insert("data", data);
      nrm::AssessmentTask constrainedTask;
      assert(codec.DecodeAssessment(Compact(root), constrainedTask).valid);
      assert(constrainedTask.maximumDelayMs == 100.0);
      assert(constrainedTask.requireDelayMetricForFeasibility);
      data.insert("maximumDelayMs", -1.0);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeAssessment(Compact(root), unconstrainedTask),
                       "SCHEMA_VALIDATION_FAILED", "/data");
   }
   {
      QJsonObject root = FixtureRoot("assessment-request.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray networks = data.value("allowedNetworks").toArray();
      networks.push_back(networks.at(0));
      data.insert("allowedNetworks", networks);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeAssessment(Compact(root), task),
                       "SCHEMA_VALIDATION_FAILED", "/data/allowedNetworks");
   }

   nrm::ResourceDemandSet demandSet;
   assert(codec.DecodeResourceDemands(
      R"({"schema":"nrm.customer.resource_demand_request.v1","messageId":"demand-001","timestamp":"2026-08-17T10:00:00+08:00","source":"CUSTOMER","data":{"demandSetId":"mission-demands","revision":1,"demands":[{"demandId":"video-01","sourcePlatformId":"survey_uav_1","destinationPlatformId":"data_processing_center","businessType":"ISR_VIDEO","requiredBandwidthBps":5000000,"maximumDelayMs":2000,"minimumPdrPercent":95,"allowedNetworks":["CDL","SATCOM"],"missionStage":"SURVEILLANCE","businessTrafficBps":4500000,"maximumDistanceM":300000,"minimumNetworkSize":2}]}})",
      demandSet).valid);
   assert(demandSet.demandSetId == "mission-demands");
   assert(demandSet.revision == 1);
   assert(demandSet.configVersion.empty());
   assert(demandSet.demands.size() == 1);
   assert(demandSet.demands.front().demandSetId == demandSet.demandSetId);
   assert(demandSet.demands.front().allowedNetworks.size() == 2);
   assert(demandSet.demands.front().valid);
   {
      QJsonObject root = FixtureRoot("resource-demand-request.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray demands = data.value("demands").toArray();
      QJsonObject demand = demands.at(0).toObject();
      demand.insert("maximumDelayMs", 0.0);
      demands.replace(0, demand);
      data.insert("demands", demands);
      root.insert("data", data);
      nrm::ResourceDemandSet unconstrainedDemands;
      assert(codec.DecodeResourceDemands(Compact(root), unconstrainedDemands).valid);
      assert(unconstrainedDemands.demands.front().maximumDelayMs == 0.0);
      demand.insert("maximumDelayMs", -1.0);
      demands.replace(0, demand);
      data.insert("demands", demands);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResourceDemands(Compact(root), unconstrainedDemands),
                       "SCHEMA_VALIDATION_FAILED", "/data/demands");
   }
   {
      QJsonObject root = FixtureRoot("resource-demand-request.example.json");
      nrm::ResourceDemandSet decodedDemands;
      assert(codec.DecodeResourceDemands(Compact(root), decodedDemands).valid);
      assert(decodedDemands.demands.size() == 2);
      assert(decodedDemands.demands.at(1).missionStage == "UNSPECIFIED");
   }

   const nrm::ResourceDemandSet preservedDemandSet = demandSet;
   assert(!codec.DecodeResourceDemands(
      R"({"schema":"nrm.customer.resource_demand_request.v1","messageId":"demand-bad","timestamp":"2026-08-17T10:00:00+08:00","source":"CUSTOMER","data":{"demandSetId":"mission-demands","revision":1,"demands":[{"demandId":"bad","sourcePlatformId":"same","destinationPlatformId":"same","businessType":"C2","requiredBandwidthBps":1,"maximumDelayMs":1,"minimumPdrPercent":90,"allowedNetworks":["LINK16"]}]}})",
      demandSet).valid);
   assert(demandSet.demandSetId == preservedDemandSet.demandSetId);

   const nrm::AssessmentTask preservedTask = task;
   assert(!codec.DecodeAssessment(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"bad-task","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{"taskId":"bad","sourcePlatformId":"same","destinationPlatformId":"same","businessType":"C2","requiredBandwidthBps":1,"maximumDelayMs":1,"minimumPdrPercent":90,"allowedNetworks":["LINK16"]}})",
      task).valid);
   assert(task.taskId == preservedTask.taskId);

   nrm::NetworkPlanDocument plan;
   assert(codec.DecodeNetworkPlan(ReadFixture("network-plan.example.json"), plan).valid);
   assert(plan.planId == "four-network-demo");
   assert(plan.revision == 1);
   assert(plan.allocations.size() == 4);
   assert(plan.demands.size() == 1);
   assert(plan.valid);
   assert(plan.configVersion.empty());
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray demands = data.value("demands").toArray();
      QJsonObject demand = demands.at(0).toObject();
      demand.insert("maximumDelayMs", 0.0);
      demands.replace(0, demand);
      data.insert("demands", demands);
      root.insert("data", data);
      nrm::NetworkPlanDocument unconstrainedPlan;
      assert(codec.DecodeNetworkPlan(Compact(root), unconstrainedPlan).valid);
      assert(unconstrainedPlan.demands.front().maximumDelayMs == 0.0);
      demand.insert("maximumDelayMs", -1.0);
      demands.replace(0, demand);
      data.insert("demands", demands);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), unconstrainedPlan),
                       "SCHEMA_VALIDATION_FAILED", "/data/demands");
   }
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("allocations", QJsonArray{});
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), plan),
                       "SCHEMA_VALIDATION_FAILED", "/data");
   }
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("demands", QJsonArray{});
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), plan),
                       "SCHEMA_VALIDATION_FAILED", "/data");
   }
   {
      QJsonObject root = FixtureRoot("resource-demand-request.example.json");
      QJsonObject data = root.value("data").toObject();
      data.insert("demands", QJsonArray{});
      root.insert("data", data);
      ExpectFirstError(codec.DecodeResourceDemands(Compact(root), demandSet),
                       "SCHEMA_VALIDATION_FAILED", "/data");
   }
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray allocations = data.value("allocations").toArray();
      QJsonObject allocation = allocations.at(0).toObject();
      const QJsonArray originalMembers = allocation.value("members").toArray();
      allocation.insert("members", QJsonArray{originalMembers.at(0)});
      allocations.replace(0, allocation);
      data.insert("allocations", allocations);
      root.insert("data", data);
      nrm::NetworkPlanDocument singleMemberPlan;
      assert(codec.DecodeNetworkPlan(Compact(root), singleMemberPlan).valid);
      allocation.insert("members", QJsonArray{});
      allocations.replace(0, allocation);
      data.insert("allocations", allocations);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), plan),
                       "SCHEMA_VALIDATION_FAILED", "/data/allocations");
   }
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray allocations = data.value("allocations").toArray();
      QJsonObject allocation = allocations.at(0).toObject();
      QJsonArray members = allocation.value("members").toArray();
      members.push_back(members.at(0));
      allocation.insert("members", members);
      allocations.replace(0, allocation);
      data.insert("allocations", allocations);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), plan),
                       "SCHEMA_VALIDATION_FAILED", "/data/allocations/members");
   }
   {
      nrm::NetworkPlanRepository repository;
      nrm::NetworkPlanDocument invalidPlan = plan;
      invalidPlan.configVersion =
         nrm::NetworkProfileRepository::BuiltInDemo().ConfigVersion();
      invalidPlan.allocations.push_back(invalidPlan.allocations.front());
      WkNrm::CustomerJsonDecodeResult decoded;
      decoded.valid = true;
      assert(!WkNrm::AcceptDecodedNetworkPlan(decoded, invalidPlan, repository));
      ExpectFirstError(decoded, "PLAN_REJECTED", "/data/allocations");
      assert(!repository.HasCurrentPlan());
   }
   {
      QJsonObject root = FixtureRoot("network-plan.example.json");
      QJsonObject data = root.value("data").toObject();
      QJsonArray demands = data.value("demands").toArray();
      QJsonObject demand = demands.at(0).toObject();
      demand.insert("payloadBits", -1.0);
      demands.replace(0, demand);
      data.insert("demands", demands);
      root.insert("data", data);
      ExpectFirstError(codec.DecodeNetworkPlan(Compact(root), plan),
                       "SCHEMA_VALIDATION_FAILED", "/data/demands");
   }

   nrm::NetworkPlanChange change;
   assert(codec.DecodeMembership(ReadFixture("membership-request.example.json"),
                                 change).valid);
   assert(change.changeId == "join-001");
   assert(change.changeType == nrm::PlanChangeType::cJOIN);
   assert(change.allocationId == "allocation-link16");
   assert(change.platformId == "l16_relay_1");

   nrm::AssessmentResult assessment;
   assessment.taskId = "task-001";
   assessment.reachable = true;
   assessment.canEstablish = true;
   assessment.canComplete = false;
   assessment.primaryRoute = {"fighter-01", "relay-01", "command-01"};
   assessment.bandwidthMarginBps.value = -1000.0;
   assessment.bandwidthMarginBps.unit = "bit/s";
   assessment.bandwidthMarginBps.valid = true;
   assessment.reasons = {nrm::AssessmentReason::cBANDWIDTH_MARGIN_NEGATIVE};
   assessment.recommendations = {"改用备用链路"};
   const QJsonObject assessmentRoot = QJsonDocument::fromJson(
      codec.EncodeAssessment(valid.envelope, assessment)).object();
   assert(assessmentRoot.value("schema") == "nrm.customer.assessment_response.v1");
   assert(assessmentRoot.value("data").toObject().value("taskId") == "task-001");
   assert(assessmentRoot.value("data").toObject().value("reasonCodes").toArray().size() == 1);

   nrm::ResourceDemandBatchResult demandBatch;
   demandBatch.demandSetId = "mission-demands";
   demandBatch.revision = 1;
   demandBatch.snapshotVersion = 110;
   demandBatch.totalCount = 1;
   demandBatch.unsatisfiedCount = 1;
   nrm::ResourceDemandMatchResult demandResult;
   demandResult.demandId = "video-01";
   demandResult.status = nrm::DemandMatchStatus::cUNSATISFIED;
   demandResult.reasons.push_back(nrm::ResourceDemandReason::cBANDWIDTH_NOT_MET);
   nrm::PlanningRecommendation recommendation;
   recommendation.type = nrm::RecommendationType::cROUTE;
   recommendation.status = nrm::RecommendationStatus::cAVAILABLE;
   recommendation.value = "survey_uav_1->airborne_relay->data_processing_center";
   recommendation.reason = nrm::ResourceDemandReason::cNONE;
   demandResult.recommendations.push_back(recommendation);
   demandBatch.results.push_back(demandResult);
   const QJsonObject demandResponseRoot = QJsonDocument::fromJson(
      codec.EncodeResourceDemandResult(valid.envelope, demandBatch)).object();
   assert(demandResponseRoot.value("schema") ==
          "nrm.customer.resource_demand_response.v1");
   const QJsonObject demandResponseData =
      demandResponseRoot.value("data").toObject();
   assert(demandResponseData.value("demandSetId") == "mission-demands");
   assert(demandResponseData.value("results").toArray().size() == 1);
   assert(demandResponseData.value("results").toArray().at(0).toObject()
             .value("recommendations").toArray().size() == 1);

   nrm::NetworkPlanEvaluationResult evaluation;
   evaluation.planId = plan.planId;
   evaluation.revision = plan.revision;
   evaluation.validation.passed = true;
   evaluation.overallStatus = nrm::PlanEvaluationStatus::cFAIL;
   evaluation.resultingState = nrm::NetworkPlanState::cREJECTED;
   nrm::PlanDemandEvaluation failedDemand;
   failedDemand.demandId = "demand-001";
   failedDemand.status = nrm::PlanEvaluationStatus::cFAIL;
   failedDemand.reasons.push_back(nrm::PlanValidationReason::cPDR_NOT_MET);
   failedDemand.recommendations.push_back("改用PDR更高的备用链路。");
   evaluation.demands.push_back(failedDemand);
   const QJsonObject planResultRoot = QJsonDocument::fromJson(
      codec.EncodePlanResult(valid.envelope, evaluation)).object();
   assert(planResultRoot.value("schema") == "nrm.customer.network_plan_result.v1");
   assert(planResultRoot.value("data").toObject().value("validationPassed").toBool());
   assert(planResultRoot.value("data").toObject().value("recommendations").toArray().size() == 1);
   return 0;
}
