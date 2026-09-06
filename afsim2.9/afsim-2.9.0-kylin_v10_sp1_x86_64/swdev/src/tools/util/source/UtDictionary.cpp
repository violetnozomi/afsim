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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtDictionary.hpp"

#include <cassert>
#include <thread>

#include "UtMemory.hpp"
#include "UtMemoryPool.hpp"

// When used with a dictionary, The mem pool acts much like a std::deque.
// This allows for lock-free conversions from id to the associated std::string.
class UtDictionaryMemoryPool : public UtMemoryPool
{
public:
   // pull in constructors
   using UtMemoryPool::UtMemoryPool;

   void PopLastAlloc()
   {
      mBlockPtr->mUsedCount--;
      using std::string;
      static_cast<std::string*>(mBlockPtr->mMemoryPtr)[mBlockPtr->mUsedCount].~string();
   }
};

namespace
{

enum SpinFlag : int32_t
{
   READER       = 4,
   WRITE_QUEUED = 2,
   WRITER       = 1
};

// Read/Write spinlock primitives didn't get put into a seperate utility class
// due to how exceptionally rare it is for them to be more performant than mutexs.
// Also because this spinlock implementation is heavily specialized towards high
// read-contention, as opposed to a more balanced read-write contention.
// Benchmarks showed that the usage of spinlocks is makes the UtDictionary ~30% faster.


//! a shared read lock, that allows multiple concurrent readers to proceed at once.
class ReadSpinLock
{
public:
   ReadSpinLock(std::atomic<int32_t>& aTracker)
      : mTracker(aTracker)
   {
      ObtainReadLock();
   }

   ~ReadSpinLock() { mTracker.fetch_add(-SpinFlag::READER, std::memory_order_release); }

private:
   void ObtainReadLock()
   {
      uint_fast32_t tries = 0;
      while (!TryReadLock())
      {
         ++tries;
         if (tries > 1000)
         {
            std::this_thread::yield();
         }
      }
   }

   bool TryReadLock()
   {
      int32_t val = mTracker.fetch_add(SpinFlag::READER, std::memory_order_acquire);
      if (val & (SpinFlag::WRITER | SpinFlag::WRITE_QUEUED))
      {
         mTracker.fetch_add(-SpinFlag::READER, std::memory_order_release);
         return false;
      }
      return true;
   }

public:
   std::atomic<int32_t>& mTracker;
};

class WriteSpinLock
{
public:
   WriteSpinLock(std::atomic<int32_t>& aTracker)
      : mTracker(aTracker)
   {
      QueueWriteLock();
      UpgradeQueuedLock();
   }

   ~WriteSpinLock() { mTracker.fetch_and(~(SpinFlag::WRITER | SpinFlag::WRITE_QUEUED), std::memory_order_relaxed); }

private:
   void QueueWriteLock()
   {
      uint_fast32_t tries = 0;
      while (!TryQueueWriteLock())
      {
         ++tries;
         if (tries > 1000)
         {
            std::this_thread::yield();
         }
      }
   }

   // transisition from a writer queued to writer
   void UpgradeQueuedLock()
   {
      uint_fast32_t tries = 0;
      while (!TryUpgradeQueuedLock())
      {
         ++tries;
         if (tries > 1000)
         {
            std::this_thread::yield();
         }
      }
   }

   bool TryUpgradeQueuedLock()
   {
      int32_t expect = SpinFlag::WRITE_QUEUED;
      return mTracker.compare_exchange_strong(expect, SpinFlag::WRITER, std::memory_order_acq_rel);
   }

   bool TryQueueWriteLock()
   {
      int32_t val = mTracker.fetch_or(SpinFlag::WRITE_QUEUED, std::memory_order_acquire);

      return ((val & (SpinFlag::WRITE_QUEUED | SpinFlag::WRITER)) == 0);
   }

   std::atomic<int32_t>& mTracker;
};

} // namespace


namespace
{
const std::string sUnknownString("<unknown>");
}

std::unique_ptr<UtDictionary> UtDictionary::mInstancePtr = nullptr;

void UtDictionary::SetDictionary(UtDictionary* aDictionaryPtr)
{
   assert(mInstancePtr.get() != aDictionaryPtr);
   mInstancePtr.reset(aDictionaryPtr);
}


void UtDictionary::Destroy()
{
   mInstancePtr = nullptr;
}

UtDictionary::UtDictionary(size_t aInitialPoolSize /*=500*/)
   : mReadWriteSpinner(0)
   , mPoolPtr(ut::make_unique<UtDictionaryMemoryPool>(sizeof(std::string), aInitialPoolSize))
{
   // Initialize dictionary ID=0 so it refers to the null (empty) string.
   WriteSpinLock write_lock(mReadWriteSpinner);
   mNumberToString.reserve(aInitialPoolSize);
   InsertNew("");
}

// virtual
UtDictionary::~UtDictionary()
{
   for (size_t i = 0; i < mNumberToString.size(); ++i)
   {
      using std::string;
      mNumberToString[i]->~string();
   }
}

//! Return a number that is uniquely associated with a string.
//! @param aString The string whose associated number is to be returned.
//! @returns The number that is uniquely associated with the supplied string.
int UtDictionary::GetNumber(const std::string& aString)
{
   {
      ReadSpinLock      read_lock(mReadWriteSpinner);
      MapType::iterator iter = mStringToNumber.find(&aString);
      if (iter != mStringToNumber.end())
      {
         return iter->second;
      }
   }

   WriteSpinLock write_lock(mReadWriteSpinner);
   return InsertNew(aString);
}

int UtDictionary::InsertNew(const std::string& aString)
{
   int number = static_cast<int>(mNumberToString.size());
   // Using the memory pool, we have to use placement new
   // and explicitly call destructor later
   std::string* newStringPtr = new (mPoolPtr->Alloc()) std::string(aString);
   auto         insertion    = mStringToNumber.insert(std::pair<std::string*, int>(newStringPtr, number));
   if (insertion.second)
   {
      // update total number of strings after it has been alloced/inserted
      // to allow for lock-free GetString()
      mNumberToString.push_back(newStringPtr);
   }
   else
   {
      // exceptionally rare scenario where two threads try to get the id of the same string,
      // and both didn't find it in the map, causing them to both try creating an id for it.
      // Fortunately, insert wont do anything if the key already existed, so we simply need to cleanup
      // the extra alloc, and return the id that was previously given to it.
      mPoolPtr->PopLastAlloc();
      return insertion.first->second;
   }
   return number;
}

bool UtDictionary::Exists(const std::string& aString) const
{
   ReadSpinLock read_lock(mReadWriteSpinner);
   bool         found = mStringToNumber.find(&aString) != mStringToNumber.end();
   return found;
}

//! Return the string that is uniquely associated with the supplied number.
//! @param aNumber The number whose associated string is to be returned.
//! @returns The string associated with the supplied number. If the supplied number was 0
//! then an empty string will be returned. If the supplied number had not yet been associated
//! with a string then '<unknown>' will be returned.
const std::string& UtDictionary::GetString(int aNumber) const
{
   ReadSpinLock read_lock(mReadWriteSpinner);
   if (aNumber < 0 || aNumber >= static_cast<int>(mNumberToString.size()))
   {
      return sUnknownString;
   }
   return *mNumberToString[aNumber];
}

//! Get the number of strings in the dictionary (including the 'null' entry).
int UtDictionary::GetCount() const
{
   ReadSpinLock read_lock(mReadWriteSpinner);
   return static_cast<int>(mNumberToString.size());
}

UtDictionary& UtDictionary::GetDictionaryP()
{
   assert(mInstancePtr == nullptr);
   mInstancePtr = ut::make_unique<UtDictionary>(128 * 1024 / sizeof(std::string)); // 128 kilobytes
   return *mInstancePtr;
}
