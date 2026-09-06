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
#include "RvEventThread.hpp"

#include <iostream>

#include <QSlider>

#include "RvWsfPipe.hpp"
#include "UtException.hpp"
#include "UtSleep.hpp"


// This code will set the thread name that shows up in visual studio,
//   very helpful for debugging
#ifdef _WIN32

#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
   DWORD  dwType;     // Must be 0x1000.
   LPCSTR szName;     // Pointer to name (in user addr space).
   DWORD  dwThreadID; // Thread ID (-1=caller thread).
   DWORD  dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(const char* name)
{
   THREADNAME_INFO info;
   info.dwType     = 0x1000;
   info.szName     = name;
   info.dwThreadID = -1;
   info.dwFlags    = 0;

   __try
   {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
   }
   __except (EXCEPTION_EXECUTE_HANDLER)
   {
   }
}
#else
// no alternative
void SetThreadName(const char*)
{
   // nothing
}
#endif

// QSlider* g_Slider = 0;
rv::EventThread::EventThread(rv::ResultDb* aDb, unsigned int aPageLimit)
   : mPageLimit(aPageLimit)
{
   mDb           = aDb;
   mRunning      = true;
   mCache        = nullptr;
   mFileStreamer = nullptr;
   mReader       = nullptr;
   ResetSource();
}

rv::EventThread::~EventThread()
{
   ResetSourceP();
   delete mCache;
}

// void rv::EventThread::SliderChange(int aValue)
//{
//    mPublicMutex.Lock();
//
//    mCache->SetCurrentTime((float)aValue);
//    mPublicMutex.Unlock();
//
//    ViWsfResultPlatform* pp = mDb->FindPlatform(1);
//    if (pp)
//    {
//       pp->BeginRead();
//       const WsfEventPipe::MsgEntityState* state = pp->FindState((float)aValue);
//       if (state)
//       {
//          float simTime = state->simTime();
//          WsfEventPipe::Vec3d v = state->state().locationWCS();
//          pp->EndRead();
//          std::cerr << " T=" << aValue << " -> " << simTime << " :: "
//                    << v.x() << ", " << v.y() << ", " << v.z() << " \n";
//       }
//       else
//       {
//          pp->EndRead();
//       }
//    }
//
// }

void rv::EventThread::SetEventSource(const std::string& aFileName)
{
   std::lock_guard<std::recursive_mutex> lock(mPublicMutex);
   ResetSource();
   mFileStreamer = new RvWsfPipe::FileStreamer(aFileName, mCache);
   mReader       = new RvWsfPipe::FileReader(mFileStreamer);
   mCache->SetReader(mReader);
}

void rv::EventThread::ResetSource()
{
   std::lock_guard<std::recursive_mutex> lock(mPublicMutex);
   ResetSourceP();
}

void rv::EventThread::ResetSourceP()
{
   // stop the reader thread if it's running
   if (mReader)
   {
      mReader->Stop();
   }

   delete mFileStreamer;
   mFileStreamer = nullptr;

   delete mReader;
   mReader = nullptr;

   delete mCache;
   mCache = new RvWsfPipe::MessageCache(mDb, mPageLimit);
}


void rv::EventThread::run()
{
   ut::SetupThreadErrorHandling();

   RvWsfPipe::StreamState streamState = RvWsfPipe::cOK;
   SetThreadName("RvEventThread");

   // QThread::sleep(5);
   int counter = 0;
   while (true)
   {
      ++counter;
      if (counter > 100)
      {
         RvWsfPipe::dump_timer_data();
         counter = 0;
      }

      mPublicMutex.lock();
      bool didWork = false;
      if (mCache)
      {
         didWork      = mCache->Update(streamState);
         mStreamState = streamState;
      }
      mPublicMutex.unlock();

      if (!mRunning)
         return;

      if (!didWork)
      {
         QThread::msleep(1);
      }
   }
   // deleteLater();
}

void rv::EventThread::QuitThread()
{
   mRunning = false;
}

void rv::EventThread::SetCurrentTime(float aSimTime)
{
   std::lock_guard<std::recursive_mutex> lock(mPublicMutex);
   mCache->SetCurrentTime(aSimTime);
}

RvWsfPipe::StreamState rv::EventThread::GetStreamState()
{
   RvWsfPipe::StreamState retval = RvWsfPipe::cOK;
   mPublicMutex.lock();
   retval = mStreamState;
   mPublicMutex.unlock();

   return retval;
}


//
// void rv::EventThread::LockMessages()
//{
//   mCache->IncrementMessageReaders();
//}
//
// void rv::EventThread::UnlockMessages()
//{
//   mCache->DecrementMessageReaders();
//}
