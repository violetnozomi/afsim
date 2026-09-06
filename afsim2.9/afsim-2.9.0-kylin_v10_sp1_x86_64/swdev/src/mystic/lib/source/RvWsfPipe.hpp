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

#ifndef RVWFSPIPE_HPP
#define RVWFSPIPE_HPP
#include <cfloat>
#include <fstream>
#include <queue>
#include <set>
#include <vector>

#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>

#include "RvEventPipeMessages.hpp"
#include "RvResultDb.hpp"
#include "RvWsfPipe_FileStreamer.hpp"
#include "UtCircularBuffer.hpp"
#include "UtPack.hpp"
#include "UtPackStream.hpp"
#include "UtWallClock.hpp"

class UtPackMessage;
class UtPackMessageStreamI;
class UtPackMessageStdStreamO;

namespace RvWsfPipe
{
constexpr size_t cMAX_SIZE_T = std::numeric_limits<size_t>::max();

// Manages storage of messages in memory and offload to disk
class MessageCache
{
public:
   MessageCache(rv::ResultDb* aDb, unsigned int aPageLimit);
   ~MessageCache();

   void SetReader(FileReader* aReader) { mReader = aReader; }

   std::map<size_t, size_t> mMessageCountsByType;
   std::map<size_t, size_t> mMessageBytesByType;

   // mutators
   void SetCurrentTime(float aSimTime);
   void SetSchema(const UtPackSchema* aSchema);

   const std::map<size_t, size_t>& GetMessageCountByType() const { return mMessageCountsByType; }
   const std::map<size_t, size_t>& GetMessageBytesByType() const { return mMessageBytesByType; }

   bool Update(StreamState& aStreamState);

private:
   bool StartStreamingLastBlock();
   void PruneFromStart();
   void PruneFromEnd();

   void TimeWindowChanged();

   rv::ResultDb* mDb;

   typedef std::map<unsigned int, rv::ResultMessage>  MessageSet;
   typedef std::pair<unsigned int, rv::ResultMessage> MsgEntry;

   // expiration time, message id
   // for all lasting messages with defined expiration times

   typedef UtCircularBuffer<rv::MsgBase*> MessageQueue;

   struct Block
   {
      int                        mPageNumber = 0;
      float                      mMinTime    = 0.0;
      float                      mMaxTime    = 0.0;
      std::vector<rv::MsgBase*>* mMessages   = nullptr;
   };

   UtCircularBuffer<Block> mBlocks;
   // is the last block being read and processed?
   bool mStreamingLastBlock;

   RvWsfPipe::FileReader* mReader;
   int                    mCurrentStreamPage;
   int                    mPagesInMemoryLimit;
   // Maximum number of messages allowed to be retained in memory
   // Keeps us from running out of memory or swapping

   float       mCurrentTime;
   UtWallClock mResetClock;
};
void dump_timer_data();
} // namespace RvWsfPipe

#endif
