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

#include "WsfCommQueue.hpp"

#include <algorithm>
#include <memory>

#include "UtCast.hpp"

namespace wsf
{
namespace comm
{

// ==========================================================================
bool Queue::Entry::operator==(const Entry& aRhs) const
{
   return Message::Identifier(*mMessage) == Message::Identifier(*aRhs.mMessage);
}

// ==========================================================================
void Queue::Entry::SetMessage(Message* aMessagePtr)
{
   if (aMessagePtr)
   {
      mMessage = ut::clone(aMessagePtr);
   }
}

// ==========================================================================
bool Queue::Push(const Entry& aEntry)
{
   // Note: The default constructor for Entry objects sets the status to
   // cQUEUED. It is not imposed by this method.
   bool ok = false;
   if (mQueue.size() < mLimit)
   {
      //! Ensure the entry has valid data. This avoids repetitive validity checks,
      //! and can also aid in debugging errors in logic prior to reception at
      //! the queue.
      if (!aEntry.GetMessage() || !aEntry.GetMessage()->SourceMessage())
      {
         throw std::logic_error("Invalid attempt to add member to queue without message members.");
      }

      mQueue.emplace_back(aEntry);
      ok = true;
   }

   return ok;
}

// ==========================================================================
void Queue::Pop()
{
   // Note: 'Popping' only requires changing the status of the entry to
   // cPROCESSED, as it becomes invalid for subsequent 'Pops'.
   if (!EmptyActiveEntries())
   {
      switch (mQueueType)
      {
      case QueueType::cFIFO:
      {
         for (auto it = std::begin(mQueue); it != std::end(mQueue); ++it)
         {
            if (it->GetStatus() == Entry::EntryStatus::cQUEUED)
            {
               it->SetStatus(Entry::EntryStatus::cPROCESSED);
               return;
            }
         }
         break;
      }
      case QueueType::cLIFO:
      {
         for (auto it = mQueue.rbegin(); it != mQueue.rend(); ++it)
         // for (auto it = std::rbegin(mQueue); it != std::rend(mQueue); ++it) // C++14
         {
            if (it->GetStatus() == Entry::EntryStatus::cQUEUED)
            {
               it->SetStatus(Entry::EntryStatus::cPROCESSED);
               return;
            }
         }
         break;
      }
      case QueueType::cPRIORITY:
      {
         // Create a subset of only valid entries.
         auto queueCopy = mQueue;
         queueCopy.erase(std::remove_if(std::begin(queueCopy),
                                        std::end(queueCopy),
                                        [](const Entry& aEntry)
                                        { return (aEntry.GetStatus() != Entry::EntryStatus::cQUEUED); }),
                         std::end(queueCopy));

         //! Always valid iterator due to initial check of non-empty queue
         auto it = std::max_element(std::begin(queueCopy),
                                    std::end(queueCopy),
                                    [](const Entry& aLhs, const Entry& aRhs) {
                                       return aLhs.GetMessage()->SourceMessage()->GetPriority() <
                                              aRhs.GetMessage()->SourceMessage()->GetPriority();
                                    });

         Message::Identifier identifier(*it->GetMessage());
         EntryProcessed(identifier);
         break;
      }
      default:
      {
         break;
      }
      }
   }
}

// ==========================================================================
const Queue::Entry& Queue::Top() const
{
   //! Check for non-empty std::list needed to avoid undefined behavior.
   if (!EmptyActiveEntries())
   {
      switch (mQueueType)
      {
      case QueueType::cFIFO:
      {
         for (auto it = std::begin(mQueue); it != std::end(mQueue); ++it)
         {
            if (it->GetStatus() == Entry::EntryStatus::cQUEUED)
            {
               return *it;
            }
         }

         break;
      }
      case QueueType::cLIFO:
      {
         for (auto it = mQueue.rbegin(); it != mQueue.rend(); ++it)
         // for (auto it = std::rbegin(mQueue); it != std::rend(mQueue); ++it) // C++14
         {
            if (it->GetStatus() == Entry::EntryStatus::cQUEUED)
            {
               return *it;
            }
         }

         break;
      }
      case QueueType::cPRIORITY:
      {
         // Create a subset of only valid entries.
         auto queueCopy = mQueue;
         queueCopy.erase(std::remove_if(std::begin(queueCopy),
                                        std::end(queueCopy),
                                        [](const Entry& aEntry)
                                        { return (aEntry.GetStatus() != Entry::EntryStatus::cQUEUED); }),
                         std::end(queueCopy));

         //! Always valid iterator due to initial check of non-empty queue
         auto it = std::max_element(std::begin(queueCopy),
                                    std::end(queueCopy),
                                    [](const Entry& aLhs, const Entry& aRhs) {
                                       return aLhs.GetMessage()->SourceMessage()->GetPriority() <
                                              aRhs.GetMessage()->SourceMessage()->GetPriority();
                                    });

         Message::Identifier identifier(*it->GetMessage());
         return *Peek(identifier);
      }
      default:
      {
         throw std::logic_error("Top() called on an invalid queue type.");
      }
      }
   }

   throw std::logic_error("Top() called on an empty queue.");
}

// ==========================================================================
const Queue::Entry* Queue::Peek(const Message::Identifier& aIdentifier) const
{
   auto it = std::find_if(std::begin(mQueue),
                          std::end(mQueue),
                          [aIdentifier](const Entry& aEntry)
                          { return Message::Identifier(*aEntry.GetMessage()) == aIdentifier; });

   if (it != std::end(mQueue))
   {
      return &(*it);
   }

   return nullptr;
}

// ==========================================================================
Queue::Entry* Queue::Peek(const Message::Identifier& aIdentifier)
{
   return const_cast<Entry*>(const_cast<const Queue*>(this)->Peek(aIdentifier));
}

// ==========================================================================
size_t Queue::SizeActiveEntries() const
{
   auto count = std::count_if(std::begin(mQueue),
                              std::end(mQueue),
                              [](const Entry& aEntry) { return (aEntry.GetStatus() == Entry::EntryStatus::cQUEUED); });

   return ut::safe_cast<size_t>(count);
}

// ==========================================================================
bool Queue::EmptyActiveEntries() const
{
   return (SizeActiveEntries() == 0U);
}

// ==========================================================================
std::vector<Queue::Entry> Queue::Remove(double aTime)
{
   std::vector<Entry> removedEntries;
   auto               it = std::stable_partition(std::begin(mQueue),
                                   std::end(mQueue),
                                   [aTime](const Entry& entry) {
                                      return ((entry.GetQueueTime() > aTime) ||
                                              (entry.GetStatus() != Entry::EntryStatus::cQUEUED));
                                   });

   if (it != std::end(mQueue))
   {
      removedEntries.assign(it, std::end(mQueue));
      mQueue.erase(it, std::end(mQueue));
   }

   return removedEntries;
}

// ==========================================================================
bool Queue::EntryProcessed(const Message::Identifier& aIdentifier)
{
   auto entryPtr = Peek(aIdentifier);
   if (!entryPtr)
   {
      return false;
   }

   if (entryPtr->GetStatus() == Entry::EntryStatus::cQUEUED)
   {
      entryPtr->SetStatus(Entry::EntryStatus::cPROCESSED);
      return true;
   }

   return false;
}

// ==========================================================================
bool Queue::EntryReprocessed(double aSimTime, const Message::Identifier& aIdentifier)
{
   auto entryPtr = Peek(aIdentifier);
   if (!entryPtr)
   {
      return false;
   }

   if (entryPtr->GetStatus() == Entry::EntryStatus::cPROCESSED)
   {
      entryPtr->SetStatus(Entry::EntryStatus::cREPROCESSED);
      entryPtr->SetQueueTime(aSimTime);
      return true;
   }

   return false;
}

// ==========================================================================
bool Queue::EntryDropped(const Message::Identifier& aIdentifier)
{
   auto it = std::find_if(std::begin(mQueue),
                          std::end(mQueue),
                          [aIdentifier](const Entry& aEntry)
                          { return Message::Identifier(*aEntry.GetMessage()) == aIdentifier; });

   if (it != std::end(mQueue))
   {
      mQueue.erase(it);
      return true;
   }

   return false;
}

} // namespace comm
} // namespace wsf
