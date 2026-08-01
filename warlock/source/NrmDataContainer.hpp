#ifndef NRM_DATA_CONTAINER_HPP
#define NRM_DATA_CONTAINER_HPP

#include <memory>
#include <string>

#include <QObject>

#include "nrm/NetworkResourceTypes.hpp"
#include "nrm/AssessmentTypes.hpp"

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
   bool HasAssessment() const { return mHasAssessment; }
   bool IsReportingHealthy() const;
   std::string GetReportingStatus() const;
   void SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot);
   void StoreAssessment(const nrm::AssessmentResult& aResult);

signals:
   void SnapshotChanged();
   void AssessmentChanged();

private:
   nrm::FrameworkSnapshot            mSnapshot;
   nrm::AssessmentResult             mAssessment;
   bool                              mHasAssessment = false;
   std::unique_ptr<SnapshotReporter> mReporterPtr;
};
} // namespace WkNrm

#endif
