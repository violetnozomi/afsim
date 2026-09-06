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

// GenLoggerNetI - Used to retreive data from the logger via ethernet.


#ifndef GEN_LOGGER_NET_I_HPP
#define GEN_LOGGER_NET_I_HPP

#include <string>

#include "GenExport.hpp"
#include "GenI.hpp"

class GenBufIManaged;
class GenUmpIO;
class GenO;

class GEN_EXPORT GenLoggerNetI : public GenI
{
public:
   GenLoggerNetI();

   // aDataSetId is a unique number for this data set
   // aLoggerRequest name if the name of the requester
   // aSubDataSetId can be used to sub divide data sets
   // aCallingClassName is for debugging purposes
   // aLoggerRequestPort is the port the logger is listening to for requests
   GenLoggerNetI(int                     aDataSetId,
                 const char*             aLoggerRequesterName,
                 const std::vector<int>& aSubDataSetId,
                 const char*             aCallingClassName  = 0,
                 int                     aLoggerRequestPort = 32767);


   virtual ~GenLoggerNetI();

   // Returns a negative number if failed
   // Arguments are the same as above
   int Init(int                     aDataSetId,
            const char*             aLoggerRequesterName,
            const std::vector<int>& aSubDataSetId,
            const char*             aCallingClassName  = 0,
            int                     aLoggerRequestPort = 32767);

   //  Return       Meaning
   //  >= 0         Data received.  This value could optionally be the
   //               number of bytes received.
   //  == -1        No data has been received.
   //  <  -1        An error has occured.
   virtual int Receive(int aWaitTimeInMicroSec);

private:
   struct DataRequest
   {
      DataRequest();
      DataRequest(GenI& aGenI, int aType);
      ~DataRequest();

      void Init(int              aDataSetId,     // Data set id
                int              aDataRecvPort,  // The real recv port
                int              aEchoRecvPort,  // echo return port
                std::vector<int> aSubDataSetId); // Sub Data set id

      void Put(GenO& aGenO) const;

      int              mType;
      int              mStatus;
      int              mDataSetId;
      int              mDataRecvPort;
      int              mEchoRecvPort;
      int              mHeader;
      int              mTimeType;
      char             mReqName[64];
      char             mFileName[128];
      std::vector<int> mSubDataSetId;
   };

   struct DataRun
   {
      DataRun(GenI& aGenI, int aType);
      ~DataRun();

      int    mType;
      char   mReqName[64];
      double mTime;
   };

   struct DataEnd
   {
      DataEnd(GenI& aGenI, int aType);
      ~DataEnd();

      int  mType;
      int  mDataSetId;
      char mReqName[64];
      int  mReason;
   };

   int  ReceiveBytes();
   void ProcessEndOfData(DataEnd* aPtr);
   void ProcessRunData(DataRun* aPtr);
   void ProcessEchoDataRequest(DataRequest* aPtr);

   static long sMaxBufferSize;

   std::string     mCallingClassName;
   DataRequest     mDataRequest;
   GenBufIManaged* mRecvBuf;
   GenUmpIO*       mUmp;
   int             mSocketRecv;
};

#endif
