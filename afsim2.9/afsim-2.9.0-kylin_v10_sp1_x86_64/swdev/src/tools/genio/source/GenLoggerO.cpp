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

#include "GenLoggerO.hpp"

#include "GenBufOManaged.hpp"

unsigned long GenLoggerO::sMaxBufferSize = 1024 * 1024 - 24 // data_set_header
                                           - 72;            // data_set_reply_header
int GenLoggerO::sLoggerPort = 32768;
int GenLoggerO::sLoggerActive;


//
// This class inherits from GenO
//

GenLoggerO::GenLoggerO(int aDataSetId, int aSubDataSetId, int aDataType)
   : GenO("GenLoggerO")
{
   // Take the default and not let the buffer grow
   mSendBuf = new GenBufOManaged(GenBuf::BigEndian, sMaxBufferSize);
   SetGenBufXferO(mSendBuf);

   // Fill up a Data Set Header
   DataSetHeader dataSetHeader(aDataSetId, aSubDataSetId, aDataType);

   // Write the header to the buffer and get the position
   // of the length variable
   dataSetHeader.Put(*this, mLengthPos);
}


GenLoggerO::~GenLoggerO()
{
   delete mSendBuf;
}

int GenLoggerO::Send()
{
   int length = static_cast<int>(mSendBuf->GetPutOffset(GenBuf::FromBeg) - mLengthPos - 4);
   int result = 0;

   if (length > 0)
   {
      // Write the length
      SetPutOffset(mLengthPos, GenBuf::FromBeg);
      Put(length);

      // Position the put pointer to the end of the data so we get the correct
      // length when we send the data to the logger below.
      SetPutOffset(0, GenBuf::FromDataEnd);

      if (sLoggerActive)
      {
         // Alright send the data
         // result = SharedSendMsg(sLoggerPort,
         //                       mSendBuf->GetBuffer(),
         //                       static_cast<int>(mSendBuf->GetPutOffset(GenBuf::FromBeg)));

         // From the Put above, Our put pointer is in the right spot for the
         // next round of data so nothing else to do here.
      }

      // Set the number of valid bytes to the size of the DataSetHeader
      // which is 24
      mSendBuf->SetNumValidBytes(DataSetHeader::GetLength());

      ClearOutputErrorStatus();
   }

   if (result < 0)
   {
      return result;
   }
   else
   {
      return length;
   }
}

//
// Private
//

GenLoggerO::DataSetHeader::DataSetHeader(int aDataSetId, int aDataType, int aSubDataSetId)
   : mSet_ID(aDataSetId)
   , mData_type(aDataType)
   , mMSIP_runtime(0.0)
   , mTime_tag(0.0)
   , mBody_num(0)
   , mData_size(aSubDataSetId)
{
}

GenLoggerO::DataSetHeader::~DataSetHeader() {}

void GenLoggerO::DataSetHeader::Put(GenO& mGenO, long& aLengthPos)
{
   // Fill in header info
   mGenO << mSet_ID;
   mGenO << mData_type;

   mGenO << mMSIP_runtime;
   mGenO << mTime_tag;

   mGenO << mBody_num;

   // Get the position in the buffer where length is stored
   aLengthPos = mGenO.GetPutOffset(GenBuf::FromBeg);

   // Spot for length, but keep it empty for now
   mGenO << mData_size;
}
