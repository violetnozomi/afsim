/**
 * @file CustomerIngestionState.hpp
 * @brief Bounded duplicate, ordering, and run-transition tracking for customer messages.
 */

#ifndef NRM_CUSTOMER_INGESTION_STATE_HPP
#define NRM_CUSTOMER_INGESTION_STATE_HPP

#include <cstddef>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <utility>

namespace nrm
{
enum class CustomerMessageDomain
{
   cRESOURCE,
   cNAVIGATION,
   cENVIRONMENT,
   cPLAN,
   cASSESSMENT,
   cDEMAND,
   cMEMBERSHIP,
   cPROVIDER
};

enum class CustomerIngestionStatus
{
   cACCEPTED,
   cDUPLICATE,
   cSTALE,
   cREJECTED
};

struct CustomerIngestionDecision
{
   CustomerIngestionStatus status = CustomerIngestionStatus::cACCEPTED;
   bool newRun = false;
};

class CustomerIngestionState
{
public:
   explicit CustomerIngestionState(std::size_t aMaximumMessageIds = 4096)
      : mMaximumMessageIds(aMaximumMessageIds == 0 ? 1 : aMaximumMessageIds)
   {
   }

   CustomerIngestionDecision Accept(const std::string& aRunId,
                                    const std::string& aMessageId,
                                    CustomerMessageDomain aDomain,
                                    bool aHasSimTime,
                                    double aSimTime)
   {
      const CustomerIngestionDecision decision =
         Preview(aRunId, aMessageId, aDomain, aHasSimTime, aSimTime);
      if (decision.status == CustomerIngestionStatus::cACCEPTED)
         Commit(aRunId, aMessageId, aDomain, aHasSimTime, aSimTime);
      return decision;
   }

   CustomerIngestionDecision Preview(const std::string& aRunId,
                                     const std::string& aMessageId,
                                     CustomerMessageDomain aDomain,
                                     bool aHasSimTime,
                                     double aSimTime) const
   {
      CustomerIngestionDecision decision;
      const MessageKey key(aRunId, aMessageId);
      if (!aMessageId.empty() && mSeenMessageIds.count(key) != 0)
      {
         decision.status = CustomerIngestionStatus::cDUPLICATE;
         return decision;
      }

      if (!aRunId.empty())
      {
         if (mRetiredRunIds.count(aRunId) != 0)
         {
            decision.status = CustomerIngestionStatus::cSTALE;
            return decision;
         }
         if (!mCurrentRunId.empty() && mCurrentRunId != aRunId)
         {
            decision.newRun = true;
         }
      }

      if (aHasSimTime && !decision.newRun)
      {
         const auto found = mLastSimTimes.find(aDomain);
         if (found != mLastSimTimes.end() && aSimTime < found->second)
         {
            decision.status = CustomerIngestionStatus::cSTALE;
            return decision;
         }
      }
      return decision;
   }

   void Commit(const std::string& aRunId,
               const std::string& aMessageId,
               CustomerMessageDomain aDomain,
               bool aHasSimTime,
               double aSimTime)
   {
      if (!aRunId.empty())
      {
         if (mCurrentRunId.empty())
            mCurrentRunId = aRunId;
         else if (mCurrentRunId != aRunId)
         {
            mRetiredRunIds.insert(mCurrentRunId);
            mCurrentRunId = aRunId;
            mLastSimTimes.clear();
         }
      }
      if (aHasSimTime) mLastSimTimes[aDomain] = aSimTime;
      Remember(MessageKey(aRunId, aMessageId));
   }

   const std::string& CurrentRunId() const { return mCurrentRunId; }

private:
   using MessageKey = std::pair<std::string, std::string>;

   void Remember(const MessageKey& aKey)
   {
      if (aKey.second.empty()) return;
      mSeenMessageIds.insert(aKey);
      mMessageOrder.push_back(aKey);
      while (mMessageOrder.size() > mMaximumMessageIds)
      {
         mSeenMessageIds.erase(mMessageOrder.front());
         mMessageOrder.pop_front();
      }
   }

   std::size_t mMaximumMessageIds;
   std::string mCurrentRunId;
   std::set<std::string> mRetiredRunIds;
   std::set<MessageKey> mSeenMessageIds;
   std::deque<MessageKey> mMessageOrder;
   std::map<CustomerMessageDomain, double> mLastSimTimes;
};
} // namespace nrm

#endif
