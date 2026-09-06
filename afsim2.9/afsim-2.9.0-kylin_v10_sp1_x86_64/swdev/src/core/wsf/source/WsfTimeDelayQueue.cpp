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

#include "WsfTimeDelayQueue.hpp"

#include <cassert>
#include <climits>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfEvent.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
//! Copy constructor.
WsfTimeDelayQueue::WsfTimeDelayQueue(const WsfTimeDelayQueue& aSrc)
   : mPlatformPtr(nullptr)
   , mNumberOfServers(aSrc.mNumberOfServers)
   , mServers()
   , mQueuingMethod(aSrc.mQueuingMethod)
   , mPendingQueuePtr(nullptr)
   , mEventEpoch(1)
{
}

// =================================================================================================
//! Destructor.
// virtual
WsfTimeDelayQueue::~WsfTimeDelayQueue()
{
   CleanUp();
   delete mPendingQueuePtr;
}

// =================================================================================================
//! Clean up all internal memory.
// private virtual
void WsfTimeDelayQueue::CleanUp()
{
   if (mPendingQueuePtr != nullptr)
   {
      mPendingQueuePtr->Purge();
   }
}

// =================================================================================================
//! Create a clone of this object.
// virtual
WsfTimeDelayQueue* WsfTimeDelayQueue::Clone() const
{
   return new WsfTimeDelayQueue(*this);
}

// =================================================================================================
//! Complete the processing of a request.
//!
//! @param aSimTime     The current simulation time.
//! @param aRequestPtr  Pointer to the request that is being completed.
//! @param aServerIndex The index of the server that is completing its request.
//!
//! @return A pointer to the request to be processed. If zero then there is no more work for the
//! server to do. If the pointer is non-zero then the request is either the next unprocessed element
//! from the request queue, or is the same request if the request extended its processing time.
// virtual
WsfTimeDelayQueue::Request* WsfTimeDelayQueue::CompleteRequest(double aSimTime, Request* aRequestPtr, size_t aServerIndex)
{
   if (aRequestPtr == nullptr) // Should not happen
   {
      return nullptr;
   }

   Request* requestPtr = aRequestPtr;
   if (requestPtr->CompleteRequest(aSimTime))
   {
      // Request is complete.  Delete the request and assign the next request on the queue (if any).
      delete requestPtr;
      requestPtr = nullptr;
      if (mPendingQueuePtr != nullptr)
      {
         requestPtr = mPendingQueuePtr->Pop();
      }
   }

   if (!mServers.empty())
   {
      // Assign continuing request to the server or mark server as available.
      mServers[aServerIndex] = requestPtr;
   }
   return requestPtr;
}

// =================================================================================================
//! Initialize the object.
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr The platform that owns the request processor.
//! @param aPlatformPartPtr The platform part that owns the request processor (may be zero)
//! @param aContextPtr  Pointer to the script context used for resolving 'reference values' (e.g.:
//! 'number_of_servers /variable NUMBER_OF_SERVERS'). A null pointer may be provided, in which case
//! the global script context is used.
//! @returns true if successful and false if not.
// virtual
bool WsfTimeDelayQueue::Initialize(double            aSimTime,
                                   WsfPlatform*      aPlatformPtr,
                                   WsfPlatformPart*  aPlatformPartPtr,
                                   WsfScriptContext* aContextPtr)
{
   bool ok = true;

   mPlatformPtr = aPlatformPtr;

   delete mPendingQueuePtr;
   mPendingQueuePtr = nullptr;
   if (mQueuingMethod == cFIRST_IN_FIRST_OUT)
   {
      mPendingQueuePtr = new FIFO_Queue();
   }
   else if (mQueuingMethod == cLAST_IN_FIRST_OUT)
   {
      mPendingQueuePtr = new LIFO_Queue();
   }
   else if (mQueuingMethod == cPRIORITY)
   {
      mPendingQueuePtr = new PriorityQueue();
   }
   else if (mQueuingMethod == cNONE)
   {
      mPendingQueuePtr = nullptr;
   }
   else
   {
      { // RAII block
         auto out = ut::log::error() << "Invalid queuing_method.";
         out.AddNote() << "Method: " << mQueuingMethod;
      }
      assert(mQueuingMethod == cNONE);
      ok = false;
   }

   WsfComponent* objectPtr = aPlatformPartPtr;
   if (objectPtr == nullptr)
   {
      objectPtr = aPlatformPtr;
   }
   ok &= mNumberOfServers.Initialize("number_of_servers",
                                     objectPtr,
                                     aContextPtr,
                                     mPlatformPtr->GetSimulation()->GetScriptContext());

   mServers.clear();
   if ((mNumberOfServers > 0) && (mNumberOfServers < std::numeric_limits<int>::max()))
   {
      mServers.resize(mNumberOfServers);
   }

   return ok;
}

// =================================================================================================
// virtual
bool WsfTimeDelayQueue::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "queuing_method")
   {
      std::string queuingMethod;
      aInput.ReadValue(queuingMethod);
      if ((queuingMethod == "first_in_first_out") || (queuingMethod == "fifo"))
      {
         mQueuingMethod = cFIRST_IN_FIRST_OUT;
      }
      else if ((queuingMethod == "last_in_first_out") || (queuingMethod == "lifo"))
      {
         mQueuingMethod = cLAST_IN_FIRST_OUT;
      }
      else if (queuingMethod == "priority")
      {
         mQueuingMethod = cPRIORITY;
      }
      else if (queuingMethod == "none")
      {
         mQueuingMethod = cNONE;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "number_of_servers")
   {
      mNumberOfServers = 0;
      std::string value;
      aInput.ReadValue(value);
      if (value != "infinite")
      {
         aInput.PushBack(value);
         mNumberOfServers.ReadValue(aInput);
         mNumberOfServers.ValueGreaterOrEqual(aInput, 0); // A value of zero implies infinite
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Set the number of servers available to process requests.
//! This is essentially the number of requests that can be processed simultaneously.
//!
//! @param aNumberOfServers The number of servers. A value less than or equal to zero or >=
//! std::numeric_limits<int>::max() will be treated as infinite.
//!
//! @note This is only effective prior to Initialize being called.
void WsfTimeDelayQueue::SetNumberOfServers(int aNumberOfServers)
{
   if (mPlatformPtr == nullptr) // Initialize has not been called
   {
      mNumberOfServers = aNumberOfServers;
   }
}

// =================================================================================================
//! Submit a request for processing by the next available processor.
//! @param aSimTime    The current simulation time.
//! @param aRequestPtr The request to be processed.
// virtual
void WsfTimeDelayQueue::SubmitRequest(double aSimTime, Request* aRequestPtr)
{
   double delayTime = aRequestPtr->TimeRequired();
   assert(delayTime >= 0.0);

   if (mServers.empty())
   {
      // number_of_servers infinite
      GetSimulation()->AddEvent(ut::make_unique<CompleteRequestEvent>(aSimTime + delayTime, aRequestPtr, this, 0));
   }
   else
   {
      // Attempt to find an available server.
      unsigned int serverIndex;
      for (serverIndex = 0; serverIndex < mServers.size(); ++serverIndex)
      {
         if (mServers[serverIndex] == nullptr)
         {
            break;
         }
      }

      if (serverIndex < mServers.size())
      {
         // Found an available entry. Assign the request to the server and schedule the event.
         mServers[serverIndex] = aRequestPtr;
         GetSimulation()->AddEvent(
            ut::make_unique<CompleteRequestEvent>(aSimTime + delayTime, aRequestPtr, this, serverIndex));
      }
      else
      {
         // No available server. Put the request on the pending queue.
         if (mQueuingMethod != cNONE)
         {
            mPendingQueuePtr->Push(aRequestPtr);
         }
      }
   }
}

// =================================================================================================
//! Indicate that the request processor has been turned off.
//! @param aSimTime The current simulation time.
// virtual
void WsfTimeDelayQueue::TurnOff(double aSimTime)
{
   CleanUp();
   ++mEventEpoch; // Increment the epoch so obsolete events will get ignored.
}

// =================================================================================================
//! Indicate that the request processor has been turned on.
//! @param aSimTime The current simulation time.
// virtual
void WsfTimeDelayQueue::TurnOn(double aSimTime)
{
   ++mEventEpoch; // Increment the epoch so obsolete events will get ignored.
}

WsfSimulation* WsfTimeDelayQueue::GetSimulation() const
{
   return mPlatformPtr->GetSimulation();
}

// =================================================================================================
// Nested class FIFO_Queue
// =================================================================================================

// =================================================================================================
// virtual
void WsfTimeDelayQueue::FIFO_Queue::Purge()
{
   while (!mQueue.empty())
   {
      Request* requestPtr = mQueue.front();
      mQueue.pop_front();
      delete requestPtr;
   }
}

// =================================================================================================
// virtual
void WsfTimeDelayQueue::FIFO_Queue::Push(Request* aRequestPtr)
{
   mQueue.push_back(aRequestPtr);
}
// =================================================================================================
// virtual
WsfTimeDelayQueue::Request* WsfTimeDelayQueue::FIFO_Queue::Pop()
{
   WsfTimeDelayQueue::Request* requestPtr = nullptr;
   if (!mQueue.empty())
   {
      requestPtr = mQueue.front();
      mQueue.pop_front();
   }
   return requestPtr;
}

// =================================================================================================
// Nested class LIFO_Queue
// =================================================================================================

// virtual
void WsfTimeDelayQueue::LIFO_Queue::Purge()
{
   while (!mQueue.empty())
   {
      Request* requestPtr = mQueue.back();
      mQueue.pop_back();
      delete requestPtr;
   }
}

// virtual
void WsfTimeDelayQueue::LIFO_Queue::Push(Request* aRequestPtr)
{
   mQueue.push_back(aRequestPtr);
}

// virtual
WsfTimeDelayQueue::Request* WsfTimeDelayQueue::LIFO_Queue::Pop()
{
   WsfTimeDelayQueue::Request* requestPtr = nullptr;
   if (!mQueue.empty())
   {
      requestPtr = mQueue.back();
      mQueue.pop_back();
   }
   return requestPtr;
}

// =================================================================================================
// Nested class PriorityQueue
// =================================================================================================

// =================================================================================================
// virtual
void WsfTimeDelayQueue::PriorityQueue::Purge()
{
   while (!mQueue.empty())
   {
      Request* requestPtr = mQueue.top();
      mQueue.pop();
      delete requestPtr;
   }
}

// =================================================================================================
// virtual
void WsfTimeDelayQueue::PriorityQueue::Push(Request* aRequestPtr)
{
   mQueue.push(aRequestPtr);
}

// =================================================================================================
// virtual
WsfTimeDelayQueue::Request* WsfTimeDelayQueue::PriorityQueue::Pop()
{
   WsfTimeDelayQueue::Request* requestPtr = nullptr;
   if (!mQueue.empty())
   {
      requestPtr = mQueue.top();
      mQueue.pop();
   }
   return requestPtr;
}

// =================================================================================================
// Nested class CompleteRequestEvent
// =================================================================================================

// =================================================================================================
WsfTimeDelayQueue::CompleteRequestEvent::CompleteRequestEvent(double             aSimTime,
                                                              Request*           aRequestPtr,
                                                              WsfTimeDelayQueue* aQueuePtr,
                                                              size_t             aServerIndex)
   : WsfEvent(aSimTime)
   , mRequestPtr(aRequestPtr)
   , mQueuePtr(aQueuePtr)
   , mServerIndex(aServerIndex)
   , mPlatformIndex(aQueuePtr->GetPlatform()->GetIndex())
   , mEventEpoch(aQueuePtr->GetEventEpoch())
{
}

// =================================================================================================
// virtual
WsfTimeDelayQueue::CompleteRequestEvent::~CompleteRequestEvent()
{
   delete mRequestPtr;
}

// =================================================================================================
// virtual
WsfEvent::EventDisposition WsfTimeDelayQueue::CompleteRequestEvent::Execute()
{
   EventDisposition disposition = cDELETE;
   if ((GetSimulation()->GetPlatformByIndex(mPlatformIndex) != nullptr) && (mEventEpoch == mQueuePtr->GetEventEpoch()))
   {
      mRequestPtr = mQueuePtr->CompleteRequest(GetTime(), mRequestPtr, mServerIndex);
      if (mRequestPtr != nullptr)
      {
         double timeNeeded = mRequestPtr->TimeRequired();
         assert(timeNeeded >= 0.0);
         SetTime(GetTime() + timeNeeded);
         disposition = cRESCHEDULE;
      }
   }
   return disposition;
}
