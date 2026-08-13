#include "NrmCustomerJsonCodec.hpp"

#include <cassert>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
QByteArray ValidEnvelope()
{
   return R"({"schema":"nrm.customer.assessment_request.v1","messageId":"msg-001","timestamp":"2026-08-13T10:00:00+08:00","source":"CUSTOMER","data":{}})";
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
   return 0;
}
