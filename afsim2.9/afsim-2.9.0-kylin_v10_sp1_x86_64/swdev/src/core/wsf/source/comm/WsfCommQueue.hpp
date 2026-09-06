// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMQUEUE_HPP
#define WSFCOMMQUEUE_HPP

#include "wsf_export.h"

#include <limits>
#include <list>
#include <memory>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "WsfCommMessage.hpp"

namespace wsf
{
namespace comm
{

//! This class provides a 'queue-like' container for holding comm messages,
//! specifically intended for usage by the datalink layer. Messages are
//! intended to be processed in a typical queue implementation of indicated
//! type. However, messages may have different lifetimes. As such, this container
//! must differentiate messages based on their status, and must have visibility
//! into the container beyond the top entry.
class WSF_EXPORT Queue
{
public:
   //! Queue Types
   enum class QueueType : size_t
   {
      cFIFO,    // First-in-first-out
      cLIFO,    // Last-in-last-out
      cPRIORITY // Assigned priority queue
   };

   class Entry
   {
   public:
      //! Entry States.
      enum class EntryStatus : size_t
      {
         cQUEUED,     // Initial push to queue. No other action taken.
         cPROCESSED,  // Popped. Awaiting feedback. Ignored during subsequent pops.
         cREPROCESSED // A special status similar to cPROCESSED. For datalink layer usage,
                      // indicates a message is scheduled for retransmission, but not eligible
                      // for typical queue processing.
      };

      Entry()                  = default;
      virtual ~Entry()         = default;
      Entry(const Entry& aSrc) = default;

      bool   operator==(const Entry& aRhs) const;
      Entry& operator=(const Entry& aRhs) = default;

      double      GetQueueTime() const { return mQueueTime; }
      void        SetQueueTime(double aSimTime) { mQueueTime = aSimTime; }
      Message*    GetMessage() const { return mMessage.get(); }
      void        SetMessage(Message* aMessagePtr);
      EntryStatus GetStatus() const { return mStatus; }
      void        SetStatus(EntryStatus aStatus) { mStatus = aStatus; }

      size_t GetCounter() const { return mCounter; }
      void   SetCounter(size_t aCount) { mCounter = aCount; }
      void   IncrementCounter() { ++mCounter; }
      void   DecrementCounter() { --mCounter; }
      void   ResetCounter() { mCounter = 0; }

   private:
      double                    mQueueTime{0.0};
      ut::CloneablePtr<Message> mMessage{nullptr};
      size_t                    mCounter{0};
      EntryStatus               mStatus{EntryStatus::cQUEUED};
   };

   Queue()          = default;
   virtual ~Queue() = default;

   bool Push(const Entry& aEntry);

   //! Pop only removes those entries in a state that qualifies,
   //! i.e. those with a status of cQUEUED.
   //! It does not actually remove the entry, the status of the
   //! entry is changed to make it ineligible for future pops.
   void Pop();

   //! Top only provides the next entry (depending on queue type)
   //! in a state valid for processing, i.e. cQUEUED.
   const Entry& Top() const;

   const Entry* Peek(const Message::Identifier& aIdentifier) const;
   Entry*       Peek(const Message::Identifier& aIdentifier);

   //! The number of entries, regardless of status.
   size_t Size() const { return mQueue.size(); }

   //! Provides the size of entries able to be 'popped',
   //! i.e. those with a status of cQUEUED.
   size_t SizeActiveEntries() const;

   //! If the queue is empty, regardless of entry status.
   bool Empty() const { return mQueue.empty(); }

   //! Indicates presence of any entries able to be 'popped',
   //! i.e. those with a status of cQUEUED.
   bool EmptyActiveEntries() const;

   QueueType GetQueueType() const { return mQueueType; }
   void      SetQueueType(QueueType aType) { mQueueType = aType; }

   //! Limits apply to all entries, regardless of status.
   //@{
   size_t GetLimit() const { return mLimit; }
   void   SetLimit(size_t aLimit) { mLimit = aLimit; }
   //@}

   std::vector<Entry> Remove(double aTime);

   //! Status call methods
   //! After initial addition of entries to the queue, members may
   //! be internally updated based on their status.
   //@{

   //! Changes an entry of status cQUEUED to cPROCESSED. Done automatically when popped,
   //! but available for other use cases.
   bool EntryProcessed(const Message::Identifier& aIdentifier);

   //! Changes an entry of status cPROCESSED to cREPROCESSED. Called externally,
   //! procedurally the same as cPROCESSED within the queue. Requires provision
   //! of current time to reset the timer for this message.
   bool EntryReprocessed(double aSimTime, const Message::Identifier& aIdentifier);

   //! Completely removes an entry regardless of status. Can be called when
   //! a message is guaranteed to be done with processing, or is discarded.
   bool EntryDropped(const Message::Identifier& aIdentifier);
   //@}

private:
   QueueType        mQueueType{QueueType::cFIFO};
   size_t           mLimit{std::numeric_limits<size_t>::max()};
   std::list<Entry> mQueue{};
};

} // namespace comm
} // namespace wsf

#endif
