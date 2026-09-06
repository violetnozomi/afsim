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

#ifndef WSFEVENTPIPEFILEWRITEWORKER_HPP
#define WSFEVENTPIPEFILEWRITEWORKER_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

#include "UtPack.hpp"
#include "UtThread.hpp"
#include "UtWallClock.hpp"

namespace wsf
{
namespace eventpipe
{
// this class manages a double-buffer for the writing of event-pipe messages to file
class FileWriterWorker : public UtThread
{
public:
   FileWriterWorker();
   void Push(std::unique_ptr<UtPackMessage> msg);

   void Run() override;
   void StartRunning();
   void StopRunning();

   UtPackMessageStdStreamO* mStreamPtr;

private:
   bool        mRunning;
   UtWallClock mClock;
   std::mutex  mFrontAccessMutex; // this is to prevent collisions of multiple sim-threads
   std::mutex  mGeneralMutex;     // this is to prevent collisions between sim-threads and the write-thread
   std::queue<std::unique_ptr<UtPackMessage>> mFrontMessageQueue;
   std::queue<std::unique_ptr<UtPackMessage>> mBackMessageQueue;
   std::condition_variable                    mBackMessagesAdded;
   std::condition_variable                    mBackMessagesPopped;
};
} // namespace eventpipe
} // namespace wsf

#endif
