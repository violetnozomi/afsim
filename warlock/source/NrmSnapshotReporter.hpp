#ifndef NRM_SNAPSHOT_REPORTER_HPP
#define NRM_SNAPSHOT_REPORTER_HPP

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#include "nrm/AssessmentTypes.hpp"

namespace WkNrm
{
class SnapshotReporter
{
public:
   explicit SnapshotReporter(const std::string& aOutputDirectory);
   ~SnapshotReporter();

   SnapshotReporter(const SnapshotReporter&) = delete;
   SnapshotReporter& operator=(const SnapshotReporter&) = delete;

   void Enqueue(const nrm::ResourceSnapshot& aSnapshot);
   void EnqueueAssessment(const nrm::AssessmentResult& aResult);

private:
   void Run();

   std::string                       mOutputDirectory;
   std::mutex                        mMutex;
   std::condition_variable           mCondition;
   std::deque<nrm::ResourceSnapshot> mQueue;
   std::deque<nrm::AssessmentResult>  mAssessmentQueue;
   std::thread                       mThread;
   bool                              mStopping = false;
   static const std::size_t          cMAX_QUEUE_SIZE = 128;
};
} // namespace WkNrm

#endif
