#include "NrmDataContainer.hpp"

#include <QByteArray>
#include <QDir>

#include "NrmSnapshotReporter.hpp"
#include "nrm/NetworkProfileRepository.hpp"

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
{
   QByteArray outputDirectory = qgetenv("NRM_OUTPUT_DIR");
   if (outputDirectory.isEmpty())
   {
      outputDirectory = "/tmp/nrm-output";
   }
   QDir().mkpath(QString::fromLocal8Bit(outputDirectory));
   nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const QByteArray profilePath = qgetenv("NRM_NETWORK_PROFILE_CONFIG");
   if (!profilePath.isEmpty())
   {
      profiles = nrm::NetworkProfileRepository();
      nrm::NetworkProfileValidation validation;
      profiles.LoadFromFile(profilePath.constData(), validation);
   }
   const std::string configVersion =
      profiles.ConfigVersion().empty() ? "invalid-external-profile"
                                       : profiles.ConfigVersion();
   mReporterPtr.reset(new SnapshotReporter(outputDirectory.constData(), configVersion));
}

WkNrm::DataContainer::~DataContainer() = default;

void WkNrm::DataContainer::SetSnapshot(const nrm::FrameworkSnapshot& aSnapshot)
{
   mSnapshot = aSnapshot;
   mReporterPtr->Enqueue(mSnapshot);
   emit SnapshotChanged();
}

void WkNrm::DataContainer::StoreAssessment(const nrm::AssessmentResult& aResult)
{
   mAssessment = aResult;
   mHasAssessment = true;
   mReporterPtr->EnqueueAssessment(aResult);
   emit AssessmentChanged();
}

bool WkNrm::DataContainer::IsReportingHealthy() const
{
   return mReporterPtr && mReporterPtr->GetStatus().healthy;
}

std::string WkNrm::DataContainer::GetReportingStatus() const
{
   if (!mReporterPtr)
   {
      return "REPORTER_NOT_INITIALIZED";
   }
   const ReporterStatus status = mReporterPtr->GetStatus();
   std::string result = status.healthy ? "OK" : "ERROR";
   if (!status.started)
   {
      result += " (starting)";
   }
   const std::uint64_t dropped =
      status.droppedSnapshotCount + status.droppedAssessmentCount;
   if (dropped > 0)
   {
      result += " dropped=" + std::to_string(dropped);
   }
   if (status.writeErrorCount > 0)
   {
      result += " errors=" + std::to_string(status.writeErrorCount);
   }
   if (!status.lastError.empty())
   {
      result += " " + status.lastError;
   }
   return result;
}
