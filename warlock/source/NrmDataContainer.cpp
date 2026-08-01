#include "NrmDataContainer.hpp"

#include <QByteArray>
#include <QDir>

#include "NrmSnapshotReporter.hpp"
#include "nrm/NetworkProfileRepository.hpp"

namespace
{
nrm::NetworkProfileRepository LoadProfiles()
{
   nrm::NetworkProfileRepository profiles =
      nrm::NetworkProfileRepository::BuiltInDemo();
   const QByteArray profilePath = qgetenv("NRM_NETWORK_PROFILE_CONFIG");
   if (!profilePath.isEmpty())
   {
      profiles = nrm::NetworkProfileRepository();
      nrm::NetworkProfileValidation validation;
      profiles.LoadFromFile(profilePath.constData(), validation);
   }
   return profiles;
}
} // namespace

WkNrm::DataContainer::DataContainer(QObject* aParentPtr)
   : QObject(aParentPtr)
   , mProfiles(LoadProfiles())
   , mCapabilityService(mProfiles)
{
   QByteArray outputDirectory = qgetenv("NRM_OUTPUT_DIR");
   if (outputDirectory.isEmpty())
   {
      outputDirectory = "/tmp/nrm-output";
   }
   QDir().mkpath(QString::fromLocal8Bit(outputDirectory));
   const std::string configVersion =
      mProfiles.ConfigVersion().empty() ? "invalid-external-profile"
                                        : mProfiles.ConfigVersion();
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

nrm::CapabilityResult WkNrm::DataContainer::QueryCapability(
   const nrm::CapabilityRequest& aRequest,
   const nrm::EnvironmentContext& aEnvironment)
{
   mCapability = mCapabilityService.Query(mSnapshot, aRequest, aEnvironment);
   mHasCapability = true;
   mReporterPtr->EnqueueCapability(mCapability);
   emit CapabilityChanged();
   return mCapability;
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
   const std::uint64_t allDropped = status.droppedSnapshotCount +
                                    status.droppedAssessmentCount +
                                    status.droppedCapabilityCount;
   if (allDropped > 0)
   {
      result += " dropped=" + std::to_string(allDropped);
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
