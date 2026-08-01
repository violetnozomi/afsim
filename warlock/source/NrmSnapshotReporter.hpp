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
   std::uint64_t writeErrorCount = 0;
   std::string lastError;
};

class SnapshotReporter
{
public:
   explicit SnapshotReporter(const std::string& aOutputDirectory,
                             const std::string& aConfigVersion = "demo-0.7.0",
                             std::size_t aMaximumQueueSize = 128,
                             bool aAutoStart = true);
   ~SnapshotReporter();

   SnapshotReporter(const SnapshotReporter&) = delete;
   SnapshotReporter& operator=(const SnapshotReporter&) = delete;

   void Start();
   void Enqueue(const nrm::ResourceSnapshot& aSnapshot);
   void EnqueueAssessment(const nrm::AssessmentResult& aResult);
   ReporterStatus GetStatus() const;
   std::string GetRunDirectory() const;

private:
   void Run();
   void RecordError(const std::string& aComponent,
                    nrm::MetricReason aReason,
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
   std::thread                       mThread;
   bool                              mStopping = false;
   bool                              mStarted = false;
   ReporterStatus                    mStatus;
};
} // namespace WkNrm

#endif
