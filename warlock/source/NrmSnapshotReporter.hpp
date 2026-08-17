#ifndef NRM_SNAPSHOT_REPORTER_HPP
#define NRM_SNAPSHOT_REPORTER_HPP

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "nrm/AssessmentTypes.hpp"
#include "nrm/CommunicationCapabilityTypes.hpp"
#include "nrm/NetworkPlanTypes.hpp"
#include "nrm/NetworkPlanCoordinationService.hpp"
#include "nrm/ResourceDemandTypes.hpp"

namespace WkNrm
{
struct ReporterStatus
{
   std::string runId;
   std::string runDirectory;
   bool healthy = true;
   bool started = false;
   std::uint64_t droppedSnapshotCount = 0;
   std::uint64_t droppedAssessmentCount = 0;
   std::uint64_t droppedCapabilityCount = 0;
   std::uint64_t droppedPlanValidationCount = 0;
   std::uint64_t droppedPlanEvaluationCount = 0;
   std::uint64_t droppedDemandResultCount = 0;
   std::uint64_t droppedPlanningRecommendationCount = 0;
   std::uint64_t droppedDemandFeedbackCount = 0;
   std::uint64_t droppedPlanningCoordinationCount = 0;
   std::uint64_t domainErrorCount = 0;
   std::uint64_t writeErrorCount = 0;
   std::string lastDomainError;
   std::string lastError;
};

class SnapshotReporter
{
public:
   explicit SnapshotReporter(const std::string& aOutputDirectory,
                             const std::string& aConfigVersion = "demo-0.7.0",
                             std::size_t aMaximumQueueSize = 128,
                             bool aAutoStart = true);
   ~SnapshotReporter() noexcept;

   SnapshotReporter(const SnapshotReporter&) = delete;
   SnapshotReporter& operator=(const SnapshotReporter&) = delete;

   void Start();
   void Enqueue(const nrm::ResourceSnapshot& aSnapshot);
   void EnqueueAssessment(const nrm::AssessmentResult& aResult);
   void EnqueueCapability(const nrm::CapabilityResult& aResult);
   void EnqueuePlanValidation(const nrm::PlanValidationResult& aResult);
   void EnqueuePlanEvaluation(const nrm::NetworkPlanEvaluationResult& aResult);
   void EnqueueDemandResults(const nrm::ResourceDemandBatchResult& aResult);
   void EnqueuePlanningRecommendations(
      const nrm::ResourceDemandBatchResult& aResult);
   void EnqueueDemandFeedback(const nrm::ResourceDemandFeedback& aFeedback);
   void EnqueuePlanningCoordination(
      const nrm::PlanCoordinationEvidence& aEvidence);
   void ReportPlanError(nrm::PlanValidationReason aReason,
                        const std::string& aField);
   void ReportDemandError(nrm::ResourceDemandReason aReason,
                          const std::string& aField);
   void ReportCustomerInterfaceEvent(const std::string& aSchema,
                                     const std::string& aMessageId,
                                     const std::string& aFileName,
                                     bool aAccepted,
                                     const std::string& aErrorCode,
                                     const std::string& aErrorPath);
   ReporterStatus GetStatus() const;
   std::string GetRunDirectory() const;

private:
   void Run();
   void RecordError(const std::string& aComponent,
                    nrm::MetricReason aReason,
                    const std::string& aField);
   void RecordDomainError(const std::string& aComponent,
                          const std::string& aReason,
                          const std::string& aField);
   void WriteManifest(bool aComplete);

   std::string                       mOutputDirectory;
   std::string                       mConfigVersion;
   std::size_t                       mMaximumQueueSize;
   std::string                       mStartTimeUtc;
   mutable std::mutex                mMutex;
   std::condition_variable           mCondition;
   std::deque<nrm::ResourceSnapshot> mQueue;
   std::deque<nrm::AssessmentResult> mAssessmentQueue;
   std::deque<nrm::CapabilityResult> mCapabilityQueue;
   std::deque<nrm::PlanValidationResult> mPlanValidationQueue;
   std::deque<nrm::NetworkPlanEvaluationResult> mPlanEvaluationQueue;
   std::deque<nrm::ResourceDemandBatchResult> mDemandResultQueue;
   std::deque<nrm::ResourceDemandBatchResult> mPlanningRecommendationQueue;
   std::deque<nrm::ResourceDemandFeedback> mDemandFeedbackQueue;
   std::deque<nrm::PlanCoordinationEvidence> mPlanningCoordinationQueue;
   std::thread                       mThread;
   bool                              mStopping = false;
   bool                              mStarted = false;
   ReporterStatus                    mStatus;
};
} // namespace WkNrm

#endif
