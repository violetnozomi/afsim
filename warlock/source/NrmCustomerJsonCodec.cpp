#include "NrmCustomerJsonCodec.hpp"

#include <set>

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
