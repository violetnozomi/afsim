// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVCHANNEL_HPP
#define RVCHANNEL_HPP
#include <iostream>

#include <QMutex>
#include <QWaitCondition>

#include "UtCircularBuffer.hpp"

namespace rv
{
template<typename T>
class Channel
{
public:
   Channel(int aMaxSize)
      : mMaxSize(aMaxSize)
   {
   }

   bool push(const T& aValue, unsigned long aMaxWaitTime = ULONG_MAX)
   {
      mMutex.lock();
      while ((int)mBuffer.size() == mMaxSize)
      {
         if (!mBufferNotFull.wait(&mMutex, aMaxWaitTime))
         {
            mMutex.unlock();
            return false;
         }
      }
      mBuffer.push_back(aValue);
      mMutex.unlock();
      return true;
   }

   bool try_pop(T& aValue)
   {
      mMutex.lock();
      if (mBuffer.empty())
      {
         mMutex.unlock();
         return false;
      }
      aValue = mBuffer.front();
      mBuffer.pop_front();
      if ((int)mBuffer.size() == mMaxSize - 1)
      {
         mBufferNotFull.wakeAll();
      }
      mMutex.unlock();
      return true;
   }

private:
   QWaitCondition      mBufferNotFull;
   QMutex              mMutex;
   int                 mMaxSize;
   UtCircularBuffer<T> mBuffer;
};
} // namespace rv

#endif
