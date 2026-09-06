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

#include "dis/WsfDisIO_Thread.hpp"

#include "Dis.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisDevice.hpp"
#include "dis/WsfDisInterface.hpp"
#include "dis/WsfDisPlatform.hpp"
#include "dis/WsfDisUtil.hpp"
#include "ext/WsfExtInterface.hpp"

// ============================================================================
WsfDisIO_Thread::WsfDisIO_Thread(WsfDisInterface* aInterfacePtr)
   : WsfThread()
   , mIncomingMutex()
   , mOutboundMutex()
   , mInterfacePtr(aInterfacePtr)
   , mThreadState(RCV_INCOMING)
   , mSendOutboundPDUs(false)
   , mReceivePDUs(false)
   , mIncomingPDUs()
   , mOutboundPDUs()
   , mWaitForStartPDU(false)
   , mSimTime(0.0)
   , mLastUpdate(0.0)
   , mTimedOut(false)
   , mIsRealTime(false)
   , mWallClock()
{
}

// ============================================================================
// virtual
WsfDisIO_Thread::~WsfDisIO_Thread()
{
   for (std::list<DisPdu*>::iterator InPduIt = mIncomingPDUs.begin(); InPduIt != mIncomingPDUs.end(); ++InPduIt)
   {
      delete (*InPduIt);
   }
   mIncomingPDUs.clear(); //! Work queue of Incoming PDUs to process

   mOutboundPDUs.clear(); //! Work queue of Outgoing PDUs to process
}

// ============================================================================
//! Performs the thread work.
//!
//! @return Function state of the thread. (See WsfThread)
WsfDisIO_Thread::FunctionType WsfDisIO_Thread::DoWork()
{
   switch (mThreadState)
   {
   case IDLE:
      break;
   case SEND_OUTBOUND:
      if (mSendOutboundPDUs)
      {
         SendOutboundPDUs();
      }
      break;
   case RCV_INCOMING:
      if (mReceivePDUs || mWaitForStartPDU)
      {
         ReceivePDUs();
      }
      break;
   case WORK_COMPLETE:
      break;
   default:
      break;
   }

   TransitionState();

   // If there is currently no work in the queue then make the thread available
   if (GetFunction() == ASSIGNED)
   {
      if (mThreadState == WORK_COMPLETE)
      {
         return AVAILABLE;
      }
   }

   return GetFunction();
}

// ============================================================================
// private
void WsfDisIO_Thread::TransitionState()
{
   switch (mThreadState)
   {
   case IDLE:
      break;
   case SEND_OUTBOUND:
      // Check for transition to processing incoming PDUs or to work complete
      if (mOutboundPDUs.empty() || (!mSendOutboundPDUs))
      {
         if (mReceivePDUs)
         {
            mThreadState = RCV_INCOMING;
         }
         else
         {
            mThreadState = WORK_COMPLETE;
         }
      }
      else if (mTimedOut)
      {
         if (mReceivePDUs)
         {
            mThreadState = RCV_INCOMING;
         }
         mTimedOut = false;
      };
      break;
   case RCV_INCOMING:
      // Check for transition to process outbound PDUs or to work complete
      if (!mWaitForStartPDU)
      {
         if (mIncomingPDUs.empty() || (!mReceivePDUs))
         {
            if (mSendOutboundPDUs && (!mOutboundPDUs.empty()))
            {
               mThreadState = SEND_OUTBOUND;
            }
            else
            {
               mThreadState = WORK_COMPLETE;
            }
         }
         else if (mTimedOut)
         {
            if (mSendOutboundPDUs && (!mOutboundPDUs.empty()))
            {
               mThreadState = SEND_OUTBOUND;
            }
            mTimedOut = false;
         }
      }
      break;
   case WORK_COMPLETE:
      if (mReceivePDUs && ((mSimTime - mLastUpdate) >= 0.1))
      {
         mThreadState = RCV_INCOMING;
         if (ReadyForWork())
         {
            AssignWork();
         }
      }
      break;
   default:
      break;
   }
}

// ============================================================================
//! Immediately send a PDU out over the current connections.
//! @param aSimTime The current simulation time.
//! @param aPdu The PDU to be sent.
void WsfDisIO_Thread::SendPduOnDevices(double aSimTime, DisPdu& aPdu)
{
   unsigned int deviceCount = mInterfacePtr->GetDeviceCount();
   for (unsigned int i = 0; i < deviceCount; ++i)
   {
      mInterfacePtr->GetDevice(i)->PutPdu(aSimTime, aPdu);
   }
}

// ============================================================================
//! Called from the main thread to add a PDU to the transmit queue.
void WsfDisIO_Thread::ImmediatePutPdu(double aSimTime, DisPdu* aPduPtr)
{
   mOutboundMutex.lock();

   // Add the data to the work queue
   PDU tempPDU;
   tempPDU.mSimTime = aSimTime;
   tempPDU.mPduPtr.reset(aPduPtr->Clone());
   mOutboundPDUs.push_back(std::move(tempPDU));

   mOutboundMutex.unlock();

   if (mThreadState == WORK_COMPLETE)
   {
      mThreadState = SEND_OUTBOUND;
   }

   if (ReadyForWork())
   {
      AssignWork(); // Wake up the thread if asleep...
   }
}

// ============================================================================
//! Called from the main thread to add a PDU to the transmit queue.
void WsfDisIO_Thread::PutPdu(double aSimTime, std::unique_ptr<DisPdu> aPduPtr)
{
   mOutboundMutex.lock();

   // Add the data to the work queue
   PDU tempPDU;
   tempPDU.mSimTime = aSimTime;
   tempPDU.mPduPtr  = std::move(aPduPtr);
   mOutboundPDUs.push_back(std::move(tempPDU));

   mOutboundMutex.unlock();

   if (mThreadState == WORK_COMPLETE)
   {
      mThreadState = SEND_OUTBOUND;
   }

   if (ReadyForWork())
   {
      AssignWork(); // Wake up the thread if asleep...
   }
}

// ============================================================================
//! Called from the main thread to get the next PDU from the receive queue.
DisPdu* WsfDisIO_Thread::GetPdu()
{
   DisPdu* pduPtr = nullptr;

   std::lock_guard<std::recursive_mutex> lock(mIncomingMutex);

   if (!mIncomingPDUs.empty())
   {
      std::list<DisPdu*>::iterator iterBegin = mIncomingPDUs.begin();
      pduPtr                                 = *iterBegin;
      mIncomingPDUs.pop_front();
   }
   return pduPtr;
}

// ============================================================================
//! Called from the main thread AdvanceTime method.
void WsfDisIO_Thread::AdvanceTime(double aSimTime)
{
   // AdvanceTime may be called even if frame is not advancing.
   // If the thread is available then the device is checked for incoming PDUs.
   if (ReadyForWork())
   {
      mSimTime = aSimTime;
      AssignWork();
   }
}

// ============================================================================
//! Called from the main thread to initialize the thread.
void WsfDisIO_Thread::Initialize(bool aAutostart)
{
   // Wait for the Start PDU only if not auto-starting.
   mWaitForStartPDU = !aAutostart;

   mIsRealTime = WsfDisUtil::GetSimulation(mInterfacePtr).IsRealTime();
   ;

   mSendOutboundPDUs = mInterfacePtr->HasOutputDevice();
   mReceivePDUs      = mInterfacePtr->HasInputDevice();

   // Idle until simulation starts
   mThreadState = IDLE;

   // Create the actual thread
   Start();
}

// ============================================================================
//! Called from the main thread to indicate that the simulation is complete.
void WsfDisIO_Thread::SimulationComplete()
{
   // Wait until the thread is stopped.
   if (GetFunction() != WsfThread::STOPPED)
   {
      WaitUntilWorkDone();
      StopWork();
      EndThread();
      Join();
   }
}

// ============================================================================
//! Called from the main thread to indicate that the simulation is complete.
void WsfDisIO_Thread::SimulationStarting()
{
   mThreadState = RCV_INCOMING;
}

// ============================================================================
// private
//
// Note: The immediate processing of entity state PDUs is currently not available due to
//       the main thread wanting to work with the platform before the platform is fully
//       initialized. An AddPlatform event could be added, but this still has the main thread
//       doing the add which isn't saving any more time than having it processed by the main thread
//       during normal PDU updates.
void WsfDisIO_Thread::ReceivePDUs()
{
   mTimedOut   = false;
   mLastUpdate = mSimTime;

   // Get the next PDU from the device and process it.
   if (mInterfacePtr->HasInputDevice())
   {
      int numIncomingPDUs(0);
      // int numEntityStatePDUs(0);
      do
      {
         mWallClock.ResetClock();
         DisPdu* pduPtr = mInterfacePtr->GetPdu();
         while (pduPtr != nullptr)
         {
            if (pduPtr->GetExerciseIdentifier() == Dis::GetExercise())
            {
               // A non-zero return code from Process() indicates that the PDU has been processed
               // and is to be deleted.  A zero return code indicates that the PDU should be retained.
               if (/*(pduPtr->GetPduType() != DisEnum::Pdu::Type::EntityState) &&*/
                   (pduPtr->GetPduType() != DisEnum::Pdu::Type::StartResume))
               {
                  // queue up everything except entity states for processing later
                  mIncomingMutex.lock();
                  mIncomingPDUs.push_back(pduPtr);
                  mIncomingMutex.unlock();
                  ++numIncomingPDUs;
               }
               else
               {
                  if (mWaitForStartPDU)
                  {
                     if (pduPtr->GetPduType() == DisEnum::Pdu::Type::StartResume)
                     {
                        mWaitForStartPDU = false;
                        ++numIncomingPDUs;
                     }
                  }

                  // if (pduPtr->Process() != 0)
                  //{
                  //    delete pduPtr;
                  //    ++numEntityStatePDUs;
                  // }
               }
            }
            else
            {
               // PDU is not for this exercise
               delete pduPtr;
            }

            if (GetFunction() == PAUSED)
            {
               // Break out of the loop if the main thread needs to halt processing
               mThreadState = IDLE;
               break;
            }

            pduPtr = mInterfacePtr->GetPdu();

            if (mIsRealTime && (mWallClock.GetClock() > 0.2))
            {
               ut::log::info() << "DisIO_Thread: ReceivePDUs: Exceeded 0.2 seconds with incoming PDUs in queue.";
               mTimedOut = true;
               break;
            }
         }

         if (mInterfacePtr->mDebugThread)
         {
            auto out = ut::log::debug() << "Received PDUs.";
            out.AddNote() << "T = " << mSimTime;
            out.AddNote() << "Count: " << numIncomingPDUs;
         }
      } while (mWaitForStartPDU);
   }
}

// ============================================================================
// private
void WsfDisIO_Thread::SendOutboundPDUs()
{
   mWallClock.ResetClock();
   mTimedOut = false;

   // Get the current size of the queue; this is an instantaneous value since PDUs are constantly being added
   size_t numPDUs(mOutboundPDUs.size());

   // Send the PDU on to the device; this could be time consuming
   std::lock_guard<std::recursive_mutex> lock(mOutboundMutex);

   std::list<PDU>::iterator iter     = mOutboundPDUs.begin();
   size_t                   pduIndex = 1;
   for (; pduIndex <= numPDUs; ++pduIndex)
   {
      if (mInterfacePtr->HasOutputDevice())
      {
         SendPduOnDevices((*iter).mSimTime, *((*iter).mPduPtr));
      }
      ++iter;

      if (GetFunction() == PAUSED)
      {
         // Break out of the loop if the main thread is starting the
         // frame and needs to halt processing
         mThreadState = IDLE;
         break;
      }

      if (mIsRealTime && (mWallClock.GetClock() > 0.2))
      {
         ut::log::info() << "DisIO_Thread: SendPDUs: Exceeded 0.2 seconds with outbound PDUs in queue.";
         mTimedOut = true;
         break;
      }
   }

   // If some PDUs were processed then remove them from the work queue
   if (iter != mOutboundPDUs.begin())
   {
      // Remove the processed PDUs; use the pduIndex not numPDUs because of the break statement
      std::list<PDU>::iterator iterBegin;
      for (unsigned int ctr = 1; ctr < pduIndex; ++ctr)
      {
         iterBegin = mOutboundPDUs.begin();
         mOutboundPDUs.pop_front();
      }
   }

   if (mInterfacePtr->mDebugThread)
   {
      auto out = ut::log::debug() << "Sending PDUs";
      out.AddNote() << "T = " << mSimTime;
      out.AddNote() << "Count: " << pduIndex - 1 << " / " << numPDUs;
   }
}
