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


#ifndef GEN_SERVER_CC_STATS_HPP
#define GEN_SERVER_CC_STATS_HPP

/*! \file    GenServerCCStats.h
 *  \brief   GenServerCCStats
 *  \author  T. Schnur
 *  \date    April 8, 2008
 *  \version 1.0
 *
 *  \class GenServerCCStats GenServerCCStats.h "GenServerCCStats.h"
 *  \brief GenServerCCStats Holds the client statistics for each connection to a GenUmpIOServer.
 *
 *.
 */

#include "genio_export.h"

#include <iosfwd>
#include <string>

class GenI;
class GenO;

class GENIO_EXPORT GenServerCCStats
{
public:
   GenServerCCStats();
   virtual ~GenServerCCStats();

   int  GetLength() const;
   void Get(GenI& aGenI);
   void Put(GenO& aGenO) const;
   void Print(std::ostream& aStream, int aFormat);

   const std::string& GetClientName() const;
   float              GetProcessTime() const;
   unsigned int       GetNumRecvsSends() const;
   unsigned int       GetNumBytes() const;

   void SetClientName(const std::string& aName);
   void SetProcessTime(float aProcessTime);
   void SetNumRecvsSends(unsigned int aNumRecvsSends);
   void SetNumBytes(unsigned int aNumBytes);

   bool operator==(const GenServerCCStats& aRhs) const;
   bool operator<(const GenServerCCStats& aRhs) const;
   bool operator>(const GenServerCCStats& aRhs) const;
   void operator+=(const GenServerCCStats& aRhs);

private:
   int GetPad1(int lengthInBytesA) const;


   std::string  mClientName;
   float        mProcessTime;
   unsigned int mNumRecvsSends;
   unsigned int mNumBytes;
};

inline const std::string& GenServerCCStats::GetClientName() const
{
   return mClientName;
}

inline float GenServerCCStats::GetProcessTime() const
{
   return mProcessTime;
}

inline unsigned int GenServerCCStats::GetNumRecvsSends() const
{
   return mNumRecvsSends;
}

inline unsigned int GenServerCCStats::GetNumBytes() const
{
   return mNumBytes;
}

inline void GenServerCCStats::SetClientName(const std::string& aClientName)
{
   mClientName = aClientName;
}

inline void GenServerCCStats::SetProcessTime(float aProcessTime)
{
   mProcessTime = aProcessTime;
}

inline void GenServerCCStats::SetNumRecvsSends(unsigned int aNumRecvsSends)
{
   mNumRecvsSends = aNumRecvsSends;
}

inline void GenServerCCStats::SetNumBytes(unsigned int aNumBytes)
{
   mNumBytes = aNumBytes;
}

inline bool GenServerCCStats::operator==(const GenServerCCStats& aRhs) const
{
   return mProcessTime == aRhs.mProcessTime;
}

inline bool GenServerCCStats::operator<(const GenServerCCStats& aRhs) const
{
   return mProcessTime < aRhs.mProcessTime;
}

inline bool GenServerCCStats::operator>(const GenServerCCStats& aRhs) const
{
   return mProcessTime > aRhs.mProcessTime;
}

inline void GenServerCCStats::operator+=(const GenServerCCStats& aRhs)
{
   mProcessTime += aRhs.mProcessTime;
   mNumRecvsSends += aRhs.mNumRecvsSends;
   mNumBytes += aRhs.mNumBytes;
}


#endif
