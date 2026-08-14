#ifndef NRM_CUSTOMER_JSON_CODEC_HPP
#define NRM_CUSTOMER_JSON_CODEC_HPP

// Qt-only JSON boundary for customer-modified AFSIM modules. Public NRM model
// objects remain independent of Qt and transport formats.

#include <string>
#include <vector>

#include <QByteArray>

#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/AssessmentTypes.hpp"
#include "nrm/NetworkPlanTypes.hpp"

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

struct CustomerProviderHello
{
   std::string providerId;
   std::string softwareVersion;
   std::vector<std::string> supportedSchemas;
   std::vector<nrm::NetworkType> supportedNetworkTypes;
};

enum class CustomerIngestStatus
{
   cACCEPTED,
   cDUPLICATE,
   cSTALE,
   cREJECTED
};

class CustomerJsonCodec
{
public:
   CustomerJsonDecodeResult Inspect(const QByteArray& aJson) const;
   QByteArray EncodeError(const CustomerJsonEnvelope& aEnvelope,
                          const std::vector<CustomerJsonError>& aErrors) const;
   CustomerJsonDecodeResult DecodeNavigation(const QByteArray& aJson,
                                             nrm::NavigationSample& aSample) const;
   CustomerJsonDecodeResult DecodeEnvironment(const QByteArray& aJson,
                                              nrm::EnvironmentSnapshot& aSnapshot,
                                              nrm::EnvironmentContext& aContext) const;
   CustomerJsonDecodeResult DecodeResources(const QByteArray& aJson,
                                            nrm::ResourceSnapshot& aSnapshot) const;
   CustomerJsonDecodeResult DecodeAssessment(const QByteArray& aJson,
                                             nrm::AssessmentTask& aTask) const;
   CustomerJsonDecodeResult DecodeNetworkPlan(const QByteArray& aJson,
                                              nrm::NetworkPlanDocument& aPlan) const;
   CustomerJsonDecodeResult DecodeMembership(const QByteArray& aJson,
                                             nrm::NetworkPlanChange& aChange) const;
   CustomerJsonDecodeResult DecodeProviderHello(const QByteArray& aJson,
                                                CustomerProviderHello& aHello) const;
   QByteArray EncodeIngestAck(const CustomerJsonEnvelope& aEnvelope,
                              CustomerIngestStatus aStatus,
                              const std::string& aDetail) const;
   QByteArray EncodeAssessment(const CustomerJsonEnvelope& aEnvelope,
                               const nrm::AssessmentResult& aResult) const;
   QByteArray EncodePlanResult(const CustomerJsonEnvelope& aEnvelope,
                               const nrm::NetworkPlanEvaluationResult& aResult,
                               const nrm::DistributionPackageResult* aPackage = nullptr) const;

private:
   static bool IsSupportedSchema(const QString& aSchema);
};
}

#endif
