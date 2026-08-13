#include "NrmCustomerJsonCodec.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
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

void ExpectFirstError(const WkNrm::CustomerJsonDecodeResult& aResult,
                      const char* aCode, const char* aPath)
{
   assert(!aResult.valid);
   assert(!aResult.errors.empty());
   assert(aResult.errors.front().code == aCode);
   assert(aResult.errors.front().path == aPath);
}
}

int main()
{
   WkNrm::CustomerJsonCodec codec;
   const WkNrm::CustomerJsonDecodeResult valid = codec.Inspect(ValidEnvelope());
   assert(valid.valid);
   assert(valid.envelope.schema == "nrm.customer.assessment_request.v1");
   assert(valid.envelope.messageId == "msg-001");
   assert(valid.envelope.source == "CUSTOMER");

   ExpectFirstError(codec.Inspect("{"), "INVALID_JSON", "/");
   ExpectFirstError(codec.Inspect("[]"), "SCHEMA_VALIDATION_FAILED", "/");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.assessment_request.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER"})"),
      "REQUIRED_FIELD_MISSING", "/data");
   ExpectFirstError(codec.Inspect(
      R"({"schema":"nrm.customer.unknown.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{}})"),
      "SCHEMA_UNSUPPORTED", "/schema");

   WkNrm::CustomerJsonEnvelope envelope;
   envelope.messageId = "msg-009";
   const QByteArray errorJson = codec.EncodeError(
      envelope, {{"REQUIRED_FIELD_MISSING", "/data/platformId", "缺少平台编号"}});
   QJsonParseError parseError;
   const QJsonDocument document = QJsonDocument::fromJson(errorJson, &parseError);
   assert(parseError.error == QJsonParseError::NoError);
   const QJsonObject root = document.object();
   assert(root.value("schema").toString() == "nrm.customer.error.v1");
   assert(root.value("messageId").toString() == "msg-009");
   assert(root.value("source").toString() == "NRM");
   const QJsonArray errors = root.value("data").toObject().value("errors").toArray();
   assert(errors.size() == 1);
   assert(errors.at(0).toObject().value("path").toString() == "/data/platformId");

   nrm::NavigationSample navigation;
   assert(codec.DecodeNavigation(ReadFixture("navigation-report.example.json"),
                                 navigation).valid);
   assert(navigation.platformName == "aircraft-01");
   assert(navigation.mode == nrm::NavigationMode::cGPS_ACTIVE);
   assert(navigation.truthLatitudeDeg.valid);
   assert(navigation.truthLatitudeDeg.unit == "deg");
   assert(navigation.totalPositionErrorM.valid);

   nrm::EnvironmentSnapshot environment;
   nrm::EnvironmentContext environmentContext;
   assert(codec.DecodeEnvironment(ReadFixture("environment-report.example.json"),
                                  environment, environmentContext).valid);
   assert(environment.valid);
   assert(environment.weather.available);
   assert(environment.weather.windSpeedMps.value == 8.0);
   assert(environmentContext.applyParameterizedEffects);

   nrm::ResourceSnapshot resources;
   assert(codec.DecodeResources(ReadFixture("resource-report.example.json"),
                                resources).valid);
   assert(resources.networks.size() == 1);
   assert(resources.endpoints.size() == 2);
   assert(resources.links.size() == 1);
   assert(resources.links.front().bandwidthBps.value == 238000.0);

   const nrm::ResourceSnapshot preserved = resources;
   const auto invalidResource = codec.DecodeResources(
      R"({"schema":"nrm.customer.resource_report.v1","messageId":"x","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{"runId":"r","simTime":1,"networks":[],"members":[],"links":[{"linkId":"l","networkId":"missing","sourceMemberId":"a","destinationMemberId":"b","state":"ONLINE"}]}})",
      resources);
   assert(!invalidResource.valid);
   assert(resources.networks.size() == preserved.networks.size());
   assert(resources.endpoints.size() == preserved.endpoints.size());
   assert(resources.links.size() == preserved.links.size());
   return 0;
}
