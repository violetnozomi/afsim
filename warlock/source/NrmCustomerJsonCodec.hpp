#ifndef NRM_CUSTOMER_JSON_CODEC_HPP
#define NRM_CUSTOMER_JSON_CODEC_HPP

// Qt-only JSON file/test/replay codec. Runtime customer integration uses the
// in-process nrm::CustomerNrmAdapter and remains independent of Qt and JSON.

#include <string>
#include <vector>

#include <QByteArray>

#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/AssessmentTypes.hpp"
#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/ResourceDemandTypes.hpp"

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
   std::string runId;
   double simTime = 0.0;
   bool hasSimTime = false;
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

class CustomerJsonValidationLayer
{
public:
   virtual ~CustomerJsonValidationLayer() = default;
   virtual CustomerJsonDecodeResult Validate(const QByteArray& aJson) const = 0;
};

class CustomerJsonCodec
{
public:
   CustomerJsonCodec();
   explicit CustomerJsonCodec(const CustomerJsonValidationLayer& aValidationLayer);
   static const char* AdapterId() { return "nrm.customer.json.v1"; }
   static const char* AdapterVersion() { return "1.0"; }
   static std::vector<std::string> SupportedRequestSchemas()
   {
      return {"nrm.customer.navigation_report.v1",
              "nrm.customer.environment_report.v1",
              "nrm.customer.resource_report.v1",
              "nrm.customer.assessment_request.v1",
              "nrm.customer.network_plan.v1",
              "nrm.customer.membership_request.v1",
              "nrm.customer.provider_hello.v1",
              "nrm.customer.resource_demand_request.v1"};
   }
   static std::vector<std::string> SupportedResponseSchemas()
   {
      return {"nrm.customer.assessment_response.v1",
              "nrm.customer.network_plan_result.v1",
              "nrm.customer.resource_demand_response.v1",
              "nrm.customer.ingest_ack.v1",
              "nrm.customer.error.v1"};
   }
   static bool IsSupportedSchema(const QString& aSchema);
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
   CustomerJsonDecodeResult DecodeResourceDemands(
      const QByteArray& aJson, nrm::ResourceDemandSet& aDemandSet) const;
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
   QByteArray EncodeResourceDemandResult(
      const CustomerJsonEnvelope& aEnvelope,
      const nrm::ResourceDemandBatchResult& aResult) const;
   QByteArray EncodePlanResult(const CustomerJsonEnvelope& aEnvelope,
                               const nrm::NetworkPlanEvaluationResult& aResult,
                               const nrm::DistributionPackageResult* aPackage = nullptr) const;

private:
   const CustomerJsonValidationLayer* mValidationLayer = nullptr;
};

// Compatibility for code compiled against the short-lived adapter name.
using CustomerJsonContractAdapter = CustomerJsonCodec;
}

#endif
