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

// GenLoggerSharedO - Used to put data to the logger via the shared interface.


#ifndef GEN_LOGGER_O_HPP
#define GEN_LOGGER_O_HPP

#include "genio_export.h"

#include "GenO.hpp"

class GenBufOManaged;
class ClientToLoggerInterface;

class GENIO_EXPORT GenLoggerO : public GenO
{
public:
   // dataSetIdA is a unique number for this data set
   // subDataSetIdA can be used to sub divide data sets
   // dataTypeA = 1 means normal data
   // dataTypeA = 2 means data will not be be skipped during playback
   //               when the time makes discrete jumps
   GenLoggerO(int aDataSetId, int aSubDataSetId = 0, int aDataType = 1);

   ~GenLoggerO() override;

   // Clears Send Buffer.
   // Returns number of bytes sent otherwise returns a negative number
   // if there was an error
   int Send() override;

   static void SetActive(int aLoggerActive);
   static int  GetActive();

private:
   struct DataSetHeader
   {
      DataSetHeader(int aDataSetId, int aDataType, int aSubDataSetId);
      ~DataSetHeader();
      static int GetLength();
      void       Put(GenO& mGenO, long& aLengthPos);

      int   mSet_ID;
      int   mData_type;
      float mMSIP_runtime;
      float mTime_tag;
      int   mBody_num;
      int   mData_size;
   };

   static unsigned long     sMaxBufferSize;
   static int               sLoggerPort;
   static int               sLoggerActive;
   GenBufOManaged*          mSendBuf;
   long                     mLengthPos;
   ClientToLoggerInterface* mClientSide;
};


// Static
inline void GenLoggerO::SetActive(int aLoggerActive)
{
   sLoggerActive = aLoggerActive;
}

// Static
inline int GenLoggerO::GetActive()
{
   return sLoggerActive;
}

// Static
inline int GenLoggerO::DataSetHeader::GetLength()
{
   return 24;
}

#endif
