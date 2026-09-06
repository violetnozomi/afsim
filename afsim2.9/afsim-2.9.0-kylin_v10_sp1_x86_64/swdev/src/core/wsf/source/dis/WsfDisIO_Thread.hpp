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

#ifndef WSFDISIO_THREAD_HPP
#define WSFDISIO_THREAD_HPP

#include "wsf_export.h"

#include <list>
#include <mutex>

class DisPdu;

class UtInput;
#include "UtWallClock.hpp"
class WsfDisInterface;
#include "WsfThread.hpp"

//! DIS interface implementation of worker thread
class WSF_EXPORT WsfDisIO_Thread : public WsfThread
{
public:
   enum ThreadState
   {
      IDLE          = 0,
      SEND_OUTBOUND = 1,
      RCV_INCOMING  = 2,
      WORK_COMPLETE = 3
   };

   //! Constructor and destructor
   WsfDisIO_Thread(WsfDisInterface* aInterfacePtr);
   ~WsfDisIO_Thread() override;

   // Implementation of abstract base class methods.

   FunctionType DoWork() override;
   FunctionType Pause() override { return AVAILABLE; }

   DisPdu* GetPdu();

   void ImmediatePutPdu(double aSimTime, DisPdu* aPduPtr);

   void PutPdu(double aSimTime, std::unique_ptr<DisPdu> aPduPtr);

   void SendPduOnDevices(double aSimTime, DisPdu& aPdu);

   void AdvanceTime(double aSimTime);
   void Initialize(bool aAutostart);

   void SimulationComplete();
   void SimulationStarting();

   void SetReceivePDUs(bool aReceivePDUs) { mReceivePDUs = aReceivePDUs; }
   void SetOutboundPDUs(bool aSendOutboundPDUs) { mSendOutboundPDUs = aSendOutboundPDUs; }

   void WaitForStartPDU(bool aWaitForStartPDU) { mWaitForStartPDU = aWaitForStartPDU; }

private:
   void TransitionState();
   void ReceivePDUs();
   void SendOutboundPDUs();

   struct PDU
   {
      double                  mSimTime;
      std::unique_ptr<DisPdu> mPduPtr;
   };

   // Mutex for handling thread access to the worker queue
   // WsfDisInterface adds PDUs to the queue
   // WsfDisIO_Thread reads and removes PDUs from the queue
   std::recursive_mutex mIncomingMutex;
   std::recursive_mutex mOutboundMutex;

   WsfDisInterface* mInterfacePtr; //! Pointer to the DIS interface

   ThreadState mThreadState;

   bool mSendOutboundPDUs;
   bool mReceivePDUs;

   std::list<DisPdu*> mIncomingPDUs; //! Work queue of Incoming PDUs to process
   std::list<PDU>     mOutboundPDUs; //! Work queue of Outbound PDUs to process

   bool mWaitForStartPDU; //! Used if not autostarting

   double mSimTime; //! Simulation time

   double mLastUpdate; //! Last time thread was ran/updated
   bool   mTimedOut;   //! Last receive/send timed out

   bool        mIsRealTime;
   UtWallClock mWallClock;
};

#endif
