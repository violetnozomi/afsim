#include "NrmCustomerJsonCodec.hpp"

#include <cmath>
#include <limits>
#include <map>
#include <set>

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "nrm/ResourceSnapshotValidator.hpp"

namespace
{
std::set<QString> DataKeys(const QString& aSchema)
{
   if (aSchema == "nrm.customer.navigation_report.v1")
      return {"confidence", "navigationType", "perceivedPosition", "platformId",
              "positionErrorM", "runId", "simTime", "status", "truthPosition"};
   if (aSchema == "nrm.customer.environment_report.v1")
      return {"applicationMode", "astronomy", "interference", "regionId",
              "runId", "simTime", "terrain", "weather"};
   if (aSchema == "nrm.customer.resource_report.v1")
      return {"alarms", "attitudes", "flows", "gateways", "links", "members",
              "networks", "operationalArea", "resourceProxies", "routes",
              "runId", "simTime"};
   if (aSchema == "nrm.customer.assessment_request.v1")
      return {"allowedNetworks", "businessType", "destinationPlatformId",
              "maximumDelayMs", "minimumPdrPercent", "requiredBandwidthBps",
              "sourcePlatformId", "taskId"};
   if (aSchema == "nrm.customer.resource_demand_request.v1")
      return {"demandSetId", "demands", "revision"};
   if (aSchema == "nrm.customer.network_plan.v1")
      return {"allocations", "demands", "planId", "planningDomain", "revision"};
   if (aSchema == "nrm.customer.membership_request.v1")
      return {"action", "allocationId", "planId", "platformId", "requestId"};
   if (aSchema == "nrm.customer.provider_hello.v1")
      return {"providerId", "softwareVersion", "supportedNetworkTypes",
              "supportedSchemas"};
   if (aSchema == "nrm.customer.assessment_response.v1")
      return {"backupRoute", "bandwidthMarginBps", "canComplete", "canEstablish",
              "delayMarginMs", "pdrMarginPercent", "primaryRoute", "reachable",
              "reasonCodes", "recommendations", "taskId"};
   if (aSchema == "nrm.customer.network_plan_result.v1")
      return {"evaluationStatus", "packagePath", "planId", "reasonCodes",
              "recommendations", "revision", "state", "validationPassed"};
   if (aSchema == "nrm.customer.resource_demand_response.v1")
      return {"dataInvalidCount", "demandSetId", "results", "revision",
              "satisfiedCount", "snapshotVersion", "totalCount",
              "unsatisfiedCount"};
   if (aSchema == "nrm.customer.ingest_ack.v1")
      return {"detail", "originalMessageId", "status"};
   if (aSchema == "nrm.customer.error.v1") return {"errors", "success"};
   return {};
}

bool HasUnknownKey(const QJsonObject& aObject, const std::set<QString>& aAllowed,
                   std::string& aPath, const std::string& aBasePath = "/data")
{
   for (auto it = aObject.begin(); it != aObject.end(); ++it)
   {
      if (aAllowed.count(it.key()) == 0)
      {
         aPath = aBasePath + "/" + it.key().toStdString();
         return true;
      }
   }
   return false;
}

bool HasUnknownNestedKey(const QString& aSchema, const QJsonObject& aData,
                         std::string& aPath)
{
   const auto object = [&aData, &aPath](const char* aName,
                                       const std::set<QString>& aAllowed)
   {
      const QJsonValue value = aData.value(aName);
      return value.isObject() &&
             HasUnknownKey(value.toObject(), aAllowed, aPath,
                           std::string("/data/") + aName);
   };
   const auto array = [&aData, &aPath](const char* aName,
                                      const std::set<QString>& aAllowed)
   {
      const QJsonValue value = aData.value(aName);
      if (!value.isArray()) return false;
      const QJsonArray values = value.toArray();
      for (int i = 0; i < values.size(); ++i)
         if (values.at(i).isObject() &&
             HasUnknownKey(values.at(i).toObject(), aAllowed, aPath,
                           std::string("/data/") + aName + "/" +
                              std::to_string(i))) return true;
      return false;
   };

   if (aSchema == "nrm.customer.navigation_report.v1")
   {
      return object("truthPosition", {"latitudeDeg", "longitudeDeg", "altitudeM"}) ||
             object("perceivedPosition", {"latitudeDeg", "longitudeDeg", "altitudeM"}) ||
             object("positionErrorM", {"inTrack", "crossTrack", "vertical"});
   }
   if (aSchema == "nrm.customer.environment_report.v1")
   {
      return object("terrain", {"enabled", "blockedLinkIds"}) ||
             object("weather", {"windSpeedMps", "rainRateMmPerHour", "cloudPercent"}) ||
             object("astronomy", {"julianDate", "sunElevationDeg"}) ||
             object("interference", {"affectedLinkIds", "maximumPowerDbm", "capacityScale"});
   }
   if (aSchema == "nrm.customer.network_plan.v1")
   {
      return array("allocations", {"allocationId", "networkName", "networkType",
                                    "profileId", "frequencyHz", "channelId", "subnetId",
                                    "slots", "members", "routePolicyId", "enabled"}) ||
             array("demands", {"demandId", "businessType", "sourcePlatformId",
                                "destinationPlatformId", "payloadBits",
                                "requiredBandwidthBps", "maximumDelayMs",
                                "minimumPdrPercent", "allowedNetworks"});
   }
   if (aSchema == "nrm.customer.resource_demand_request.v1")
      return array("demands", {"allowedNetworks", "businessTrafficBps",
                                "businessType", "demandId",
                                "destinationPlatformId", "maximumDelayMs",
                                "maximumDistanceM", "minimumNetworkSize",
                                "minimumPdrPercent", "missionStage",
                                "payloadBits", "requiredBandwidthBps",
                                "sourcePlatformId"});
   if (aSchema == "nrm.customer.resource_demand_response.v1")
      return array("results", {"demandId", "reasonCodes",
                                "recommendations", "status"});
   if (aSchema == "nrm.customer.error.v1")
      return array("errors", {"code", "message", "path", "retryable"});
   if (aSchema != "nrm.customer.resource_report.v1") return false;

   if (array("networks", {"networkId", "networkType", "name"}) ||
       array("members", {"memberId", "platformId", "networkId", "state",
                          "role", "position"}) ||
       array("links", {"linkId", "networkId", "sourceMemberId",
                        "destinationMemberId", "state", "bandwidthBps", "delayMs",
                        "pdrPercent", "rssiDbm", "snrDb", "berRatio", "trafficBps",
                        "queuePercent", "subnetId", "supportedBusinessTypes",
                        "protocolResource", "coverage"}) ||
       object("operationalArea", {"areaId", "validFrom", "validUntil", "points"}) ||
       array("attitudes", {"platformId", "headingDeg", "pitchDeg", "rollDeg",
                            "sampleTime"}) ||
       array("routes", {"routeId", "sourceMemberId", "destinationMemberId",
                         "hops", "active"}) ||
       array("flows", {"flowId", "businessType", "sourceMemberId",
                        "destinationMemberId", "trafficBps"}) ||
       array("gateways", {"gatewayId", "platformId", "ingressNetworkId",
                           "egressNetworkId", "enabled"}) ||
       array("resourceProxies", {"proxyId", "resourceType", "providerId",
                                  "online", "lastUpdateTime"}) ||
       array("alarms", {"alarmId", "severity", "objectId", "reasonCode",
                         "startTime", "active"})) return true;

   const QJsonArray members = aData.value("members").toArray();
   for (int i = 0; i < members.size(); ++i)
   {
      const QJsonObject member = members.at(i).toObject();
      if (member.value("position").isObject() &&
          HasUnknownKey(member.value("position").toObject(),
                        {"latitudeDeg", "longitudeDeg", "altitudeM"}, aPath,
                        "/data/members/" + std::to_string(i) + "/position")) return true;
   }
   const QJsonArray links = aData.value("links").toArray();
   for (int i = 0; i < links.size(); ++i)
   {
      const QJsonObject link = links.at(i).toObject();
      if (link.value("protocolResource").isObject() &&
          HasUnknownKey(link.value("protocolResource").toObject(),
                        {"kind", "capacity", "used"}, aPath,
                        "/data/links/" + std::to_string(i) + "/protocolResource")) return true;
      if (link.value("coverage").isObject() &&
          HasUnknownKey(link.value("coverage").toObject(), {"maximumRangeM"},
                        aPath, "/data/links/" + std::to_string(i) + "/coverage")) return true;
   }
   const QJsonObject area = aData.value("operationalArea").toObject();
   const QJsonArray points = area.value("points").toArray();
   for (int i = 0; i < points.size(); ++i)
      if (points.at(i).isObject() &&
          HasUnknownKey(points.at(i).toObject(),
                        {"latitudeDeg", "longitudeDeg", "altitudeM"}, aPath,
                        "/data/operationalArea/points/" + std::to_string(i))) return true;
   return false;
}

WkNrm::CustomerJsonDecodeResult Failure(const char* aCode, const char* aPath,
                                        const char* aMessage)
{
   WkNrm::CustomerJsonDecodeResult result;
   result.errors.push_back({aCode, aPath, aMessage});
   return result;
}

bool IsIdentifier(const QString& aValue)
{
   if (aValue.isEmpty() || aValue.size() > 64) return false;
   for (const QChar character : aValue)
   {
      const ushort value = character.unicode();
      const bool alphaNumeric =
         (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') ||
         (value >= '0' && value <= '9');
      if (!alphaNumeric && value != '_' && value != '.' && value != ':' &&
          value != '@' && value != '/' && value != '-') return false;
   }
   return true;
}

bool ValidateIdentifier(const QJsonValue& aValue, const std::string& aPath,
                        WkNrm::CustomerJsonDecodeResult& aResult)
{
   if (aValue.isString() && IsIdentifier(aValue.toString())) return true;
   aResult.errors.push_back(
      {"SCHEMA_VALIDATION_FAILED", aPath,
       "标识符必须为1至64个允许字符[A-Za-z0-9_.:@/-]"});
   return false;
}

bool ValidateOptionalIdentifier(const QJsonObject& aObject, const char* aName,
                                const std::string& aBasePath,
                                WkNrm::CustomerJsonDecodeResult& aResult)
{
   const QString name = QString::fromLatin1(aName);
   return !aObject.contains(name) ||
          ValidateIdentifier(aObject.value(name), aBasePath + "/" + aName,
                             aResult);
}

bool ValidateIdentifierArray(const QJsonValue& aValue,
                             const std::string& aPath,
                             WkNrm::CustomerJsonDecodeResult& aResult)
{
   if (!aValue.isArray()) return true;
   const QJsonArray values = aValue.toArray();
   for (int index = 0; index < values.size(); ++index)
      if (!ValidateIdentifier(values.at(index),
                              aPath + "/" + std::to_string(index), aResult))
         return false;
   return true;
}

bool ValidateObjectArrayIdentifiers(
   const QJsonObject& aData, const char* aArrayName,
   const std::vector<const char*>& aFields,
   const std::vector<const char*>& aArrayFields,
   WkNrm::CustomerJsonDecodeResult& aResult)
{
   const QJsonValue value = aData.value(QString::fromLatin1(aArrayName));
   if (!value.isArray()) return true;
   const QJsonArray objects = value.toArray();
   for (int index = 0; index < objects.size(); ++index)
   {
      if (!objects.at(index).isObject()) continue;
      const QJsonObject object = objects.at(index).toObject();
      const std::string base =
         std::string("/data/") + aArrayName + "/" + std::to_string(index);
      for (const char* field : aFields)
         if (!ValidateOptionalIdentifier(object, field, base, aResult))
            return false;
      for (const char* field : aArrayFields)
         if (!ValidateIdentifierArray(object.value(QString::fromLatin1(field)),
                                      base + "/" + field, aResult))
            return false;
   }
   return true;
}

bool ValidateSchemaIdentifiers(const QString& aSchema, const QJsonObject& aData,
                               WkNrm::CustomerJsonDecodeResult& aResult)
{
   const auto direct = [&aData, &aResult](
      std::initializer_list<const char*> aFields)
   {
      for (const char* field : aFields)
         if (!ValidateOptionalIdentifier(aData, field, "/data", aResult))
            return false;
      return true;
   };
   if (aSchema == "nrm.customer.navigation_report.v1")
      return direct({"runId", "platformId"});
   if (aSchema == "nrm.customer.environment_report.v1")
   {
      if (!direct({"runId", "regionId"})) return false;
      const QJsonObject terrain = aData.value("terrain").toObject();
      if (!ValidateIdentifierArray(terrain.value("blockedLinkIds"),
                                   "/data/terrain/blockedLinkIds", aResult))
         return false;
      const QJsonObject interference = aData.value("interference").toObject();
      return ValidateIdentifierArray(interference.value("affectedLinkIds"),
                                     "/data/interference/affectedLinkIds",
                                     aResult);
   }
   if (aSchema == "nrm.customer.resource_report.v1")
   {
      return direct({"runId"}) &&
             ValidateObjectArrayIdentifiers(aData, "networks", {"networkId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "members", {"memberId", "platformId", "networkId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "links", {"linkId", "networkId", "sourceMemberId", "destinationMemberId"}, {}, aResult) &&
             ValidateOptionalIdentifier(aData.value("operationalArea").toObject(), "areaId", "/data/operationalArea", aResult) &&
             ValidateObjectArrayIdentifiers(aData, "attitudes", {"platformId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "routes", {"routeId", "sourceMemberId", "destinationMemberId"}, {"hops"}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "flows", {"flowId", "sourceMemberId", "destinationMemberId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "gateways", {"gatewayId", "platformId", "ingressNetworkId", "egressNetworkId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "resourceProxies", {"proxyId", "providerId"}, {}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "alarms", {"alarmId", "objectId"}, {}, aResult);
   }
   if (aSchema == "nrm.customer.assessment_request.v1")
      return direct({"taskId", "sourcePlatformId", "destinationPlatformId"});
   if (aSchema == "nrm.customer.assessment_response.v1")
      return direct({"taskId"}) &&
             ValidateIdentifierArray(aData.value("primaryRoute"),
                                     "/data/primaryRoute", aResult) &&
             ValidateIdentifierArray(aData.value("backupRoute"),
                                     "/data/backupRoute", aResult);
   if (aSchema == "nrm.customer.resource_demand_request.v1")
      return direct({"demandSetId"}) &&
             ValidateObjectArrayIdentifiers(aData, "demands", {"demandId", "sourcePlatformId", "destinationPlatformId"}, {}, aResult);
   if (aSchema == "nrm.customer.resource_demand_response.v1")
      return direct({"demandSetId"}) &&
             ValidateObjectArrayIdentifiers(aData, "results", {"demandId"}, {}, aResult);
   if (aSchema == "nrm.customer.network_plan.v1")
      return direct({"planId"}) &&
             ValidateObjectArrayIdentifiers(aData, "allocations", {"allocationId", "profileId", "channelId", "subnetId", "routePolicyId"}, {"members", "slots"}, aResult) &&
             ValidateObjectArrayIdentifiers(aData, "demands", {"demandId", "sourcePlatformId", "destinationPlatformId"}, {}, aResult);
   if (aSchema == "nrm.customer.network_plan_result.v1")
      return direct({"planId"});
   if (aSchema == "nrm.customer.membership_request.v1")
      return direct({"requestId", "planId", "allocationId", "platformId"});
   if (aSchema == "nrm.customer.provider_hello.v1")
      return direct({"providerId"});
   if (aSchema == "nrm.customer.ingest_ack.v1")
      return direct({"originalMessageId"});
   return true;
}

const char* ExpectedSource(const QString& aSchema)
{
   if (aSchema == "nrm.customer.assessment_response.v1" ||
       aSchema == "nrm.customer.resource_demand_response.v1" ||
       aSchema == "nrm.customer.network_plan_result.v1" ||
       aSchema == "nrm.customer.ingest_ack.v1" ||
       aSchema == "nrm.customer.error.v1")
      return "NRM";
   return "CUSTOMER";
}

bool RequiredString(const QJsonObject& aObject, const char* aName,
                    WkNrm::CustomerJsonDecodeResult& aResult)
{
   const QString key = QString::fromLatin1(aName);
   const QJsonValue value = aObject.value(key);
   if (!value.isString() || value.toString().isEmpty())
   {
      aResult.errors.push_back(
         {"REQUIRED_FIELD_MISSING", "/" + std::string(aName),
          "必需字符串字段缺失或为空"});
      return false;
   }
   return true;
}

bool FiniteNumber(const QJsonObject& aObject, const char* aName,
                  double& aValue,
                  double aMinimum = -std::numeric_limits<double>::infinity(),
                  double aMaximum = std::numeric_limits<double>::infinity())
{
   const QJsonValue value = aObject.value(QString::fromLatin1(aName));
   if (!value.isDouble()) return false;
   const double number = value.toDouble();
   if (!std::isfinite(number) || number < aMinimum || number > aMaximum)
      return false;
   aValue = number;
   return true;
}

bool OptionalFiniteNumber(
   const QJsonObject& aObject, const char* aName, double& aValue,
   double aMinimum = -std::numeric_limits<double>::infinity(),
   double aMaximum = std::numeric_limits<double>::infinity())
{
   return !aObject.contains(QString::fromLatin1(aName)) ||
          FiniteNumber(aObject, aName, aValue, aMinimum, aMaximum);
}

bool NonNegativeInteger(const QJsonObject& aObject, const char* aName,
                        std::size_t& aValue)
{
   double number = 0.0;
   if (!FiniteNumber(aObject, aName, number, 0.0,
                     static_cast<double>(std::numeric_limits<std::size_t>::max())) ||
       std::floor(number) != number)
      return false;
   aValue = static_cast<std::size_t>(number);
   return true;
}

bool Position(const QJsonObject& aObject, double& aLatitudeDeg,
              double& aLongitudeDeg, double& aAltitudeM)
{
   return aObject.size() == 3 &&
          FiniteNumber(aObject, "latitudeDeg", aLatitudeDeg, -90.0, 90.0) &&
          FiniteNumber(aObject, "longitudeDeg", aLongitudeDeg, -180.0, 180.0) &&
          FiniteNumber(aObject, "altitudeM", aAltitudeM);
}

QJsonObject DataObject(const QByteArray& aJson)
{
   return QJsonDocument::fromJson(aJson).object().value("data").toObject();
}

nrm::DataOrigin Origin(const QString& aSource)
{
   if (aSource == "AFSIM") return nrm::DataOrigin::cAFSIM_INTERNAL;
   if (aSource == "REPLAY") return nrm::DataOrigin::cREPLAY;
   if (aSource == "NRM") return nrm::DataOrigin::cDERIVED;
   return nrm::DataOrigin::cCUSTOMER_MODULE;
}

nrm::Confidence Confidence(const QString& aText)
{
   if (aText == "HIGH") return nrm::Confidence::cHIGH;
   if (aText == "MEDIUM") return nrm::Confidence::cMEDIUM;
   return nrm::Confidence::cLOW;
}

nrm::NetworkType NetworkType(const QString& aText)
{
   if (aText == "LINK11") return nrm::NetworkType::cLINK11;
   if (aText == "LINK16") return nrm::NetworkType::cLINK16;
   if (aText == "SATCOM") return nrm::NetworkType::cSATCOM;
   if (aText == "CDL") return nrm::NetworkType::cCDL;
   return nrm::NetworkType::cUNKNOWN;
}

nrm::ResourceState State(const QString& aText)
{
   if (aText == "ONLINE") return nrm::ResourceState::cONLINE;
   if (aText == "OFFLINE") return nrm::ResourceState::cOFFLINE;
   if (aText == "DISABLED") return nrm::ResourceState::cDISABLED;
   if (aText == "FAILED") return nrm::ResourceState::cFAILED;
   return nrm::ResourceState::cUNKNOWN;
}

void Metric(nrm::MetricValue<double>& aMetric, double aValue, const char* aUnit,
            double aTime, nrm::DataOrigin aOrigin,
            nrm::Confidence aConfidence = nrm::Confidence::cHIGH)
{
   aMetric.value = aValue;
   aMetric.unit = aUnit;
   aMetric.valid = true;
   aMetric.sampleTime = aTime;
   aMetric.origin = aOrigin;
   aMetric.confidence = aConfidence;
   aMetric.reason = nrm::MetricReason::cNONE;
}

bool OptionalMetric(const QJsonObject& aObject, const char* aName,
                    nrm::MetricValue<double>& aMetric, const char* aUnit,
                    double aTime, nrm::DataOrigin aOrigin, double aMinimum,
                    double aMaximum)
{
   double value = 0.0;
   if (!OptionalFiniteNumber(aObject, aName, value, aMinimum, aMaximum))
      return false;
   if (aObject.contains(QString::fromLatin1(aName)))
      Metric(aMetric, value, aUnit, aTime, aOrigin);
   return true;
}

QJsonArray Strings(const std::vector<std::string>& aValues)
{
   QJsonArray result;
   for (const std::string& value : aValues)
      result.push_back(QString::fromStdString(value));
   return result;
}

QJsonObject ResponseRoot(const char* aSchema,
                         const WkNrm::CustomerJsonEnvelope& aEnvelope,
                         const QJsonObject& aData)
{
   QJsonObject root;
   root.insert("schema", aSchema);
   root.insert("messageId", QString::fromStdString(aEnvelope.messageId));
   root.insert("timestamp", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
   root.insert("source", "NRM");
   root.insert("data", aData);
   return root;
}
}

namespace
{
class DefaultCustomerJsonValidationLayer final
   : public WkNrm::CustomerJsonValidationLayer
{
public:
   WkNrm::CustomerJsonDecodeResult Validate(const QByteArray& aJson) const override;
};

const DefaultCustomerJsonValidationLayer& DefaultValidationLayer()
{
   static const DefaultCustomerJsonValidationLayer layer;
   return layer;
}
}

WkNrm::CustomerJsonDecodeResult
DefaultCustomerJsonValidationLayer::Validate(const QByteArray& aJson) const
{
   QJsonParseError parseError;
   const QJsonDocument document = QJsonDocument::fromJson(aJson, &parseError);
   if (parseError.error != QJsonParseError::NoError)
      return Failure("INVALID_JSON", "/", "JSON语法错误");
   if (!document.isObject())
      return Failure("SCHEMA_VALIDATION_FAILED", "/", "JSON根节点必须是对象");

   const QJsonObject root = document.object();
   WkNrm::CustomerJsonDecodeResult result;
   const std::set<QString> allowed = {"schema", "messageId", "timestamp", "source", "data"};
   for (auto it = root.begin(); it != root.end(); ++it)
   {
      if (allowed.count(it.key()) == 0)
      {
         result.errors.push_back({"SCHEMA_VALIDATION_FAILED",
                                  "/" + it.key().toStdString(),
                                  "公共信封包含未知字段"});
      }
   }
   RequiredString(root, "schema", result);
   RequiredString(root, "messageId", result);
   RequiredString(root, "timestamp", result);
   RequiredString(root, "source", result);
   if (!root.value("data").isObject())
      result.errors.push_back(
         {"REQUIRED_FIELD_MISSING", "/data", "data必须是对象"});
   if (!result.errors.empty()) return result;

   const QString schema = root.value("schema").toString();
   if (!WkNrm::CustomerJsonCodec::IsSupportedSchema(schema))
      return Failure("SCHEMA_UNSUPPORTED", "/schema", "不支持的接口Schema");
   if (!ValidateIdentifier(root.value("messageId"), "/messageId", result))
      return result;
   std::string unknownPath;
   if (HasUnknownKey(root.value("data").toObject(), DataKeys(schema), unknownPath))
      return Failure("SCHEMA_VALIDATION_FAILED", unknownPath.c_str(),
                     "data包含Schema未定义字段");
   if (HasUnknownNestedKey(schema, root.value("data").toObject(), unknownPath))
      return Failure("SCHEMA_VALIDATION_FAILED", unknownPath.c_str(),
                     "嵌套对象包含Schema未定义字段");
   const QDateTime timestamp = QDateTime::fromString(root.value("timestamp").toString(),
                                                     Qt::ISODate);
   if (!timestamp.isValid() || timestamp.timeSpec() == Qt::LocalTime)
      return Failure("SCHEMA_VALIDATION_FAILED", "/timestamp",
                     "timestamp必须是包含时区的ISO 8601时间");
   const QString source = root.value("source").toString();
   if (source != QString::fromLatin1(ExpectedSource(schema)))
      return Failure("SCHEMA_VALIDATION_FAILED", "/source",
                     "source与消息方向不一致");

   result.envelope.schema = schema.toStdString();
   result.envelope.messageId = root.value("messageId").toString().toStdString();
   result.envelope.timestamp = root.value("timestamp").toString().toStdString();
   result.envelope.source = source.toStdString();
   const QJsonObject data = root.value("data").toObject();
   if (!ValidateSchemaIdentifiers(schema, data, result)) return result;
   if (data.value("runId").isString())
      result.envelope.runId = data.value("runId").toString().toStdString();
   if (data.value("simTime").isDouble() &&
       std::isfinite(data.value("simTime").toDouble()))
   {
      result.envelope.simTime = data.value("simTime").toDouble();
      result.envelope.hasSimTime = true;
   }
   result.valid = true;
   return result;
}

WkNrm::CustomerJsonCodec::CustomerJsonCodec()
   : mValidationLayer(&DefaultValidationLayer())
{
}

WkNrm::CustomerJsonCodec::CustomerJsonCodec(
   const CustomerJsonValidationLayer& aValidationLayer)
   : mValidationLayer(&aValidationLayer)
{
}

WkNrm::CustomerJsonDecodeResult
WkNrm::CustomerJsonCodec::Inspect(const QByteArray& aJson) const
{
   return mValidationLayer->Validate(aJson);
}

QByteArray WkNrm::CustomerJsonCodec::EncodeError(
   const CustomerJsonEnvelope& aEnvelope,
   const std::vector<CustomerJsonError>& aErrors) const
{
   QJsonArray errors;
   for (const CustomerJsonError& error : aErrors)
   {
      QJsonObject item;
      item.insert("code", QString::fromStdString(error.code));
      item.insert("path", QString::fromStdString(error.path));
      item.insert("message", QString::fromStdString(error.message));
      errors.push_back(item);
   }
   QJsonObject data;
   data.insert("success", false);
   data.insert("errors", errors);
   QJsonObject root;
   root.insert("schema", "nrm.customer.error.v1");
   root.insert("messageId", QString::fromStdString(aEnvelope.messageId));
   root.insert("timestamp", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
   root.insert("source", "NRM");
   root.insert("data", data);
   return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

bool WkNrm::CustomerJsonCodec::IsSupportedSchema(const QString& aSchema)
{
   static const std::set<QString> schemas = {
      "nrm.customer.navigation_report.v1",
      "nrm.customer.environment_report.v1",
      "nrm.customer.resource_report.v1",
      "nrm.customer.assessment_request.v1",
      "nrm.customer.assessment_response.v1",
      "nrm.customer.resource_demand_request.v1",
      "nrm.customer.resource_demand_response.v1",
      "nrm.customer.network_plan.v1",
      "nrm.customer.network_plan_result.v1",
      "nrm.customer.membership_request.v1",
      "nrm.customer.provider_hello.v1",
      "nrm.customer.ingest_ack.v1",
      "nrm.customer.error.v1"};
   return schemas.count(aSchema) != 0;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeNavigation(
   const QByteArray& aJson, nrm::NavigationSample& aSample) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.navigation_report.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是导航消息") : result;
   const QJsonObject data = DataObject(aJson);
   const QJsonObject truth = data.value("truthPosition").toObject();
   const QJsonObject perceived = data.value("perceivedPosition").toObject();
   const QJsonObject error = data.value("positionErrorM").toObject();
   const QString type = data.value("navigationType").toString();
   const QString status = data.value("status").toString();
   double simTime = 0.0;
   double truthLatitude = 0.0;
   double truthLongitude = 0.0;
   double truthAltitude = 0.0;
   double perceivedLatitude = 0.0;
   double perceivedLongitude = 0.0;
   double perceivedAltitude = 0.0;
   if (data.value("platformId").toString().isEmpty() ||
       !FiniteNumber(data, "simTime", simTime, 0.0) ||
       (type != "GNSS" && type != "INS" && type != "INTEGRATED"))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "导航数据字段无效");
   if (!Position(truth, truthLatitude, truthLongitude, truthAltitude))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data/truthPosition",
                     "真实位置字段无效");
   if (!Position(perceived, perceivedLatitude, perceivedLongitude,
                 perceivedAltitude))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data/perceivedPosition",
                     "感知位置字段无效");
   if (data.contains("positionErrorM") && !data.value("positionErrorM").isObject())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data/positionErrorM",
                     "位置误差必须是对象");
   if (!status.isEmpty() && !data.value("status").isString())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data/status", "导航状态无效");
   const QString confidenceText = data.value("confidence").toString();
   if (data.contains("confidence") && confidenceText != "LOW" &&
       confidenceText != "MEDIUM" && confidenceText != "HIGH")
      return Failure("SCHEMA_VALIDATION_FAILED", "/data/confidence",
                     "置信度枚举无效");
   nrm::NavigationSample sample;
   sample.platformId = data.value("platformId").toString().toStdString();
   sample.platformName = sample.platformId;
   sample.navigationType = type.toStdString();
   sample.rawStatus = status.isEmpty() ? type.toStdString() : status.toStdString();
   sample.mode = type == "INS" ? nrm::NavigationMode::cINS : nrm::NavigationMode::cGPS_ACTIVE;
   sample.statusCode = type == "INS" ? -1 : 1;
   sample.sampleTime = simTime;
   sample.origin = Origin(QString::fromStdString(result.envelope.source));
   sample.confidence = Confidence(data.value("confidence").toString());
   sample.valid = true;
   Metric(sample.truthLatitudeDeg, truthLatitude, "deg", sample.sampleTime,
          sample.origin, sample.confidence);
   Metric(sample.truthLongitudeDeg, truthLongitude, "deg", sample.sampleTime,
          sample.origin, sample.confidence);
   Metric(sample.truthAltitudeM, truthAltitude, "m", sample.sampleTime,
          sample.origin, sample.confidence);
   Metric(sample.perceivedLatitudeDeg, perceivedLatitude, "deg", sample.sampleTime,
          sample.origin, sample.confidence);
   Metric(sample.perceivedLongitudeDeg, perceivedLongitude, "deg", sample.sampleTime,
          sample.origin, sample.confidence);
   Metric(sample.perceivedAltitudeM, perceivedAltitude, "m", sample.sampleTime,
          sample.origin, sample.confidence);
   if (!error.isEmpty())
   {
      double inTrack = 0.0;
      double crossTrack = 0.0;
      double vertical = 0.0;
      if (error.size() != 3 || !FiniteNumber(error, "inTrack", inTrack) ||
          !FiniteNumber(error, "crossTrack", crossTrack) ||
          !FiniteNumber(error, "vertical", vertical))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/positionErrorM",
                        "位置误差字段无效");
      Metric(sample.inTrackErrorM, inTrack, "m", sample.sampleTime, sample.origin, sample.confidence);
      Metric(sample.crossTrackErrorM, crossTrack, "m", sample.sampleTime, sample.origin, sample.confidence);
      Metric(sample.verticalErrorM, vertical, "m", sample.sampleTime, sample.origin, sample.confidence);
      Metric(sample.totalPositionErrorM, std::sqrt(inTrack * inTrack + crossTrack * crossTrack + vertical * vertical), "m", sample.sampleTime, sample.origin, sample.confidence);
   }
   aSample = sample;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeEnvironment(
   const QByteArray& aJson, nrm::EnvironmentSnapshot& aSnapshot,
   nrm::EnvironmentContext& aContext) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.environment_report.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是环境消息") : result;
   const QJsonObject data = DataObject(aJson);
   double simTime = 0.0;
   const QString applicationMode = data.value("applicationMode").toString();
   if (data.value("runId").toString().isEmpty() ||
       data.value("regionId").toString().isEmpty() ||
       !FiniteNumber(data, "simTime", simTime, 0.0) ||
       (applicationMode != "INFORMATION_ONLY" &&
        applicationMode != "ALREADY_INCLUDED" &&
        applicationMode != "CANDIDATE_ADJUSTMENT"))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "环境数据字段无效");
   if (!data.contains("terrain") && !data.contains("weather") &&
       !data.contains("astronomy") && !data.contains("interference"))
      return Failure("REQUIRED_FIELD_MISSING", "/data",
                     "至少需要一种环境数据");
   nrm::EnvironmentSnapshot snapshot;
   nrm::EnvironmentContext context;
   snapshot.providerId = result.envelope.source;
   snapshot.sampleTime = simTime;
   snapshot.origin = Origin(QString::fromStdString(result.envelope.source));
   snapshot.valid = true;
   if (data.contains("terrain"))
   {
      if (!data.value("terrain").isObject() ||
          !data.value("terrain").toObject().value("enabled").isBool())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/terrain",
                        "地形字段无效");
      snapshot.terrain.available = true;
      context.customerProvidedDomains.push_back(
         nrm::EnvironmentDomain::cTERRAIN);
      snapshot.terrain.origin = snapshot.origin;
      snapshot.terrain.confidence = snapshot.confidence;
      const QJsonObject terrain = data.value("terrain").toObject();
      snapshot.terrain.enabled = terrain.value("enabled").toBool();
      if (terrain.contains("blockedLinkIds"))
      {
         if (!terrain.value("blockedLinkIds").isArray())
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/terrain/blockedLinkIds",
                           "阻断链路列表无效");
         std::set<QString> ids;
         for (const QJsonValue& value : terrain.value("blockedLinkIds").toArray())
         {
            if (!value.isString() || value.toString().isEmpty() ||
                !ids.insert(value.toString()).second)
               return Failure("SCHEMA_VALIDATION_FAILED", "/data/terrain/blockedLinkIds",
                              "阻断链路编号为空或重复");
            snapshot.terrain.blockedLinkIds.push_back(value.toString().toStdString());
         }
         snapshot.terrain.blockedLinkCount = snapshot.terrain.blockedLinkIds.size();
      }
   }
   if (data.contains("weather"))
   {
      if (!data.value("weather").isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/weather", "气象字段无效");
      snapshot.weather.available = true;
      context.customerProvidedDomains.push_back(
         nrm::EnvironmentDomain::cWEATHER);
      snapshot.weather.origin = snapshot.origin;
      snapshot.weather.confidence = snapshot.confidence;
      const auto weather = data.value("weather").toObject();
      double value = 0.0;
      if (!OptionalFiniteNumber(weather, "windSpeedMps", value, 0.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/weather/windSpeedMps",
                        "风速字段无效");
      if (weather.contains("windSpeedMps"))
         Metric(snapshot.weather.windSpeedMps, value, "m/s", snapshot.sampleTime,
                snapshot.origin);
      if (!OptionalFiniteNumber(weather, "rainRateMmPerHour", value, 0.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/weather/rainRateMmPerHour",
                        "降雨率字段无效");
      if (weather.contains("rainRateMmPerHour"))
         Metric(snapshot.weather.rainRateMmPerHour, value, "mm/h", snapshot.sampleTime,
                snapshot.origin);
      if (!OptionalFiniteNumber(weather, "cloudPercent", value, 0.0, 100.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/weather/cloudPercent",
                        "云量字段无效");
      if (weather.contains("cloudPercent"))
         Metric(snapshot.weather.cloudPercent, value, "percent", snapshot.sampleTime,
                snapshot.origin);
   }
   if (data.contains("astronomy"))
   {
      if (!data.value("astronomy").isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/astronomy", "天象字段无效");
      snapshot.celestial.available = true;
      context.customerProvidedDomains.push_back(
         nrm::EnvironmentDomain::cCELESTIAL);
      snapshot.celestial.origin = snapshot.origin;
      snapshot.celestial.confidence = snapshot.confidence;
      const auto astronomy = data.value("astronomy").toObject();
      double value = 0.0;
      if (!OptionalFiniteNumber(astronomy, "julianDate", value))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/astronomy/julianDate",
                        "儒略日字段无效");
      if (astronomy.contains("julianDate"))
         Metric(snapshot.celestial.julianDate, value, "day", snapshot.sampleTime,
                snapshot.origin);
      if (!OptionalFiniteNumber(astronomy, "sunElevationDeg", value, -90.0, 90.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/astronomy/sunElevationDeg",
                        "太阳高度角字段无效");
      if (astronomy.contains("sunElevationDeg"))
         Metric(snapshot.celestial.sunElevationDeg, value, "deg", snapshot.sampleTime,
                snapshot.origin);
   }
   if (data.contains("interference"))
   {
      if (!data.value("interference").isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/interference",
                        "电磁干扰字段无效");
      snapshot.interference.available = true;
      context.customerProvidedDomains.push_back(
         nrm::EnvironmentDomain::cELECTROMAGNETIC_INTERFERENCE);
      snapshot.interference.origin = snapshot.origin;
      snapshot.interference.confidence = snapshot.confidence;
      const auto interference = data.value("interference").toObject();
      double value = 0.0;
      if (!OptionalFiniteNumber(interference, "maximumPowerDbm", value))
         return Failure("SCHEMA_VALIDATION_FAILED",
                        "/data/interference/maximumPowerDbm", "干扰功率字段无效");
      if (interference.contains("maximumPowerDbm"))
         Metric(snapshot.interference.maximumPowerDbm, value, "dBm",
                snapshot.sampleTime, snapshot.origin);
      if (!OptionalFiniteNumber(interference, "capacityScale", value, 0.0, 1.0))
         return Failure("SCHEMA_VALIDATION_FAILED",
                        "/data/interference/capacityScale", "容量缩放字段无效");
      if (interference.contains("capacityScale"))
         Metric(snapshot.interference.capacityScale, value, "ratio",
                snapshot.sampleTime, snapshot.origin);
      if (interference.contains("affectedLinkIds"))
      {
         if (!interference.value("affectedLinkIds").isArray())
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/interference/affectedLinkIds", "受影响链路列表无效");
         std::set<QString> ids;
         for (const QJsonValue& affected : interference.value("affectedLinkIds").toArray())
         {
            if (!affected.isString() || affected.toString().isEmpty() ||
                !ids.insert(affected.toString()).second)
               return Failure("SCHEMA_VALIDATION_FAILED",
                              "/data/interference/affectedLinkIds",
                              "受影响链路编号为空或重复");
            snapshot.interference.affectedLinkIds.push_back(
               affected.toString().toStdString());
         }
      }
   }
   context.contextId = data.value("regionId").toString().toStdString();
   context.schemaVersion = result.envelope.schema;
   context.providerId = result.envelope.source;
   context.sampleTime = snapshot.sampleTime;
   context.origin = snapshot.origin;
   context.valid = true;
   if (applicationMode == "ALREADY_INCLUDED")
      context.applicationMode =
         nrm::EnvironmentApplicationMode::cALREADY_INCLUDED;
   else if (applicationMode == "CANDIDATE_ADJUSTMENT")
      context.applicationMode =
         nrm::EnvironmentApplicationMode::cCANDIDATE_ADJUSTMENT;
   else
      context.applicationMode =
         nrm::EnvironmentApplicationMode::cINFORMATION_ONLY;
   context.applyParameterizedEffects = applicationMode == "CANDIDATE_ADJUSTMENT";
   aSnapshot = snapshot;
   aContext = context;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeResources(
   const QByteArray& aJson, nrm::ResourceSnapshot& aSnapshot) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.resource_report.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是资源消息") : result;
   const QJsonObject data = DataObject(aJson);
   double simTime = 0.0;
   if (data.value("runId").toString().isEmpty() ||
       !FiniteNumber(data, "simTime", simTime, 0.0) ||
       !data.value("networks").isArray() || !data.value("members").isArray() ||
       !data.value("links").isArray())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "资源快照字段无效");

   nrm::ResourceSnapshot snapshot;
   snapshot.simTime = simTime;
   snapshot.runtimeState = nrm::RuntimeState::cRUNNING;
   snapshot.origin = Origin(QString::fromStdString(result.envelope.source));
   snapshot.providerId = result.envelope.source;
   std::map<QString, nrm::NetworkType> networks;
   for (const QJsonValue& value : data.value("networks").toArray())
   {
      if (!value.isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/networks",
                        "网络记录必须是对象");
      const QJsonObject object = value.toObject();
      const QString id = object.value("networkId").toString();
      const nrm::NetworkType type = NetworkType(object.value("networkType").toString());
      if (id.isEmpty() || object.value("name").toString().isEmpty() ||
          type == nrm::NetworkType::cUNKNOWN)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/networks",
                        "网络编号、名称或类型无效");
      networks.emplace(id, type);
      nrm::NetworkSnapshot network;
      network.networkId = id.toStdString();
      network.networkName = object.value("name").toString().toStdString();
      network.networkType = type;
      network.origin = snapshot.origin;
      snapshot.networks.push_back(network);
   }
   std::map<QString, nrm::EndpointSnapshot> endpoints;
   for (const QJsonValue& value : data.value("members").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString id = object.value("memberId").toString();
      const QString networkId = object.value("networkId").toString();
      nrm::EndpointSnapshot endpoint;
      endpoint.endpointId = id.toStdString();
      endpoint.platformId = object.value("platformId").toString().toStdString();
      endpoint.platformName = endpoint.platformId;
      endpoint.networkId = networkId.toStdString();
      endpoint.networkName = networkId.toStdString();
      const auto network = networks.find(networkId);
      endpoint.networkType = network == networks.end()
                                ? nrm::NetworkType::cUNKNOWN : network->second;
      endpoint.state = State(object.value("state").toString());
      if (endpoint.state == nrm::ResourceState::cUNKNOWN)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/members/state",
                        "成员状态枚举无效");
      endpoint.canSend = endpoint.state == nrm::ResourceState::cONLINE;
      endpoint.canReceive = endpoint.canSend;
      const QJsonObject position = object.value("position").toObject();
      double latitude = 0.0;
      double longitude = 0.0;
      double altitude = 0.0;
      if (!object.value("position").isObject() ||
          !Position(position, latitude, longitude, altitude))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/members/position",
                        "成员位置字段无效");
      Metric(endpoint.latitudeDeg, latitude, "deg", snapshot.simTime, snapshot.origin);
      Metric(endpoint.longitudeDeg, longitude, "deg", snapshot.simTime, snapshot.origin);
      Metric(endpoint.altitudeM, altitude, "m", snapshot.simTime, snapshot.origin);
      endpoints.emplace(id, endpoint);
      snapshot.endpoints.push_back(endpoint);
   }
   for (const QJsonValue& value : data.value("links").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString id = object.value("linkId").toString();
      const QString networkId = object.value("networkId").toString();
      const QString sourceId = object.value("sourceMemberId").toString();
      const QString destinationId = object.value("destinationMemberId").toString();
      nrm::LinkSnapshot link;
      link.linkId = id.toStdString();
      link.sourceEndpointId = sourceId.toStdString();
      link.destinationEndpointId = destinationId.toStdString();
      const auto source = endpoints.find(sourceId);
      const auto destination = endpoints.find(destinationId);
      if (source != endpoints.end()) link.sourcePlatform = source->second.platformName;
      if (destination != endpoints.end())
         link.destinationPlatform = destination->second.platformName;
      link.networkId = networkId.toStdString();
      link.networkName = networkId.toStdString();
      const auto network = networks.find(networkId);
      link.networkType = network == networks.end()
                            ? nrm::NetworkType::cUNKNOWN : network->second;
      link.state = State(object.value("state").toString());
      if (link.state == nrm::ResourceState::cUNKNOWN)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/state",
                        "链路状态枚举无效");
      link.subnetId = object.value("subnetId").toString().toStdString();
      std::set<QString> businessTypes;
      for (const QJsonValue& business : object.value("supportedBusinessTypes").toArray())
      {
         if (!business.isString() || business.toString().isEmpty() ||
             !businessTypes.insert(business.toString()).second)
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/links/supportedBusinessTypes",
                           "承载业务类型无效或重复");
         link.supportedBusinessTypes.push_back(business.toString().toStdString());
      }
      const double negativeInfinity = -std::numeric_limits<double>::infinity();
      const double positiveInfinity = std::numeric_limits<double>::infinity();
      if (!OptionalMetric(object, "bandwidthBps", link.bandwidthBps, "bit/s",
                          snapshot.simTime, snapshot.origin, 0.0,
                          positiveInfinity) ||
          !OptionalMetric(object, "rssiDbm", link.rssiDbm, "dBm",
                          snapshot.simTime, snapshot.origin, negativeInfinity,
                          positiveInfinity) ||
          !OptionalMetric(object, "snrDb", link.snrDb, "dB", snapshot.simTime,
                          snapshot.origin, negativeInfinity, positiveInfinity) ||
          !OptionalMetric(object, "berRatio", link.ber, "ratio",
                          snapshot.simTime, snapshot.origin, 0.0, 1.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links",
                        "链路物理量字段无效");
      nrm::WindowMetrics window;
      window.windowS = 0.0;
      if (!OptionalMetric(object, "delayMs", window.averageTransportDelayMs,
                          "ms", snapshot.simTime, snapshot.origin, 0.0,
                          positiveInfinity))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/delayMs",
                        "链路时延字段无效");
      if (!OptionalMetric(object, "pdrPercent", window.deliveryRatioPercent,
                          "percent", snapshot.simTime, snapshot.origin, 0.0,
                          100.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/pdrPercent",
                        "链路PDR字段无效");
      if (!OptionalMetric(object, "trafficBps", window.deliveredThroughputBps,
                          "bit/s", snapshot.simTime, snapshot.origin, 0.0,
                          positiveInfinity))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/trafficBps",
                        "链路流量字段无效");
      if (!OptionalMetric(object, "queuePercent", window.queueUtilizationPercent,
                          "percent", snapshot.simTime, snapshot.origin, 0.0,
                          100.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/queuePercent",
                        "队列占用率字段无效");
      const QJsonObject protocol = object.value("protocolResource").toObject();
      if (!protocol.isEmpty())
      {
         std::size_t capacity = 0;
         std::size_t used = 0;
         const QString kind = protocol.value("kind").toString();
         if ((kind != "POLLING_UNIT" && kind != "TIMESLOT" &&
              kind != "BEAM_CHANNEL" && kind != "CHANNEL") ||
             !NonNegativeInteger(protocol, "capacity", capacity) ||
             !NonNegativeInteger(protocol, "used", used))
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/links/protocolResource",
                           "协议资源字段无效");
         link.protocolResource.kind = protocol.value("kind").toString().toStdString();
         link.protocolResource.capacity = capacity;
         link.protocolResource.used = used;
         link.protocolResource.remaining =
            link.protocolResource.capacity >= link.protocolResource.used
               ? link.protocolResource.capacity - link.protocolResource.used : 0;
         link.protocolResource.valid = true;
         if (link.protocolResource.capacity > 0)
            Metric(link.protocolResource.utilizationPercent,
                   100.0 * static_cast<double>(link.protocolResource.used) /
                      static_cast<double>(link.protocolResource.capacity),
                   "percent", snapshot.simTime, snapshot.origin);
         else
            link.protocolResource.utilizationPercent.reason = nrm::MetricReason::cZERO_DENOMINATOR;
      }
      const QJsonObject coverage = object.value("coverage").toObject();
      if (!coverage.isEmpty())
      {
         double maximumRangeM = 0.0;
         if (!FiniteNumber(coverage, "maximumRangeM", maximumRangeM, 0.0))
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/links/coverage",
                           "覆盖范围字段无效");
         Metric(link.coverage.maximumRangeM, maximumRangeM,
                "m", snapshot.simTime, snapshot.origin);
         link.coverage.valid = true;
      }
      link.windows.push_back(window);
      snapshot.links.push_back(link);
   }
   if (data.contains("operationalArea"))
   {
      if (!data.value("operationalArea").isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/operationalArea",
                        "作战区域必须是对象");
      const QJsonObject area = data.value("operationalArea").toObject();
      snapshot.operationalArea.areaId = area.value("areaId").toString().toStdString();
      if (!FiniteNumber(area, "validFrom", snapshot.operationalArea.validFrom, 0.0) ||
          !FiniteNumber(area, "validUntil", snapshot.operationalArea.validUntil, 0.0) ||
          !area.value("points").isArray())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/operationalArea",
                        "作战区域时间或点集无效");
      for (const QJsonValue& value : area.value("points").toArray())
      {
         if (!value.isObject())
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/operationalArea/points", "区域点必须是对象");
         const QJsonObject point = value.toObject();
         double latitude = 0.0;
         double longitude = 0.0;
         double altitude = 0.0;
         if (!Position(point, latitude, longitude, altitude))
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/operationalArea/points", "区域点坐标无效");
         snapshot.operationalArea.points.push_back({latitude, longitude, altitude});
      }
      if (snapshot.operationalArea.areaId.empty() ||
          snapshot.operationalArea.points.size() < 3 ||
          snapshot.operationalArea.validUntil < snapshot.operationalArea.validFrom)
         return Failure("DATA_INVALID", "/data/operationalArea", "作战区域字段无效");
   }
   std::set<QString> attitudePlatforms;
   for (const QJsonValue& value : data.value("attitudes").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString platformId = object.value("platformId").toString();
      if (platformId.isEmpty() || !attitudePlatforms.insert(platformId).second)
         return Failure("DATA_INVALID", "/data/attitudes", "平台姿态重复或无平台编号");
      nrm::PlatformAttitude attitude;
      attitude.platformName = platformId.toStdString();
      if (!FiniteNumber(object, "headingDeg", attitude.headingDeg, 0.0, 360.0) ||
          !FiniteNumber(object, "pitchDeg", attitude.pitchDeg, -90.0, 90.0) ||
          !FiniteNumber(object, "rollDeg", attitude.rollDeg, -180.0, 180.0) ||
          !FiniteNumber(object, "sampleTime", attitude.sampleTime, 0.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/attitudes",
                        "平台姿态数值无效");
      attitude.origin = snapshot.origin;
      attitude.valid = true;
      snapshot.platformAttitudes.push_back(attitude);
   }
   std::set<QString> alarmIds;
   for (const QJsonValue& value : data.value("alarms").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString alarmId = object.value("alarmId").toString();
      if (alarmId.isEmpty() || !alarmIds.insert(alarmId).second)
         return Failure("DATA_INVALID", "/data/alarms", "告警编号重复或为空");
      nrm::ResourceAlarm alarm;
      alarm.alarmId = alarmId.toStdString();
      alarm.objectId = object.value("objectId").toString().toStdString();
      alarm.reasonCode = object.value("reasonCode").toString().toStdString();
      alarm.severity = object.value("severity").toString().toStdString();
      if (alarm.objectId.empty() || alarm.reasonCode.empty() ||
          (alarm.severity != "INFO" && alarm.severity != "WARNING" &&
           alarm.severity != "ERROR") ||
          !FiniteNumber(object, "startTime", alarm.startTime, 0.0) ||
          !object.value("active").isBool())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/alarms",
                        "告警字段无效");
      alarm.active = object.value("active").toBool();
      snapshot.alarms.push_back(alarm);
   }
   std::set<QString> proxyIds;
   for (const QJsonValue& value : data.value("resourceProxies").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString proxyId = object.value("proxyId").toString();
      if (proxyId.isEmpty() || !proxyIds.insert(proxyId).second)
         return Failure("DATA_INVALID", "/data/resourceProxies", "资源代理编号重复或为空");
      nrm::ResourceProxyState proxy;
      proxy.proxyId = proxyId.toStdString();
      proxy.resourceType = object.value("resourceType").toString().toStdString();
      proxy.providerId = object.value("providerId").toString().toStdString();
      if (proxy.resourceType.empty() || proxy.providerId.empty() ||
          !object.value("online").isBool() ||
          !FiniteNumber(object, "lastUpdateTime", proxy.lastUpdateTime, 0.0))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/resourceProxies",
                        "资源代理字段无效");
      proxy.online = object.value("online").toBool();
      snapshot.resourceProxies.push_back(proxy);
   }
   for (const QJsonValue& value : data.value("routes").toArray())
   {
      const QJsonObject object = value.toObject();
      nrm::RouteResourceState route;
      route.routeId = object.value("routeId").toString().toStdString();
      route.sourceMemberId = object.value("sourceMemberId").toString().toStdString();
      route.destinationMemberId = object.value("destinationMemberId").toString().toStdString();
      if (!object.value("active").isBool() || !object.value("hops").isArray())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/routes", "路由字段无效");
      route.active = object.value("active").toBool();
      for (const QJsonValue& hop : object.value("hops").toArray())
      {
         if (!hop.isString() || hop.toString().isEmpty())
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/routes/hops",
                           "路由跳无效");
         route.hops.push_back(hop.toString().toStdString());
      }
      snapshot.routes.push_back(route);
   }
   for (const QJsonValue& value : data.value("flows").toArray())
   {
      const QJsonObject object = value.toObject();
      nrm::BusinessFlowState flow;
      flow.flowId = object.value("flowId").toString().toStdString();
      flow.businessType = object.value("businessType").toString().toStdString();
      flow.sourceMemberId = object.value("sourceMemberId").toString().toStdString();
      flow.destinationMemberId = object.value("destinationMemberId").toString().toStdString();
      if (flow.flowId.empty() || endpoints.count(QString::fromStdString(flow.sourceMemberId)) == 0 ||
          endpoints.count(QString::fromStdString(flow.destinationMemberId)) == 0)
         return Failure("REFERENCE_NOT_FOUND", "/data/flows", "业务流引用不存在");
      if (object.contains("trafficBps"))
      {
         double trafficBps = 0.0;
         if (!FiniteNumber(object, "trafficBps", trafficBps, 0.0))
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/flows/trafficBps",
                           "业务流量字段无效");
         Metric(flow.trafficBps, trafficBps, "bit/s", snapshot.simTime,
                snapshot.origin);
      }
      snapshot.flows.push_back(flow);
   }
   for (const QJsonValue& value : data.value("gateways").toArray())
   {
      const QJsonObject object = value.toObject();
      nrm::GatewayResourceState gateway;
      gateway.gatewayId = object.value("gatewayId").toString().toStdString();
      gateway.platformId = object.value("platformId").toString().toStdString();
      gateway.ingressNetworkId = object.value("ingressNetworkId").toString().toStdString();
      gateway.egressNetworkId = object.value("egressNetworkId").toString().toStdString();
      if (!object.value("enabled").isBool())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/gateways/enabled",
                        "网关启用状态无效");
      gateway.enabled = object.value("enabled").toBool();
      snapshot.gateways.push_back(gateway);
   }
   for (nrm::NetworkSnapshot& network : snapshot.networks)
   {
      for (const auto& endpoint : snapshot.endpoints)
      {
         if (endpoint.networkId != network.networkId) continue;
         ++network.endpointCount;
         if (endpoint.state == nrm::ResourceState::cONLINE) ++network.onlineCount;
      }
      for (const auto& link : snapshot.links)
      {
         if (link.networkId == network.networkId &&
             link.state == nrm::ResourceState::cONLINE)
            ++network.activeLinks;
      }
   }
   const nrm::ResourceSnapshotValidationResult semantic =
      nrm::ResourceSnapshotValidator().Validate(snapshot);
   if (!semantic.valid)
   {
      const nrm::ResourceSnapshotValidationIssue& issue = semantic.issues.front();
      const std::string path = "/data" + issue.path;
      const std::string message =
         std::string(nrm::ToString(issue.reason)) + ": " + issue.message;
      return Failure("RESOURCE_SEMANTIC_INVALID", path.c_str(), message.c_str());
   }
   aSnapshot = snapshot;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeProviderHello(
   const QByteArray& aJson, CustomerProviderHello& aHello) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.provider_hello.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是提供方握手消息") : result;
   const QJsonObject data = DataObject(aJson);
   if (data.value("providerId").toString().isEmpty() ||
       data.value("softwareVersion").toString().isEmpty() ||
       !data.value("supportedSchemas").isArray() ||
       !data.value("supportedNetworkTypes").isArray())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "提供方握手字段无效");
   CustomerProviderHello hello;
   hello.providerId = data.value("providerId").toString().toStdString();
   hello.softwareVersion = data.value("softwareVersion").toString().toStdString();
   std::set<QString> schemas;
   for (const QJsonValue& value : data.value("supportedSchemas").toArray())
   {
      if (!value.isString() || value.toString().isEmpty() ||
          !schemas.insert(value.toString()).second)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/supportedSchemas",
                        "Schema列表为空、无效或重复");
      hello.supportedSchemas.push_back(value.toString().toStdString());
   }
   std::set<nrm::NetworkType> networkTypes;
   for (const QJsonValue& value : data.value("supportedNetworkTypes").toArray())
   {
      const nrm::NetworkType type = NetworkType(value.toString());
      if (type == nrm::NetworkType::cUNKNOWN ||
          !networkTypes.insert(type).second)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/supportedNetworkTypes",
                        "网络类型无效或重复");
      hello.supportedNetworkTypes.push_back(type);
   }
   if (hello.supportedSchemas.empty() || hello.supportedNetworkTypes.empty())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "能力列表不能为空");
   aHello = hello;
   return result;
}

QByteArray WkNrm::CustomerJsonCodec::EncodeIngestAck(
   const CustomerJsonEnvelope& aEnvelope, CustomerIngestStatus aStatus,
   const std::string& aDetail) const
{
   const char* status = "REJECTED";
   switch (aStatus)
   {
   case CustomerIngestStatus::cACCEPTED: status = "ACCEPTED"; break;
   case CustomerIngestStatus::cDUPLICATE: status = "DUPLICATE"; break;
   case CustomerIngestStatus::cSTALE: status = "STALE"; break;
   case CustomerIngestStatus::cREJECTED: status = "REJECTED"; break;
   }
   QJsonObject data;
   data.insert("status", status);
   data.insert("originalMessageId", QString::fromStdString(aEnvelope.messageId));
   data.insert("detail", QString::fromStdString(aDetail));
   return QJsonDocument(ResponseRoot("nrm.customer.ingest_ack.v1", aEnvelope, data))
      .toJson(QJsonDocument::Compact);
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeAssessment(
   const QByteArray& aJson, nrm::AssessmentTask& aTask) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.assessment_request.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是任务评估请求") : result;
   const QJsonObject data = DataObject(aJson);
   const QString source = data.value("sourcePlatformId").toString();
   const QString destination = data.value("destinationPlatformId").toString();
   if (data.value("taskId").toString().isEmpty() || source.isEmpty() ||
       destination.isEmpty() || source == destination ||
       data.value("businessType").toString().isEmpty() ||
       !data.value("requiredBandwidthBps").isDouble() ||
       !data.value("maximumDelayMs").isDouble() ||
       !data.value("minimumPdrPercent").isDouble() ||
       !data.value("allowedNetworks").isArray())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "任务评估字段无效");

   nrm::AssessmentTask task;
   task.taskId = data.value("taskId").toString().toStdString();
   task.sourcePlatform = source.toStdString();
   task.destinationPlatform = destination.toStdString();
   task.businessType = data.value("businessType").toString().toStdString();
   task.requiredBandwidthBps = data.value("requiredBandwidthBps").toDouble();
   task.maximumDelayMs = data.value("maximumDelayMs").toDouble();
   task.requireDelayMetricForFeasibility = task.maximumDelayMs > 0.0;
   task.minimumPdrPercent = data.value("minimumPdrPercent").toDouble();
   std::set<nrm::NetworkType> networkTypes;
   for (const QJsonValue& value : data.value("allowedNetworks").toArray())
   {
      const nrm::NetworkType type = NetworkType(value.toString());
      if (type == nrm::NetworkType::cUNKNOWN ||
          !networkTypes.insert(type).second)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allowedNetworks",
                        "包含未知或重复网络类型");
      task.allowedNetworks.push_back(type);
   }
   if (task.requiredBandwidthBps < 0.0 || task.maximumDelayMs < 0.0 ||
       task.minimumPdrPercent < 0.0 || task.minimumPdrPercent > 100.0 ||
       task.allowedNetworks.empty())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "任务约束超出允许范围");
   aTask = task;
   return result;
}

WkNrm::CustomerJsonDecodeResult
WkNrm::CustomerJsonCodec::DecodeResourceDemands(
   const QByteArray& aJson, nrm::ResourceDemandSet& aDemandSet) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid ||
       result.envelope.schema != "nrm.customer.resource_demand_request.v1")
      return result.valid
         ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是资源需求请求")
         : result;

   const QJsonObject data = DataObject(aJson);
   std::size_t revision = 0;
   if (data.value("demandSetId").toString().isEmpty() ||
       !NonNegativeInteger(data, "revision", revision) || revision < 1 ||
       !data.value("demands").isArray() ||
       data.value("demands").toArray().isEmpty())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data",
                     "资源需求集字段无效");

   nrm::ResourceDemandSet demandSet;
   demandSet.demandSetId = data.value("demandSetId").toString().toStdString();
   demandSet.revision = static_cast<std::uint64_t>(revision);
   demandSet.providerId = result.envelope.source;
   demandSet.createdTime = result.envelope.timestamp;
   demandSet.requestSource = result.envelope.source;
   demandSet.correlationId = result.envelope.messageId;
   demandSet.source = nrm::DataOrigin::cCUSTOMER_MODULE;
   demandSet.confidence = nrm::Confidence::cLOW;

   std::set<std::string> demandIds;
   const QJsonArray demands = data.value("demands").toArray();
   for (int index = 0; index < demands.size(); ++index)
   {
      if (!demands.at(index).isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "资源需求必须是对象");
      const QJsonObject object = demands.at(index).toObject();
      const QString demandId = object.value("demandId").toString();
      const QString source = object.value("sourcePlatformId").toString();
      const QString destination =
         object.value("destinationPlatformId").toString();
      double bandwidth = 0.0;
      double delay = 0.0;
      double pdr = 0.0;
      double traffic = 0.0;
      double distance = 0.0;
      std::size_t payloadBits = 0;
      std::size_t minimumNetworkSize = 0;
      if (demandId.isEmpty() || source.isEmpty() || destination.isEmpty() ||
          source == destination ||
          object.value("businessType").toString().isEmpty() ||
          !FiniteNumber(object, "requiredBandwidthBps", bandwidth, 0.0) ||
          !FiniteNumber(object, "maximumDelayMs", delay, 0.0) ||
          !FiniteNumber(object, "minimumPdrPercent", pdr, 0.0, 100.0) ||
          !object.value("allowedNetworks").isArray() ||
          object.value("allowedNetworks").toArray().isEmpty() ||
          !OptionalFiniteNumber(object, "businessTrafficBps", traffic, 0.0) ||
          !OptionalFiniteNumber(object, "maximumDistanceM", distance, 0.0) ||
          (object.contains("payloadBits") &&
           !NonNegativeInteger(object, "payloadBits", payloadBits)) ||
          (object.contains("minimumNetworkSize") &&
           !NonNegativeInteger(object, "minimumNetworkSize",
                               minimumNetworkSize)))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "资源需求字段无效");
      if (!demandIds.insert(demandId.toStdString()).second)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "资源需求编号重复");

      nrm::ResourceDemand demand;
      demand.demandId = demandId.toStdString();
      demand.demandSetId = demandSet.demandSetId;
      demand.revision = demandSet.revision;
      demand.missionStage =
         object.value("missionStage").toString().toStdString();
      if (demand.missionStage.empty()) demand.missionStage = "UNSPECIFIED";
      demand.businessType =
         object.value("businessType").toString().toStdString();
      demand.sourcePlatform = source.toStdString();
      demand.destinationPlatform = destination.toStdString();
      demand.payloadBits = static_cast<std::uint64_t>(payloadBits);
      demand.businessTrafficBps = traffic;
      demand.requiredBandwidthBps = bandwidth;
      demand.maximumDelayMs = delay;
      demand.minimumPdrPercent = pdr;
      demand.maximumDistanceM = distance;
      demand.minimumNetworkSize = minimumNetworkSize;
      std::set<nrm::NetworkType> networkTypes;
      for (const QJsonValue& value : object.value("allowedNetworks").toArray())
      {
         if (!value.isString())
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/demands/allowedNetworks",
                           "允许网络必须是字符串");
         const nrm::NetworkType type = NetworkType(value.toString());
         if (type == nrm::NetworkType::cUNKNOWN ||
             !networkTypes.insert(type).second)
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/demands/allowedNetworks",
                           "允许网络包含未知或重复类型");
         demand.allowedNetworks.push_back(type);
      }
      demand.source = nrm::DataOrigin::cCUSTOMER_MODULE;
      demand.confidence = nrm::Confidence::cLOW;
      demand.valid = true;
      demandSet.demands.push_back(demand);
   }
   demandSet.valid = true;
   aDemandSet = demandSet;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeNetworkPlan(
   const QByteArray& aJson, nrm::NetworkPlanDocument& aPlan) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.network_plan.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是网络规划消息") : result;
   const QJsonObject data = DataObject(aJson);
   std::size_t revision = 0;
   if (data.value("planId").toString().isEmpty() ||
       !NonNegativeInteger(data, "revision", revision) || revision < 1 ||
       !data.value("allocations").isArray() ||
       data.value("allocations").toArray().isEmpty() ||
       !data.value("demands").isArray() ||
       data.value("demands").toArray().isEmpty())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "网络规划字段无效");

   nrm::NetworkPlanDocument plan;
   plan.planId = data.value("planId").toString().toStdString();
   plan.revision = static_cast<std::uint64_t>(revision);
   // The transport codec does not own the active model configuration. The
   // DataContainer binds the current profile version before plan acceptance.
   plan.configVersion.clear();
   plan.providerId = result.envelope.source;
   plan.createdTime = result.envelope.timestamp;
   const QString planningDomain = data.value("planningDomain").toString("JOINT");
   if (planningDomain == "AIRBORNE") plan.planningDomain = nrm::PlanningDomain::cAIRBORNE;
   else if (planningDomain == "GROUND") plan.planningDomain = nrm::PlanningDomain::cGROUND;
   else if (planningDomain == "JOINT") plan.planningDomain = nrm::PlanningDomain::cJOINT;
   else return Failure("SCHEMA_VALIDATION_FAILED", "/data/planningDomain", "规划域无效");
   for (const QJsonValue& value : data.value("allocations").toArray())
   {
      if (!value.isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations",
                        "资源分配必须是对象");
      const QJsonObject object = value.toObject();
      nrm::NetworkPlanAllocation allocation;
      allocation.allocationId = object.value("allocationId").toString().toStdString();
      allocation.networkName = object.value("networkName").toString().toStdString();
      allocation.networkType = NetworkType(object.value("networkType").toString());
      allocation.profileId = object.value("profileId").toString().toStdString();
      allocation.frequencyHz = object.value("frequencyHz").toDouble();
      allocation.channelId = object.value("channelId").toString().toStdString();
      allocation.subnetId = object.value("subnetId").toString().toStdString();
      allocation.routePolicyId = object.value("routePolicyId").toString().toStdString();
      if (!object.value("frequencyHz").isDouble() ||
          !FiniteNumber(object, "frequencyHz", allocation.frequencyHz, 0.0) ||
          !object.value("enabled").isBool() || !object.value("members").isArray() ||
          (object.contains("slots") && !object.value("slots").isArray()))
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations",
                        "资源分配数值或集合字段无效");
      allocation.enabled = object.value("enabled").toBool();
      std::set<QString> members;
      for (const QJsonValue& member : object.value("members").toArray())
      {
         if (!member.isString() || member.toString().isEmpty() ||
             !members.insert(member.toString()).second)
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations/members",
                           "成员编号无效或重复");
         allocation.memberPlatformIds.push_back(member.toString().toStdString());
      }
      std::set<QString> slots;
      for (const QJsonValue& slot : object.value("slots").toArray())
      {
         if (!slot.isString() || slot.toString().isEmpty() ||
             !slots.insert(slot.toString()).second)
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations/slots",
                           "时隙编号无效或重复");
         allocation.slotIds.push_back(slot.toString().toStdString());
      }
      if (allocation.allocationId.empty() || allocation.networkName.empty() ||
          allocation.networkType == nrm::NetworkType::cUNKNOWN ||
          allocation.profileId.empty() || allocation.channelId.empty() ||
          allocation.subnetId.empty() || allocation.routePolicyId.empty() ||
          allocation.memberPlatformIds.empty())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations",
                        "资源分配记录字段无效");
      plan.allocations.push_back(allocation);
   }
   for (const QJsonValue& value : data.value("demands").toArray())
   {
      if (!value.isObject())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "业务需求必须是对象");
      const QJsonObject object = value.toObject();
      nrm::NetworkPlanDemand demand;
      demand.demandId = object.value("demandId").toString().toStdString();
      demand.businessType = object.value("businessType").toString().toStdString();
      demand.sourcePlatform = object.value("sourcePlatformId").toString().toStdString();
      demand.destinationPlatform = object.value("destinationPlatformId").toString().toStdString();
      std::size_t payloadBits = 0;
      if (!NonNegativeInteger(object, "payloadBits", payloadBits) ||
          !FiniteNumber(object, "requiredBandwidthBps",
                        demand.requiredBandwidthBps, 0.0) ||
          !FiniteNumber(object, "maximumDelayMs", demand.maximumDelayMs, 0.0) ||
          !FiniteNumber(object, "minimumPdrPercent", demand.minimumPdrPercent,
                        0.0, 100.0) ||
          !object.value("allowedNetworks").isArray())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "业务需求数值字段无效");
      demand.payloadBits = static_cast<std::uint64_t>(payloadBits);
      std::set<nrm::NetworkType> allowedNetworkTypes;
      for (const QJsonValue& network : object.value("allowedNetworks").toArray())
      {
         if (!network.isString())
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/demands/allowedNetworks", "网络类型必须是字符串");
         const nrm::NetworkType type = NetworkType(network.toString());
         if (type == nrm::NetworkType::cUNKNOWN ||
             !allowedNetworkTypes.insert(type).second)
            return Failure("SCHEMA_VALIDATION_FAILED",
                           "/data/demands/allowedNetworks",
                           "包含未知或重复网络类型");
         demand.allowedNetworks.push_back(type);
      }
      if (demand.demandId.empty() || demand.businessType.empty() ||
          demand.sourcePlatform.empty() || demand.destinationPlatform.empty() ||
          demand.sourcePlatform == demand.destinationPlatform ||
          demand.allowedNetworks.empty())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "业务需求记录字段无效");
      plan.demands.push_back(demand);
   }
   plan.valid = true;
   aPlan = plan;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeMembership(
   const QByteArray& aJson, nrm::NetworkPlanChange& aChange) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.membership_request.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是成员变更请求") : result;
   const QJsonObject data = DataObject(aJson);
   const QString action = data.value("action").toString();
   nrm::NetworkPlanChange change;
   change.changeId = data.value("requestId").toString().toStdString();
   change.planId = data.value("planId").toString().toStdString();
   change.allocationId = data.value("allocationId").toString().toStdString();
   change.platformId = data.value("platformId").toString().toStdString();
   change.changeType = action == "LEAVE" ? nrm::PlanChangeType::cLEAVE : nrm::PlanChangeType::cJOIN;
   if (change.changeId.empty() || data.value("planId").toString().isEmpty() ||
       change.allocationId.empty() || change.platformId.empty() ||
       (action != "JOIN" && action != "LEAVE"))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "成员变更字段无效");
   aChange = change;
   return result;
}

QByteArray WkNrm::CustomerJsonCodec::EncodeAssessment(
   const CustomerJsonEnvelope& aEnvelope, const nrm::AssessmentResult& aResult) const
{
   QJsonObject data;
   data.insert("taskId", QString::fromStdString(aResult.taskId));
   data.insert("reachable", aResult.reachable);
   data.insert("canEstablish", aResult.canEstablish);
   data.insert("canComplete", aResult.canComplete);
   if (aResult.primaryRoute.size() >= 2) data.insert("primaryRoute", Strings(aResult.primaryRoute));
   if (aResult.backupRoute.size() >= 2) data.insert("backupRoute", Strings(aResult.backupRoute));
   if (aResult.bandwidthMarginBps.valid) data.insert("bandwidthMarginBps", aResult.bandwidthMarginBps.value);
   if (aResult.delayMarginMs.valid) data.insert("delayMarginMs", aResult.delayMarginMs.value);
   if (aResult.reliabilityMarginPercent.valid) data.insert("pdrMarginPercent", aResult.reliabilityMarginPercent.value);
   QJsonArray reasons;
   for (const nrm::AssessmentReason reason : aResult.reasons) reasons.push_back(nrm::ToString(reason));
   data.insert("reasonCodes", reasons);
   data.insert("recommendations", Strings(aResult.recommendations));
   return QJsonDocument(ResponseRoot("nrm.customer.assessment_response.v1", aEnvelope, data))
      .toJson(QJsonDocument::Compact);
}

QByteArray WkNrm::CustomerJsonCodec::EncodeResourceDemandResult(
   const CustomerJsonEnvelope& aEnvelope,
   const nrm::ResourceDemandBatchResult& aResult) const
{
   QJsonObject data;
   data.insert("demandSetId", QString::fromStdString(aResult.demandSetId));
   data.insert("revision", static_cast<qint64>(aResult.revision));
   data.insert("snapshotVersion",
               static_cast<qint64>(aResult.snapshotVersion));
   data.insert("totalCount", static_cast<qint64>(aResult.totalCount));
   data.insert("satisfiedCount",
               static_cast<qint64>(aResult.satisfiedCount));
   data.insert("unsatisfiedCount",
               static_cast<qint64>(aResult.unsatisfiedCount));
   data.insert("dataInvalidCount",
               static_cast<qint64>(aResult.dataInvalidCount));
   QJsonArray results;
   for (const nrm::ResourceDemandMatchResult& result : aResult.results)
   {
      QJsonObject item;
      item.insert("demandId", QString::fromStdString(result.demandId));
      item.insert("status", nrm::ToString(result.status));
      QJsonArray reasons;
      for (const nrm::ResourceDemandReason reason : result.reasons)
         reasons.push_back(nrm::ToString(reason));
      item.insert("reasonCodes", reasons);
      QJsonArray recommendations;
      for (const nrm::PlanningRecommendation& recommendation :
           result.recommendations)
      {
         QJsonObject recommendationObject;
         recommendationObject.insert("type", nrm::ToString(recommendation.type));
         recommendationObject.insert("status",
                                     nrm::ToString(recommendation.status));
         recommendationObject.insert(
            "value", QString::fromStdString(recommendation.value));
         recommendationObject.insert("reason",
                                     nrm::ToString(recommendation.reason));
         recommendations.push_back(recommendationObject);
      }
      item.insert("recommendations", recommendations);
      results.push_back(item);
   }
   data.insert("results", results);
   return QJsonDocument(ResponseRoot(
      "nrm.customer.resource_demand_response.v1", aEnvelope, data))
      .toJson(QJsonDocument::Compact);
}

QByteArray WkNrm::CustomerJsonCodec::EncodePlanResult(
   const CustomerJsonEnvelope& aEnvelope,
   const nrm::NetworkPlanEvaluationResult& aResult,
   const nrm::DistributionPackageResult* aPackage) const
{
   QJsonObject data;
   data.insert("planId", QString::fromStdString(aResult.planId));
   data.insert("revision", static_cast<qint64>(aResult.revision));
   data.insert("validationPassed", aResult.validation.passed);
   data.insert("evaluationStatus", nrm::ToString(aResult.overallStatus));
   data.insert("state", nrm::ToString(aResult.resultingState));
   QJsonArray reasons;
   QJsonArray recommendations;
   for (const nrm::PlanValidationIssue& issue : aResult.validation.issues)
      reasons.push_back(nrm::ToString(issue.reason));
   for (const nrm::PlanDemandEvaluation& demand : aResult.demands)
   {
      for (const nrm::PlanValidationReason reason : demand.reasons)
         reasons.push_back(nrm::ToString(reason));
      for (const std::string& recommendation : demand.recommendations)
         recommendations.push_back(QString::fromStdString(recommendation));
   }
   data.insert("reasonCodes", reasons);
   data.insert("recommendations", recommendations);
   if (aPackage && aPackage->generated && !aPackage->outputPath.empty())
      data.insert("packagePath", QString::fromStdString(aPackage->outputPath));
   return QJsonDocument(ResponseRoot("nrm.customer.network_plan_result.v1", aEnvelope, data))
      .toJson(QJsonDocument::Compact);
}
