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
//

#include "GenServerCCStats.hpp"

#include <iomanip>
#include <iostream>

#include "GenI.hpp"
#include "GenO.hpp"


GenServerCCStats::GenServerCCStats()
   : mClientName("Unknown")
   , mProcessTime(0.0F)
   , mNumRecvsSends(0)
   , mNumBytes(0)
{
}

GenServerCCStats::~GenServerCCStats() {}

int GenServerCCStats::GetLength() const
{
   // mProcessTime         4
   // mNumRecvsSends       4
   // mNumBytes            4
   // mClientName length   4

   return 16 + (int)mClientName.length() + GetPad1((int)mClientName.length());
}

void GenServerCCStats::Get(GenI& aGenI)
{
   aGenI >> mProcessTime;
   aGenI >> mNumRecvsSends;
   aGenI >> mNumBytes;
   aGenI.GetString(mClientName);

   // When we recv a string an extra 4 bytes were sent to describe the
   // length
   for (int j = GetPad1((int)mClientName.length()); j > 0; --j)
   {
      char pad1;
      aGenI >> pad1;
   }
}

void GenServerCCStats::Put(GenO& aGenO) const
{
   aGenO << mProcessTime;
   aGenO << mNumRecvsSends;
   aGenO << mNumBytes;
   aGenO.PutString(mClientName);

   // When we send a string an extra 4 bytes are sent to describe the
   // length
   for (int j = GetPad1((int)mClientName.length()); j > 0; --j)
   {
      char pad1 = 0;
      aGenO << pad1;
   }
}

void GenServerCCStats::Print(std::ostream& aStream, int aFormat)
{
   switch (aFormat)
   {
   case 1:
      aStream << " ClientName      = " << mClientName << std::endl;
      aStream << " RecvProcessTime = " << mProcessTime << std::endl;
      aStream << " NumRecvsSends   = " << mNumRecvsSends << std::endl;
      aStream << " NumBytes        = " << mNumBytes << std::endl;
      break;

   case 2:
      aStream << std::endl;
      aStream << std::setw(15) << std::left << "Client Name";
      aStream << std::setw(15) << std::left << "ProcessTime";
      aStream << std::setw(15) << std::left << "NumRecvsSends";
      aStream << std::setw(15) << std::left << "NumBytes" << std::endl;

      aStream << std::setw(15) << std::left << mClientName;
      aStream << std::setw(15) << std::left << mProcessTime;
      aStream << std::setw(15) << std::left << mNumRecvsSends;
      aStream << std::setw(15) << std::left << mNumBytes << std::endl;
      break;

   default:
      aStream << std::endl;
      aStream << std::setw(15) << std::left << mClientName;
      aStream << std::setw(15) << std::left << mProcessTime;
      aStream << std::setw(15) << std::left << mNumRecvsSends;
      aStream << std::setw(15) << std::left << mNumBytes << std::endl;
      break;
   }
}

//
// Private
//

inline int GenServerCCStats::GetPad1(int lengthInBytesA) const
{
   int tmp = (lengthInBytesA + 7) / 8;
   return tmp * 8 - lengthInBytesA;
}
