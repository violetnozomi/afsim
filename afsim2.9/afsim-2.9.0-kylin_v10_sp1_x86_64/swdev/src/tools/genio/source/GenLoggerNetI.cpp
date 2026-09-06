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

#include "GenLoggerNetI.hpp"

#include "GenBufIManaged.hpp"
#include "GenUmpIO.hpp"
#include "UtLog.hpp"
#include "ump.h"

#ifndef _WIN32
#include <unistd.h>
#endif

#include <iostream>

// Temporary prototype for now


long GenLoggerNetI::sMaxBufferSize = 32768;

//
// This class inherits from GenI
//


GenLoggerNetI::GenLoggerNetI()
   : GenI("GenLoggerNetI")
   , mRecvBuf(0)
   , mUmp(0)
   , mSocketRecv(-99999)
{
}

GenLoggerNetI::GenLoggerNetI(int                     aDataSetId,
                             const char*             aLoggerRequesterName,
                             const std::vector<int>& aSubDataSetId,
                             const char*             aCallingClassName,
                             int                     aLoggerRequestPort)
   : GenI("GenLoggerNetI")
   , mRecvBuf(0)
   , mUmp(0)
   , mSocketRecv(-99999)
{
   Init(aDataSetId, aLoggerRequesterName, aSubDataSetId, aCallingClassName, aLoggerRequestPort);
}


GenLoggerNetI::~GenLoggerNetI()
{
   delete mRecvBuf;
   delete mUmp;
}

// Returns a positive number is successful
int GenLoggerNetI::Init(int                     aDataSetId,
                        const char*             aLoggerRequesterName,
                        const std::vector<int>& aSubDataSetId,
                        const char*             aCallingClassName,
                        int                     aLoggerRequestPort)
{
   // Store calling class name if it is not a zero pointer
   mCallingClassName = aCallingClassName != 0 ? aCallingClassName : "";

   if (mUmp != 0)
   {
      mUmp = new GenUmpIO(0,                    // Socket File
                          aLoggerRequesterName, // Network Id
                          aLoggerRequestPort,   // Send to port
                          0,                    // Receive on port
                          GenUmpIO::TCP_CLIENT, ////////  Thuy ???????
                          "GenLoggerNetI");     // my name
   }

   // Create the receive socket for the real data
   mSocketRecv = umpCreateSocketC(0, // Requester name
                                  0, // Send port
                                  0, // Receive port
                                  UDP_RCV_ONLY,
                                  NO_CONVERT,
                                  QUEUED | UMP_ALLOW_MULTIPLE_RECEIVERS);

   if (mSocketRecv < 0)
   {
      auto out = ut::log::error() << "Could not open Receive UMP socket in GenLoggerNetO::Init().";
      out.AddNote() << "Received Error Code: " << mSocketRecv;
      return -402; // Need to return a better error code
   }

   // Create buffer
   mRecvBuf = new GenBufIManaged(GenBuf::BigEndian, sMaxBufferSize);
   SetGenBufXferI(mRecvBuf);

   // Get the Recv data port
   UMPSockInfoType socketRecvInfo;
   umpGetSocketInfoC(mSocketRecv, &socketRecvInfo, 0);
   int dataRecvPort = static_cast<int>(socketRecvInfo.localPort);

   // Build data request Class
   mDataRequest.Init(aDataSetId,               // Data set id
                     dataRecvPort,             // The real recv port
                     mUmp->GetLocalRecvPort(), // echo return port
                     aSubDataSetId);

   // Make the request
   mDataRequest.Put(*mUmp);
   mUmp->Send();

   return mSocketRecv;
}


int GenLoggerNetI::Receive(int aWaitTimeInMicroSec)
{
   // Return here if socket is not open
   if (mSocketRecv < 0)
   {
      auto out = ut::log::error() << "GenLoggerNetI::Receive called without UMP being correctly initialized.";
      out.AddNote() << "Calling Class: " << mCallingClassName;
      return -1;
   }

   // First look for Data Request Replys
   long int bytesLeft = mUmp->GetGetOffset(GenBuf::FromDataEnd);

   if (!bytesLeft)
   {
      bytesLeft = ReceiveBytes();
   }

   while (bytesLeft)
   {
      // Read the first integer
      int dataType;
      *mUmp >> dataType;

      if (dataType == 0)
      {
         // Got an End of Data Packet
         DataEnd* ptr = new DataEnd(*mUmp, dataType);
         ProcessEndOfData(ptr);
         delete ptr;
      }
      else if (dataType >= 10 && dataType <= 16)
      {
         // Got a Runtime Data Packet
         DataRun* ptr = new DataRun(*mUmp, dataType);
         ProcessRunData(ptr);
         delete ptr;
      }
      else
      {
         // Got a Data Request Echo
         DataRequest* ptr = new DataRequest(*mUmp, dataType);
         ProcessEchoDataRequest(ptr);
         delete ptr;
      }

      bytesLeft = mUmp->GetGetOffset(GenBuf::FromDataEnd);
      if (!bytesLeft)
      {
         bytesLeft = ReceiveBytes();
      }
   }

   // Now deal with the real data
   ClearInputErrorStatus();

   unsigned long validBytes = mRecvBuf->GetGetOffset(GenBuf::FromDataEnd);
   if (validBytes)
   {
      unsigned long getOffset = mRecvBuf->GetGetOffset(GenBuf::FromBeg);

      auto out = ut::log::error() << "GenLoggerNetI::Receive called.";
      out.AddNote() << "Calling Class: " << mCallingClassName;
      out.AddNote() << "Remaining Valid Bytes: " << validBytes;
      out.AddNote() << "Initial Valid Bytes: " << validBytes + getOffset;
   }

   // Clear the input buffer
   mRecvBuf->Reset();

   int numBytes = umpRcvMsgC(mSocketRecv, mRecvBuf->GetBuffer(), sMaxBufferSize, aWaitTimeInMicroSec, 0);

   // Return immediately if an error occured
   if (numBytes < 0)
   {
      return numBytes;
   }
   else
   {
      mRecvBuf->SetNumValidBytes(numBytes);
      return numBytes;
   }
}


//
// Private
//

GenLoggerNetI::DataRequest::DataRequest()
   : mType(1)
   , mStatus(0)
   , mDataSetId(0)
   , mDataRecvPort(0)
   , mEchoRecvPort(0)
   , mHeader(0)
   , mTimeType(0)
   , mSubDataSetId(0)
{
///////  Thuy and Dana might need to look at this this for windows someday
#ifndef _WIN32
   if (gethostname(mReqName, 64))
   {
      // This is an error condition
      mReqName[0] = 0;
   }
#endif

   mFileName[0] = '\0';
}

GenLoggerNetI::DataRequest::DataRequest(GenI& aGenI, int aType)
{
   mType = aType;

   aGenI >> mStatus;
   aGenI >> mDataSetId;
   aGenI >> mDataRecvPort;
   aGenI >> mEchoRecvPort;
   aGenI >> mHeader;
   aGenI >> mTimeType;

   aGenI.GetArray(mReqName, 64);
   aGenI.GetArray(mFileName, 128);

   int numSubDataSetsL;
   aGenI >> numSubDataSetsL;
}

GenLoggerNetI::DataRequest::~DataRequest() {}

void GenLoggerNetI::DataRequest::Init(int              aDataSetId,    // Data set id
                                      int              aDataRecvPort, // The real recv port
                                      int              aEchoRecvPort, // echo return port
                                      std::vector<int> aSubDataSetId)
{
   mDataSetId    = aDataSetId;
   mDataRecvPort = aDataRecvPort;
   mEchoRecvPort = aEchoRecvPort;
   mSubDataSetId = aSubDataSetId;
}


void GenLoggerNetI::DataRequest::Put(GenO& aGenO) const
{
   aGenO << mType;
   aGenO << mStatus;
   aGenO << mDataSetId;
   aGenO << mDataRecvPort;
   aGenO << mEchoRecvPort;
   aGenO << mHeader;
   aGenO << mTimeType;

   aGenO.PutArray(mReqName, 64);
   aGenO.PutArray(mFileName, 128);

   aGenO << static_cast<int>(mSubDataSetId.size());

   std::vector<int>::const_iterator iter(mSubDataSetId.begin());
   while (iter != mSubDataSetId.end())
   {
      aGenO << *iter;
      ++iter;
   }
}

GenLoggerNetI::DataRun::DataRun(GenI& aGenI, int aType)
{
   mType = aType;

   aGenI.GetArray(mReqName, 64);

   // I think this is needed
   int pad;
   aGenI >> pad;

   aGenI >> mTime;
}

GenLoggerNetI::DataRun::~DataRun() {}

GenLoggerNetI::DataEnd::DataEnd(GenI& aGenI, int aType)
{
   mType = aType;

   aGenI >> mDataSetId;

   aGenI.GetArray(mReqName, 64);

   aGenI >> mReason;
}

GenLoggerNetI::DataEnd::~DataEnd() {}

int GenLoggerNetI::ReceiveBytes()
{
   int status = mUmp->Receive(0);
   if (status < 0)
   {
      if (status < -1)
      {
         auto out = ut::log::error() << "UMP error in GenLoggerNetI::ReceiveBytes().";
         out.AddNote() << "Status: " << status;
      }
      status = 0;
   }
   return status;
}

void GenLoggerNetI::ProcessEndOfData(DataEnd* aPtr)
{
   auto out = ut::log::info() << "GenLoggerNetI::Receive() received flag.";
   out.AddNote() << "Calling Class: " << mCallingClassName;

   switch (aPtr->mReason)
   {
   case 0:
      out.AddNote() << "Flag: Request is complete";
      break;
   case 1:
      out.AddNote() << "Flag: MSIPBASE went to reset";
      break;
   case 4:
      out.AddNote() << "Flag: MSIPBASE went to stop or drop";
      break;
   case 7:
      out.AddNote() << "Flag: Logger terminate";
      break;
   default:
      out.AddNote() << "Flag: Unknown end of data";
      break;
   }
}

void GenLoggerNetI::ProcessRunData(DataRun* /* aPtr */)
{
   // Keep this routine as a place holder for now
}

void GenLoggerNetI::ProcessEchoDataRequest(DataRequest* aPtr)
{
   if (mDataRequest.mType != -aPtr->mType)
   {
      auto out = ut::log::info() << "Unrequested data set was received by genLoggerNetI::Receive().";
      out.AddNote() << "Calling Class: " << mCallingClassName;
      out.AddNote() << "Data Set: " << aPtr->mType;
   }
}
