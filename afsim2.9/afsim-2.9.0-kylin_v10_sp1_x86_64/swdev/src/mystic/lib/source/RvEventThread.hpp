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

#ifndef RVWSFEVENTTHREAD_HPP
#define RVWSFEVENTTHREAD_HPP

#include <mutex>

#include <QMutex>
#include <QThread>

#include "RvWsfPipe_FileStreamer.hpp"

namespace RvWsfPipe
{
class MessageCache;
}

namespace rv
{
class ResultDb;

class EventThread : public QThread
{
public:
   EventThread(ResultDb* aDb, unsigned int aPageLimit);
   ~EventThread() override;
   // other thread access

   void SetEventSource(const std::string& aFileName);

   void ResetSource();
   void QuitThread();
   void SetCurrentTime(float aSimTime);
   // void LockMessages();
   // void UnlockMessages();

   RvWsfPipe::MessageCache* LockCache()
   {
      LockThread();
      return mCache;
   }
   void                   UnlockCache() { UnlockThread(); }
   RvWsfPipe::StreamState GetStreamState();

private:
   void LockThread() { mPublicMutex.lock(); }
   void UnlockThread() { mPublicMutex.unlock(); }

   // void SliderChange(int aValue);
   void ResetSourceP();
   void run() override;

   ResultDb*                mDb;
   volatile bool            mRunning;
   std::recursive_mutex     mPublicMutex;
   RvWsfPipe::MessageCache* mCache;
   RvWsfPipe::FileReader*   mReader;
   RvWsfPipe::FileStreamer* mFileStreamer;
   RvWsfPipe::StreamState   mStreamState{RvWsfPipe::cOK};
   unsigned int             mPageLimit;
};
} // namespace rv
#endif
