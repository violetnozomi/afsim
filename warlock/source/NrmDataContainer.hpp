#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <memory>
#include <string>

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/AssessmentTypes.hpp"
#include "nrm/CommunicationCapabilityService.hpp"

namespace WkNrm
{
class SnapshotReporter;

class DataContainer : public QObject
{
   Q_OBJECT

public:
   explicit DataContainer(QObject* aParentPtr = nullptr);
   ~DataContainer() override;

   const nrm::FrameworkSnapshot& GetSnapshot() const { return mSnapshot; }
   const nrm::AssessmentResult& GetAssessment() const { return mAssessment; }
   const nrm::CapabilityResult& GetCapability() const { return mCapability; }
   bool HasAssessment() const { return mHasAssessment; }
   bool HasCapability() const { return mHasCapability; }
   bool IsReportingHealthy() const;
   std::string GetReportingStatus() const;
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);
   void StoreAssessment(const nrm::AssessmentResult& aResult);
   nrm::CapabilityResult QueryCapability(
      const nrm::CapabilityRequest& aRequest,
      const nrm::EnvironmentContext& aEnvironment = nrm::EnvironmentContext());

signals:
   void SnapshotChanged();
   void AssessmentChanged();
   void CapabilityChanged();

private:
   nrm::FrameworkSnapshot            mSnapshot;
   nrm::AssessmentResult             mAssessment;
   nrm::CapabilityResult             mCapability;
   bool                              mHasAssessment = false;
   bool                              mHasCapability = false;
   nrm::NetworkProfileRepository     mProfiles;
   nrm::CommunicationCapabilityService mCapabilityService;
   std::unique_ptr<SnapshotReporter> mReporterPtr;
};
} // namespace WkNrm

#endif
