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

#ifndef WSFTIMEDELAYQUEUE_HPP
#define WSFTIMEDELAYQUEUE_HPP

#include "wsf_export.h"

#include <list>
#include <queue>
#include <stack>
#include <vector>

class UtInput;
#include "WsfEvent.hpp"
class WsfPlatform;
class WsfPlatformPart;
class WsfScriptContext;
#include "WsfVariable.hpp"

//! A class that implements a queue where the work performed by the servers
//! is represented by a time delay.
//!
//! The caller submits an object of derived from Request which represents the work to be performed.
//! The request includes a specification of the amount of time needed to complete the work and
//! the definition of a method that is called when the request has been completed.
//! If a server is available then it will be assigned to that server, otherwise it will be
//! put on a pending queue for the next available server.
//!
//! To perform the 'work', the server will schedule an event for the completion time of the
//! work (time work started + time needed for the request). When the event executes, it invokes
//! Request::CompleteRequest to allow the submitter to detect that the time delay has expired
//! and to perform any actions associated with the completion. The server is then release and
//! will then be free to select the next highest priority pending request.
//!
//! This is typically used to implement the concept of 'thinking time' or 'processing time'.
//!
//! At the current time only a single first-in-first-out server is allocated.
class WSF_EXPORT WsfTimeDelayQueue
{
public:
   //! Queuing method
   enum QueuingMethod
   {
      cFIRST_IN_FIRST_OUT, //!< First-in, first-out queuing
      cLAST_IN_FIRST_OUT,  //!< Last-in, first-out queuing
      cPRIORITY,           //!< Priority queuing
      cNONE                //!< No queuing
   };

   //! An abstract base class that represents a request to be performed.
   //! The caller must derive from this class and provide an implementation for CompleteRequest()
   //! to perform the actual work.
   class Request
   {
   public:
      //! Construct a request.
      //! @param aTimeRequired The time required to complete the request.
      Request(double aTimeRequired)
         : mTimeRequired(aTimeRequired)
      {
      }
      virtual ~Request() = default;

      //! Perform the actual processing associated with the request.
      //! @param aSimTime The current simulation time.
      //! @returns If true then the request is complete. If false then the request is not complete and
      //! SetTimeRequired should have been called to set the ADDITIONAL time needed to complete the request.
      virtual bool CompleteRequest(double aSimTime) = 0;

      //! Get the amount of time needed to complete the request.
      double TimeRequired() const { return mTimeRequired; }

      //! Set the amount of time needed to complete the request.
      void SetTimeRequired(double aTimeRequired) { mTimeRequired = aTimeRequired; }

   private:
      //! The amount of time required to complete the request.
      double mTimeRequired{0.0};
   };

   WsfTimeDelayQueue() = default;
   WsfTimeDelayQueue(const WsfTimeDelayQueue& aSrc);
   WsfTimeDelayQueue& operator=(const WsfTimeDelayQueue&) = delete;
   virtual ~WsfTimeDelayQueue();

   //! @name Framework Methods
   //@{
   virtual WsfTimeDelayQueue* Clone() const;

   virtual bool Initialize(double            aSimTime,
                           WsfPlatform*      aPlatformPtr,
                           WsfPlatformPart*  aPlatformPartPtr,
                           WsfScriptContext* aContextPtr);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void TurnOff(double aSimTime);

   virtual void TurnOn(double aSimTime);
   //@}

   void SetNumberOfServers(int aNumberOfServers);

   virtual void SubmitRequest(double aSimTime, Request* aRequestPtr);

   //! @name Internal methods.
   //! The following are used for event processing.
   //@{
   virtual Request* CompleteRequest(double aSimTime, Request* aRequestPtr, size_t aServerIndex);

   //! Return the current event scheduling epoch.
   unsigned int GetEventEpoch() const { return mEventEpoch; }

   //! Return the platform associated with the request queue.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }
   //@}

   //! A queue of pending requests that have not been assigned to a server
   class RequestQueue
   {
   public:
      RequestQueue()          = default;
      virtual ~RequestQueue() = default;

      virtual void     Purge()                    = 0;
      virtual void     Push(Request* aRequestPtr) = 0;
      virtual Request* Pop()                      = 0;
   };

   //! Adapter class for a First-In, First-Out (FIFO) queue.
   class FIFO_Queue : public WsfTimeDelayQueue::RequestQueue
   {
   public:
      void     Purge() override;
      void     Push(Request* aRequestPtr) override;
      Request* Pop() override;

   private:
      std::deque<Request*> mQueue;
   };

   //! Adapter class for a Last-In, First-Out (LIFO) queue.
   class LIFO_Queue : public WsfTimeDelayQueue::RequestQueue
   {
   public:
      void     Purge() override;
      void     Push(Request* aRequestPtr) override;
      Request* Pop() override;

   private:
      std::deque<Request*> mQueue;
   };

   //! Adapter class for a priority queue.
   class PriorityQueue : public WsfTimeDelayQueue::RequestQueue
   {
   public:
      void     Purge() override;
      void     Push(Request* aRequestPtr) override;
      Request* Pop() override;

   private:
      std::priority_queue<Request*> mQueue;
   };

   //! An event scheduled to complete the request at the end of its delay time.
   class CompleteRequestEvent : public WsfEvent
   {
   public:
      CompleteRequestEvent() = default;
      CompleteRequestEvent(double aSimTime, Request* aRequestPtr, WsfTimeDelayQueue* aQueuePtr, size_t aServerIndex);

      ~CompleteRequestEvent() override;

      EventDisposition Execute() override;

   private:
      Request*           mRequestPtr{nullptr};
      WsfTimeDelayQueue* mQueuePtr{nullptr};
      size_t             mServerIndex{0};
      size_t             mPlatformIndex{0};
      unsigned int       mEventEpoch{0};
   };

   WsfSimulation* GetSimulation() const;

protected:
   void CleanUp();

   //! Pointer to the platform that owns the queue.
   WsfPlatform* mPlatformPtr{nullptr};

   //! The number of servers
   WsfVariable<int> mNumberOfServers{1};

   //! The current request being processed by each server.
   //! If an entry is zero then the server is not busy.
   std::vector<Request*> mServers;

   //! The method of queuing requests when a server is not available.
   QueuingMethod mQueuingMethod{cFIRST_IN_FIRST_OUT};

   //! Pointer to the adapter that implements the queuing methodology.
   RequestQueue* mPendingQueuePtr{nullptr};

   //! The current event scheduling epoch.
   unsigned int mEventEpoch{1};
};

#endif
