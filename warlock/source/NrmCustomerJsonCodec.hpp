#ifndef NRM_CUSTOMER_JSON_CODEC_HPP
#define NRM_CUSTOMER_JSON_CODEC_HPP

// Qt-only JSON boundary for customer-modified AFSIM modules. Public NRM model
// objects remain independent of Qt and transport formats.

#include <string>
#include <vector>

#include <QByteArray>

namespace WkNrm
{
struct CustomerJsonError
{
   std::string code;
   std::string path;
   std::string message;
};

struct CustomerJsonEnvelope
{
   std::string schema;
   std::string messageId;
   std::string timestamp;
   std::string source;
};

struct CustomerJsonDecodeResult
{
   bool valid = false;
   CustomerJsonEnvelope envelope;
   std::vector<CustomerJsonError> errors;
};

class CustomerJsonCodec
{
public:
   CustomerJsonDecodeResult Inspect(const QByteArray& aJson) const;
   QByteArray EncodeError(const CustomerJsonEnvelope& aEnvelope,
                          const std::vector<CustomerJsonError>& aErrors) const;

private:
   static bool IsSupportedSchema(const QString& aSchema);
};
}

#endif
