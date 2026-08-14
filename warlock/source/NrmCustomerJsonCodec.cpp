#include "NrmCustomerJsonCodec.hpp"

#include <set>
#include <cmath>
#include <map>

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace
{
WkNrm::CustomerJsonDecodeResult Failure(const char* aCode, const char* aPath,
                                        const char* aMessage)
{
   WkNrm::CustomerJsonDecodeResult result;
   result.errors.push_back({aCode, aPath, aMessage});
   return result;
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

WkNrm::CustomerJsonDecodeResult
WkNrm::CustomerJsonCodec::Inspect(const QByteArray& aJson) const
{
   QJsonParseError parseError;
   const QJsonDocument document = QJsonDocument::fromJson(aJson, &parseError);
   if (parseError.error != QJsonParseError::NoError)
      return Failure("INVALID_JSON", "/", "JSON语法错误");
   if (!document.isObject())
      return Failure("SCHEMA_VALIDATION_FAILED", "/", "JSON根节点必须是对象");

   const QJsonObject root = document.object();
   CustomerJsonDecodeResult result;
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
   if (!IsSupportedSchema(schema))
      return Failure("SCHEMA_UNSUPPORTED", "/schema", "不支持的接口Schema");
   const QDateTime timestamp = QDateTime::fromString(root.value("timestamp").toString(),
                                                     Qt::ISODate);
   if (!timestamp.isValid() || timestamp.timeSpec() == Qt::LocalTime)
      return Failure("SCHEMA_VALIDATION_FAILED", "/timestamp",
                     "timestamp必须是包含时区的ISO 8601时间");
   const QString source = root.value("source").toString();
   if (source != "CUSTOMER" && source != "AFSIM" && source != "NRM" &&
       source != "REPLAY")
      return Failure("SCHEMA_VALIDATION_FAILED", "/source", "source枚举无效");

   result.envelope.schema = schema.toStdString();
   result.envelope.messageId = root.value("messageId").toString().toStdString();
   result.envelope.timestamp = root.value("timestamp").toString().toStdString();
   result.envelope.source = source.toStdString();
   result.valid = true;
   return result;
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
      "nrm.customer.network_plan.v1",
      "nrm.customer.network_plan_result.v1",
      "nrm.customer.membership_request.v1",
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
   if (data.value("platformId").toString().isEmpty() || !data.value("simTime").isDouble() ||
       truth.size() != 3 || perceived.size() != 3 || error.size() != 3 ||
       (type != "GNSS" && type != "INS" && type != "INTEGRATED"))
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "导航数据字段无效");
   nrm::NavigationSample sample;
   sample.platformName = data.value("platformId").toString().toStdString();
   sample.rawStatus = status.isEmpty() ? type.toStdString() : status.toStdString();
   sample.mode = type == "INS" ? nrm::NavigationMode::cINS : nrm::NavigationMode::cGPS_ACTIVE;
   sample.statusCode = type == "INS" ? -1 : 1;
   sample.sampleTime = data.value("simTime").toDouble();
   sample.origin = Origin(QString::fromStdString(result.envelope.source));
   sample.confidence = Confidence(data.value("confidence").toString());
   sample.valid = true;
   Metric(sample.truthLatitudeDeg, truth.value("latitudeDeg").toDouble(), "deg", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.truthLongitudeDeg, truth.value("longitudeDeg").toDouble(), "deg", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.truthAltitudeM, truth.value("altitudeM").toDouble(), "m", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.perceivedLatitudeDeg, perceived.value("latitudeDeg").toDouble(), "deg", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.perceivedLongitudeDeg, perceived.value("longitudeDeg").toDouble(), "deg", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.perceivedAltitudeM, perceived.value("altitudeM").toDouble(), "m", sample.sampleTime, sample.origin, sample.confidence);
   const double inTrack = error.value("inTrack").toDouble();
   const double crossTrack = error.value("crossTrack").toDouble();
   const double vertical = error.value("vertical").toDouble();
   Metric(sample.inTrackErrorM, inTrack, "m", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.crossTrackErrorM, crossTrack, "m", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.verticalErrorM, vertical, "m", sample.sampleTime, sample.origin, sample.confidence);
   Metric(sample.totalPositionErrorM, std::sqrt(inTrack * inTrack + crossTrack * crossTrack + vertical * vertical), "m", sample.sampleTime, sample.origin, sample.confidence);
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
   if (data.value("runId").toString().isEmpty() || data.value("regionId").toString().isEmpty() || !data.value("simTime").isDouble())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "环境数据字段无效");
   nrm::EnvironmentSnapshot snapshot;
   nrm::EnvironmentContext context;
   snapshot.providerId = result.envelope.source;
   snapshot.sampleTime = data.value("simTime").toDouble();
   snapshot.origin = Origin(QString::fromStdString(result.envelope.source));
   snapshot.valid = true;
   if (data.contains("terrain")) { snapshot.terrain.available = true; snapshot.terrain.enabled = data.value("terrain").toObject().value("enabled").toBool(); }
   if (data.contains("weather"))
   {
      snapshot.weather.available = true;
      const auto weather = data.value("weather").toObject();
      if (weather.contains("windSpeedMps")) Metric(snapshot.weather.windSpeedMps, weather.value("windSpeedMps").toDouble(), "m/s", snapshot.sampleTime, snapshot.origin);
      if (weather.contains("rainRateMmPerHour")) Metric(snapshot.weather.rainRateMmPerHour, weather.value("rainRateMmPerHour").toDouble(), "mm/h", snapshot.sampleTime, snapshot.origin);
   }
   if (data.contains("astronomy")) { snapshot.celestial.available = true; const auto astronomy=data.value("astronomy").toObject(); if (astronomy.contains("julianDate")) Metric(snapshot.celestial.julianDate, astronomy.value("julianDate").toDouble(), "day", snapshot.sampleTime, snapshot.origin); }
   if (data.contains("interference")) { snapshot.interference.available = true; const auto interference=data.value("interference").toObject(); if (interference.contains("maximumPowerDbm")) Metric(snapshot.interference.maximumPowerDbm, interference.value("maximumPowerDbm").toDouble(), "dBm", snapshot.sampleTime, snapshot.origin); }
   context.contextId = data.value("regionId").toString().toStdString();
   context.schemaVersion = result.envelope.schema;
   context.providerId = result.envelope.source;
   context.sampleTime = snapshot.sampleTime;
   context.origin = snapshot.origin;
   context.valid = true;
   context.applyParameterizedEffects = data.value("applicationMode").toString() == "CANDIDATE_ADJUSTMENT";
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
   if (data.value("runId").toString().isEmpty() || !data.value("simTime").isDouble() ||
       !data.value("networks").isArray() || !data.value("members").isArray() ||
       !data.value("links").isArray())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "资源快照字段无效");

   nrm::ResourceSnapshot snapshot;
   snapshot.simTime = data.value("simTime").toDouble();
   snapshot.runtimeState = nrm::RuntimeState::cRUNNING;
   snapshot.origin = Origin(QString::fromStdString(result.envelope.source));
   snapshot.providerId = result.envelope.source;
   std::map<QString, nrm::NetworkType> networks;
   for (const QJsonValue& value : data.value("networks").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString id = object.value("networkId").toString();
      const nrm::NetworkType type = NetworkType(object.value("networkType").toString());
      if (id.isEmpty() || type == nrm::NetworkType::cUNKNOWN || networks.count(id) != 0)
         return Failure("DATA_INVALID", "/data/networks", "网络ID重复或网络类型无效");
      networks[id] = type;
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
      if (id.isEmpty() || endpoints.count(id) != 0 || networks.count(networkId) == 0)
         return Failure("REFERENCE_NOT_FOUND", "/data/members", "成员ID重复或网络引用不存在");
      nrm::EndpointSnapshot endpoint;
      endpoint.endpointId = id.toStdString();
      endpoint.platformName = object.value("platformId").toString().toStdString();
      endpoint.networkName = networkId.toStdString();
      endpoint.networkType = networks[networkId];
      endpoint.state = State(object.value("state").toString());
      endpoint.canSend = endpoint.state == nrm::ResourceState::cONLINE;
      endpoint.canReceive = endpoint.canSend;
      const QJsonObject position = object.value("position").toObject();
      Metric(endpoint.latitudeDeg, position.value("latitudeDeg").toDouble(), "deg", snapshot.simTime, snapshot.origin);
      Metric(endpoint.longitudeDeg, position.value("longitudeDeg").toDouble(), "deg", snapshot.simTime, snapshot.origin);
      Metric(endpoint.altitudeM, position.value("altitudeM").toDouble(), "m", snapshot.simTime, snapshot.origin);
      endpoints[id] = endpoint;
      snapshot.endpoints.push_back(endpoint);
   }
   std::set<QString> linkIds;
   for (const QJsonValue& value : data.value("links").toArray())
   {
      const QJsonObject object = value.toObject();
      const QString id = object.value("linkId").toString();
      const QString networkId = object.value("networkId").toString();
      const QString sourceId = object.value("sourceMemberId").toString();
      const QString destinationId = object.value("destinationMemberId").toString();
      if (id.isEmpty() || !linkIds.insert(id).second || networks.count(networkId) == 0 ||
          endpoints.count(sourceId) == 0 || endpoints.count(destinationId) == 0 ||
          endpoints[sourceId].networkType != networks[networkId] ||
          endpoints[destinationId].networkType != networks[networkId])
         return Failure("REFERENCE_NOT_FOUND", "/data/links", "链路ID重复或引用不存在");
      nrm::LinkSnapshot link;
      link.linkId = id.toStdString();
      link.sourceEndpointId = sourceId.toStdString();
      link.destinationEndpointId = destinationId.toStdString();
      link.sourcePlatform = endpoints[sourceId].platformName;
      link.destinationPlatform = endpoints[destinationId].platformName;
      link.networkName = networkId.toStdString();
      link.networkType = networks[networkId];
      link.state = State(object.value("state").toString());
      if (object.contains("bandwidthBps")) Metric(link.bandwidthBps, object.value("bandwidthBps").toDouble(), "bit/s", snapshot.simTime, snapshot.origin);
      if (object.contains("rssiDbm")) Metric(link.rssiDbm, object.value("rssiDbm").toDouble(), "dBm", snapshot.simTime, snapshot.origin);
      if (object.contains("snrDb")) Metric(link.snrDb, object.value("snrDb").toDouble(), "dB", snapshot.simTime, snapshot.origin);
      if (object.contains("berRatio")) Metric(link.ber, object.value("berRatio").toDouble(), "ratio", snapshot.simTime, snapshot.origin);
      nrm::WindowMetrics window;
      window.windowS = 0.0;
      if (object.contains("delayMs")) Metric(window.averageTransportDelayMs, object.value("delayMs").toDouble(), "ms", snapshot.simTime, snapshot.origin);
      if (object.contains("pdrPercent")) Metric(window.deliveryRatioPercent, object.value("pdrPercent").toDouble(), "percent", snapshot.simTime, snapshot.origin);
      if (object.contains("trafficBps")) Metric(window.deliveredThroughputBps, object.value("trafficBps").toDouble(), "bit/s", snapshot.simTime, snapshot.origin);
      if (object.contains("queuePercent")) { window.utilizationPercent.valid = false; }
      link.windows.push_back(window);
      snapshot.links.push_back(link);
   }
   for (nrm::NetworkSnapshot& network : snapshot.networks)
   {
      for (const auto& endpoint : snapshot.endpoints) if (endpoint.networkType == network.networkType) { ++network.endpointCount; if (endpoint.state == nrm::ResourceState::cONLINE) ++network.onlineCount; }
      for (const auto& link : snapshot.links) if (link.networkType == network.networkType && link.state == nrm::ResourceState::cONLINE) ++network.activeLinks;
   }
   aSnapshot = snapshot;
   return result;
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
   task.minimumPdrPercent = data.value("minimumPdrPercent").toDouble();
   for (const QJsonValue& value : data.value("allowedNetworks").toArray())
   {
      const nrm::NetworkType type = NetworkType(value.toString());
      if (type == nrm::NetworkType::cUNKNOWN)
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allowedNetworks",
                        "包含未知网络类型");
      task.allowedNetworks.push_back(type);
   }
   if (task.requiredBandwidthBps < 0.0 || task.maximumDelayMs <= 0.0 ||
       task.minimumPdrPercent < 0.0 || task.minimumPdrPercent > 100.0 ||
       task.allowedNetworks.empty())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "任务约束超出允许范围");
   aTask = task;
   return result;
}

WkNrm::CustomerJsonDecodeResult WkNrm::CustomerJsonCodec::DecodeNetworkPlan(
   const QByteArray& aJson, nrm::NetworkPlanDocument& aPlan) const
{
   CustomerJsonDecodeResult result = Inspect(aJson);
   if (!result.valid || result.envelope.schema != "nrm.customer.network_plan.v1")
      return result.valid ? Failure("SCHEMA_UNSUPPORTED", "/schema", "不是网络规划消息") : result;
   const QJsonObject data = DataObject(aJson);
   if (data.value("planId").toString().isEmpty() ||
       data.value("revision").toInt() < 1 || !data.value("allocations").isArray() ||
       !data.value("demands").isArray())
      return Failure("SCHEMA_VALIDATION_FAILED", "/data", "网络规划字段无效");

   nrm::NetworkPlanDocument plan;
   plan.planId = data.value("planId").toString().toStdString();
   plan.revision = static_cast<std::uint64_t>(data.value("revision").toInt());
   plan.configVersion = "customer-json-v1";
   plan.providerId = result.envelope.source;
   plan.createdTime = result.envelope.timestamp;
   for (const QJsonValue& value : data.value("allocations").toArray())
   {
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
      allocation.enabled = object.value("enabled").toBool(true);
      for (const QJsonValue& member : object.value("members").toArray())
         allocation.memberPlatformIds.push_back(member.toString().toStdString());
      for (const QJsonValue& slot : object.value("slots").toArray())
         allocation.slotIds.push_back(slot.toString().toStdString());
      if (allocation.allocationId.empty() || allocation.networkName.empty() ||
          allocation.networkType == nrm::NetworkType::cUNKNOWN ||
          allocation.profileId.empty() || allocation.memberPlatformIds.empty())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/allocations",
                        "资源分配记录字段无效");
      plan.allocations.push_back(allocation);
   }
   for (const QJsonValue& value : data.value("demands").toArray())
   {
      const QJsonObject object = value.toObject();
      nrm::NetworkPlanDemand demand;
      demand.demandId = object.value("demandId").toString().toStdString();
      demand.businessType = object.value("businessType").toString().toStdString();
      demand.sourcePlatform = object.value("sourcePlatformId").toString().toStdString();
      demand.destinationPlatform = object.value("destinationPlatformId").toString().toStdString();
      demand.payloadBits = static_cast<std::uint64_t>(object.value("payloadBits").toDouble());
      demand.requiredBandwidthBps = object.value("requiredBandwidthBps").toDouble();
      demand.maximumDelayMs = object.value("maximumDelayMs").toDouble();
      demand.minimumPdrPercent = object.value("minimumPdrPercent").toDouble();
      for (const QJsonValue& network : object.value("allowedNetworks").toArray())
         demand.allowedNetworks.push_back(NetworkType(network.toString()));
      if (demand.demandId.empty() || demand.businessType.empty() ||
          demand.sourcePlatform.empty() || demand.destinationPlatform.empty() ||
          demand.sourcePlatform == demand.destinationPlatform ||
          demand.allowedNetworks.empty())
         return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands",
                        "业务需求记录字段无效");
      for (const nrm::NetworkType type : demand.allowedNetworks)
         if (type == nrm::NetworkType::cUNKNOWN)
            return Failure("SCHEMA_VALIDATION_FAILED", "/data/demands/allowedNetworks",
                           "包含未知网络类型");
      plan.demands.push_back(demand);
   }
   plan.valid = !plan.allocations.empty();
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
