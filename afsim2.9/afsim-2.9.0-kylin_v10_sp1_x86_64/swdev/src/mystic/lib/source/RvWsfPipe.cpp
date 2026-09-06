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

#include "RvWsfPipe.hpp"

#include <iostream>

#include <QBasicTimer>

#include "RvEventPipeUtil.hpp"
#include "RvWsfPipe_FileStreamer.hpp"
#include "UtPackStream.hpp"
#include "UtWallClock.hpp"
#define DEBUG_STREAMING 0


//#define DEBUG_LOG(A,B,C) logfile << A << " " << B << " " << C << "\n"; logfile.flush();
#define DEBUG_LOG(A, B, C)

namespace
{
const int cMESSAGES_IN_MEMORY_LIMIT = 1 * 1000 * 1000;

struct EP_Timer
{
   void Reset(const double aDumpTime)
   {
      lastDumpTime                   = aDumpTime;
      Cache_AddMessageFromStream     = 0;
      MessageSource_ProcessTransient = 0;
      MessageSource_ProcessStream    = 0;
      MessageSource_UpdateOpen       = 0;
      ReadMessageContinue            = 0;
   }

   double lastDumpTime{1.0e30};
   double Cache_AddMessageFromStream{0.0};
   double MessageSource_ProcessTransient{0.0};
   double MessageSource_ProcessStream{0.0};
   double MessageSource_UpdateOpen{0.0};
   double ReadMessageContinue{0.0};
};
} // namespace

EP_Timer    sTimeCounts;
UtWallClock sTimerClock;
namespace RvWsfPipe
{
void dump_timer_data()
{
   return;
   double timeNow = sTimerClock.GetRawClock();
   double delta   = timeNow - sTimeCounts.lastDumpTime;

   if (delta > 1.0)
   {
      std::cerr << "TimeChange: " << delta << '\n';
      std::cerr << "Cache_AddMessageFromStream: " << sTimeCounts.Cache_AddMessageFromStream / delta << '\n';
      std::cerr << "MessageSource_ProcessTransient: " << sTimeCounts.MessageSource_ProcessTransient / delta << '\n';
      std::cerr << "MessageSource_ProcessStream: " << sTimeCounts.MessageSource_ProcessStream / delta << '\n';
      std::cerr << "MessageSource_UpdateOpen: " << sTimeCounts.MessageSource_UpdateOpen / delta << '\n';
      std::cerr << "ReadMessageContinue: " << sTimeCounts.ReadMessageContinue / delta << '\n';
      sTimeCounts.Reset(timeNow);
   }
   else if (delta < 0)
   {
      sTimeCounts.lastDumpTime = timeNow;
   }
}
} // namespace RvWsfPipe
struct TimeScope
{
   double* inc;
   double  startTime;
   TimeScope(double* aIncrementTime)
      : inc(aIncrementTime)
   {
      startTime = sTimerClock.GetRawClock();
   }
   ~TimeScope() { (*inc) += (sTimerClock.GetRawClock() - startTime); }
};

//#define SCOPE_TIMER(name) TimeScope _scopeTimer(&sTimeCounts.name)
#define SCOPE_TIMER(name)


namespace RvWsfPipe
{

MessageCache::MessageCache(rv::ResultDb* aDb, unsigned int aPageLimit)
   : mDb(aDb)
{
   mResetClock.ResetClock();
   mReader             = nullptr;
   mStreamingLastBlock = false;
   mCurrentStreamPage  = 0;

   mPagesInMemoryLimit = aPageLimit;

   SetCurrentTime(0);
}

MessageCache::~MessageCache()
{
   while (!mBlocks.empty())
   {
      Block                      b        = mBlocks.back();
      std::vector<rv::MsgBase*>& messages = *b.mMessages;
      for (int i = -1 + (int)messages.size(); i >= 0; --i)
      {
         rv::MsgBase* msg = messages[i];
         msg->Unref();
      }
      delete b.mMessages;
      mBlocks.pop_back();
   }
}

void MessageCache::SetCurrentTime(float aSimTime)
{
   mCurrentTime = aSimTime;
}

void MessageCache::PruneFromStart()
{
   assert(!mBlocks.empty());
   Block b = mBlocks.front();
#if DEBUG_STREAMING
   std::cerr << "Prune first block: " << b.mPageNumber << '\n';
   std::cerr << "  first: " << b.mMessages->front()->simTime() << '\n';
   std::cerr << "  last: " << b.mMessages->back()->simTime() << '\n';
#endif
   mBlocks.pop_front();
   {
      std::vector<rv::MsgBase*>& messages = *b.mMessages;
      for (size_t i = 0; i < messages.size(); ++i)
      {
         rv::MsgBase* msg = messages[i];
         mDb->PopFront(msg);
         msg->Unref();
      }
   }
   delete b.mMessages;
}

void MessageCache::PruneFromEnd()
{
   assert(!mBlocks.empty());
   Block b = mBlocks.back();
#if DEBUG_STREAMING
   std::cerr << "Prune last block: " << b.mPageNumber << '\n';
   std::cerr << "  first: " << b.mMessages->front()->simTime() << '\n';
   std::cerr << "  last: " << b.mMessages->back()->simTime() << '\n';
#endif
   mBlocks.pop_back();
   {
      std::vector<rv::MsgBase*>& messages = *b.mMessages;
      for (int i = -1 + (int)messages.size(); i >= 0; --i)
      {
         rv::MsgBase* msg = messages[i];
         mDb->PopBack(msg);
         msg->Unref();
      }
   }
   delete b.mMessages;
   mStreamingLastBlock = false;
}

void MessageCache::SetSchema(const UtPackSchema* aSchema)
{
   mDb->SetSchema(aSchema);
}

bool MessageCache::StartStreamingLastBlock()
{
   assert(!mStreamingLastBlock);

   if (!mBlocks.empty())
   {
      int page = mBlocks.back().mPageNumber;
      if (mCurrentStreamPage != page + 1)
      {
         return false;
      }
      mStreamingLastBlock = true;
      Block newBlock;
      newBlock.mMessages   = new std::vector<rv::MsgBase*>;
      newBlock.mPageNumber = mCurrentStreamPage;
#if DEBUG_STREAMING
      std::cerr << "Stream new block: " << newBlock.mPageNumber << '\n';
#endif
      mBlocks.push_back(newBlock);

      std::vector<rv::MsgBase*> messages;
      mReader->RegainStream(messages);

      for (size_t i = 0; i < messages.size(); ++i)
      {
         rv::MsgBase* msgPtr           = messages[i];
         bool         messageIsOneTime = rv::IsOneTimeMessage(msgPtr);
         if (!messageIsOneTime)
         {
            msgPtr->Ref();
            mDb->PushBack(msgPtr);
            newBlock.mMessages->push_back(msgPtr);
         }
      }
      TimeWindowChanged();
   }
   else
   {
      mStreamingLastBlock = true;

      Block newBlock;
      newBlock.mMessages   = new std::vector<rv::MsgBase*>;
      newBlock.mPageNumber = mCurrentStreamPage = 0;
#if DEBUG_STREAMING
      std::cerr << "Stream new block: " << 0 << '\n';
#endif
      mBlocks.push_back(newBlock);
   }
   assert(mStreamingLastBlock);
   return true;
}

bool MessageCache::Update(RvWsfPipe::StreamState& aStreamState)
{
   bool addBlockOnFront = false, addBlockOnBack = false;
   bool prune = false;
   if ((int)mBlocks.size() < mPagesInMemoryLimit)
   {
      if (!mBlocks.empty())
      {
         if (mCurrentTime - mBlocks.front().mMinTime < mBlocks.back().mMaxTime - mCurrentTime)
         {
            addBlockOnFront = mBlocks.front().mPageNumber != 0;
         }
         if (!addBlockOnFront)
         {
            addBlockOnBack = true;
         }
      }
      else
      {
         StartStreamingLastBlock();
      }
   }
   else
   {
      float dFront = mCurrentTime - mBlocks.front().mMinTime;
      float dBack  = mBlocks.back().mMaxTime - mCurrentTime;
      if (dFront < dBack * .3f)
      {
         if (mBlocks.front().mPageNumber != 0)
         {
            addBlockOnFront = true;
            prune           = true;
         }
      }
      else if (dBack < dFront * .3f)
      {
         int page = mBlocks.back().mPageNumber;
         if ((mCurrentStreamPage != page + 1) ||
             (mResetClock.GetClock() < 1.0)) // if we aren't at the end, or we haven't been waiting too long for an update
         {
            addBlockOnBack = true;
            prune          = true;
         }
      }
   }

   if (addBlockOnBack && !mStreamingLastBlock)
   {
      StartStreamingLastBlock(); // may fail if conditions aren't right
   }

   bool performedWork = false;
   if (addBlockOnBack &&
       !mStreamingLastBlock) // if we are at the end, mStreamingLastBlock never clears, so we get in a locked state
   {
      performedWork = true;
      int readPage  = 0;
      if (!mBlocks.empty())
         readPage = mBlocks.back().mPageNumber + 1;
      std::vector<rv::MsgBase*> messages;
      mReader->ReadPage(readPage, messages);

      Block b;
      b.mPageNumber = readPage;
      b.mMaxTime    = messages.back()->simTime();
      b.mMinTime    = messages.front()->simTime();
      b.mMessages   = new std::vector<rv::MsgBase*>;
#if DEBUG_STREAMING
      std::cerr << "Add block to back: " << b.mPageNumber << '\n';
#endif

      int msgCount = (int)messages.size();
      int lastMsg  = msgCount - 1;
      for (int i = 0; i < msgCount; ++i)
      {
         rv::MsgBase* msg = messages[i];
         if (i == lastMsg)
         {
            mDb->mMaxTimeInCache = messages[lastMsg]->simTime();
         }
         if (!rv::IsOneTimeMessage(msg))
         {
            mDb->PushBack(msg);
            b.mMessages->push_back(msg);
         }
         else
         {
            msg->Unref();
         }
      }
      mBlocks.push_back(b);

      if (prune)
      {
         PruneFromStart();
      }
      TimeWindowChanged();
   }

   if (addBlockOnFront)
   {
      performedWork = true;
      int readPage  = 0;
      if (!mBlocks.empty())
         readPage = mBlocks.front().mPageNumber - 1;

      std::vector<rv::MsgBase*> messages;
      mReader->ReadPage(readPage, messages);

      Block b;
      b.mPageNumber = readPage;
      b.mMaxTime    = messages.back()->simTime();
      b.mMinTime    = messages.front()->simTime();
      b.mMessages   = new std::vector<rv::MsgBase*>;
#if DEBUG_STREAMING
      std::cerr << "Add block to front: " << b.mPageNumber << '\n';
      std::cerr << "  first: " << messages.front()->simTime() << '\n';
      std::cerr << "  last: " << messages.back()->simTime() << '\n';
#endif
      for (int i = (int)messages.size() - 1; i >= 0; --i)
      {
         rv::MsgBase* msg = messages[i];
         if (i == 0)
         {
            b.mMinTime = messages[0]->simTime();
         }
         if (!rv::IsOneTimeMessage(msg))
         {
            mDb->PushFront(msg);
            b.mMessages->push_back(msg);
         }
         else
         {
            msg->Unref();
         }
      }
      std::reverse(b.mMessages->begin(), b.mMessages->end());
      mBlocks.push_front(b);

      if (prune)
      {
         PruneFromEnd();
      }
      TimeWindowChanged();
   }

   if (!performedWork) // this will handle messages beyond the window, sort of a peek forward for one time messages
   {
      FileReader::MessageAndSizeArray messages;
      Page*                           streamPage;
      int                             pageIndex = mReader->UpdateStream(messages, streamPage, aStreamState);
      if (!messages.empty())
      {
         performedWork = true;

         Block* block = nullptr;
         if (mStreamingLastBlock)
         {
            block = &mBlocks.back();
         }

         float maxTime = messages.back().first->simTime();
         for (size_t i = 0; i < messages.size(); ++i)
         {
            FileReader::MessageAndSize msgAndSize       = messages[i];
            rv::MsgBase*               msgPtr           = msgAndSize.first;
            bool                       messageIsOneTime = rv::IsOneTimeMessage(msgPtr);
            int                        msgId            = msgAndSize.first->GetMessageId();

            ++mMessageCountsByType[msgId];
            mMessageBytesByType[msgId] += msgAndSize.second;

            if (messageIsOneTime)
            {
               mDb->ProcessOneTimeMessage(msgPtr);
               msgPtr->Unref();
            }
            else
            {
               if (mStreamingLastBlock)
               {
                  // TODO: need to process messages even if they aren't in scope
                  mDb->PushBack(msgPtr);
                  if (block)
                  {
                     block->mMessages->push_back(msgPtr);
                  }
               }
               else
               {
                  msgPtr->Unref();
               }
            }
         }
         if (block)
         {
            block->mMaxTime = maxTime;
            if (!block->mMessages->empty())
            {
               block->mMinTime = block->mMessages->front()->simTime();
            }
         }
         mDb->SetMaxTime(maxTime);
         // this may be the end... but how do we know?
         mResetClock.ResetClock();

         if (pageIndex != -1)
         {
            // completed a page
#if DEBUG_STREAMING
            std::cerr << "Stream block complete: " << pageIndex << '\n';
#endif
            ++mCurrentStreamPage;
            if (mStreamingLastBlock)
            {
               if (block)
               {
                  block->mPageNumber = pageIndex;
               }
               mStreamingLastBlock = false;
               while ((int)mBlocks.size() > mPagesInMemoryLimit)
               {
                  PruneFromStart();
               }
            }
         }
         TimeWindowChanged();
      }
   }

   return performedWork;
}

void MessageCache::TimeWindowChanged()
{
   if (mBlocks.empty())
   {
      mDb->SetTimeRangeInCache(0, 0);
   }
   else
   {
      if (mBlocks.back().mMaxTime >= 0)
      {
         mDb->SetTimeRangeInCache(mBlocks.front().mMinTime, mBlocks.back().mMaxTime);
      }
   }
}

} // namespace RvWsfPipe
