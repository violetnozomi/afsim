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
#ifndef RVWSFPIPE_FILESTREAMER_HPP
#define RVWSFPIPE_FILESTREAMER_HPP

#include <iostream>
#include <mutex>
#include <set>

#include <QThread>

#include "RvChannel.hpp"
#include "RvEventPipeClasses.hpp"
#include "UtPack.hpp"
#include "UtWallClock.hpp"

namespace RvWsfPipe
{
class MessageCache;

struct Page
{
   static const int cMINIMUM_PAGE_BYTES = 500000;
   // static const int cMINIMUM_PAGE_BYTES = 5000;
   //  byte offset in file
   std::streamoff mStreamOffset;
   // index of event, starting at 0
   unsigned int mEventIndex;
   // number of messages in page
   int mMessagesContained;

   int mPageBytes;
};
enum StreamState
{
   cOK                 = 0,
   cRESET_REQUIRED     = 1,
   cFAILED             = 2,
   cTIME_TYPE_MISMATCH = 3
};

// this is what actually reads from the file
class FileStreamer
{
public:
   FileStreamer(const std::string& aFileName, MessageCache* aCachePtr);

   ~FileStreamer() = default;

   struct MessageData
   {
      rv::MsgBase* mMessagePtr  = nullptr;
      int          mMessageSize = 0;
      // file offset AFTER message data
      size_t mMessageFileOffset = 0;
      bool   mIgnored           = false;
   };

   // Attempt to read more data from the input stream
   // State machine is used to handle partial reads
   // because the file may be being written to
   MessageData             Update();
   StreamState             GetState() const;
   void                    SetFailed() { mState = cREAD_ERROR; }
   std::string             GetFileName() const { return mFileName; }
   const UtPackSerializer* GetSerializer() const { return mSerializer.get(); }

private:
   enum State
   {
      cREAD_HEADER,
      cREAD_HEADER_CONTINUE,
      cREAD_MESSAGE,
      cREAD_MESSAGE_CONTINUE,
      cREAD_ERROR,
      cREAD_RESET_REQUIRED,
      cREAD_ERROR_TIME_TYPE_MISMATCH
   };

   size_t           GetMessageFileOffset() const { return mCurrentMessageOffset; }
   int              GetMessageSize() const { return mCurrentMessageHeader.mSize; }
   void             ReadHeader();
   void             ReadHeaderContinue();
   void             HeaderComplete();
   UtPackMessage*   ReadMessage(bool& aIgnored);
   UtPackMessage*   ReadMessageDirect();
   UtPackMessage*   ReadMessageContinue(bool& aIgnored);
   rv::MsgExecData* ReadExecMessage();
   std::string      LookupMessageNameInSchema(unsigned int aId);
   void             BuildUnknownMessageMap();
   bool             CompareUtml(UtmlObject& aLocal, UtmlObject& aAER) const;
   bool             CompareMessage(const std::string& aName, UtPackStruct* aLocal, UtPackStruct* aRemote) const;
   void             StructToStream(std::ostream& aStream, UtPackStruct* aStruct, unsigned int aDepth = 1) const;
   void             TypeContentsToStream(std::ostream& aStream, UtPackType* aType, unsigned int aDepth) const;
   bool             UsesDoubleTime(UtmlObject& aObject) const;

   std::string                           mFileName;
   State                                 mState;
   std::unique_ptr<std::ifstream>        mStreamPtr;
   std::string                           mHeaderString;
   std::unique_ptr<UtPackSchema>         mSchema;
   std::unique_ptr<UtPackSerializer>     mSerializer;
   UtPackStream                          mPackBuffer;
   int                                   mHeaderBytes;
   UtPackMessageStream::PacketHeaderInfo mCurrentMessageHeader;
   size_t                                mCurrentMessageOffset;
   size_t                                mCurrentFileOffset;
   MessageCache*                         mCachePtr;
   std::string                           mExecDataTimeStamp{""};
   size_t                                mExecDataOffset{0};
   double                                mExecDataReviewTime{-1.0};
   UtWallClock                           mTimer;
   std::map<unsigned int, std::string>   mUnknownMessageMap;
   mutable std::set<QString>             mIgnoredMessageNames;
   mutable std::set<unsigned int>        mIgnoredMessageIds;
   bool                                  mWarningDisplayed;
};

class FileStreamerThread : public QThread
{
public:
   FileStreamerThread(FileStreamer* aStream)
      : mQuit(false)
      , mChannel(3000)
      , mStream(aStream)
   {
   }
   ~FileStreamerThread() override;
   bool try_pop(FileStreamer::MessageData& aData) { return mChannel.try_pop(aData); }

   void quit() { mQuit = true; }

protected:
   void                                   run() override;
   volatile bool                          mQuit;
   rv::Channel<FileStreamer::MessageData> mChannel;
   FileStreamer*                          mStream;
};

class FileBlockStreamer
{
public:
   FileBlockStreamer(const FileStreamer& aSrc);

   void ReadBlock(size_t aOffset, size_t aBytes, unsigned int aMessageIndex, std::vector<rv::MsgBase*>& aMessages);

private:
   // Returns false if at end of data
   // returns true if more messages may exist
   // aMsgPtr may be set to null if an unknown message is found
   bool ReadMessageP(UtPackMessage*& aMsgPtr);

   std::string                    mFileName;
   std::unique_ptr<std::ifstream> mStreamPtr;
   const UtPackSerializer*        mSerializer;
   UtPackStream                   mPackBuffer;
};

class FileReader
{
public:
   using MessageAndSize      = std::pair<rv::MsgBase*, int>;
   using MessageAndSizeArray = std::vector<MessageAndSize>;

   FileReader(FileStreamer* aStream);
   ~FileReader();

   void Stop();

   void ReadPage(int aPageIndex, std::vector<rv::MsgBase*>& aMessages);

   int UpdateStream(MessageAndSizeArray& aMessages, Page*& aPagePtr, StreamState& aStreamState);

   void RegainStream(std::vector<rv::MsgBase*>& aAlreadyStreamedMessages);

private:
   std::vector<Page> mPages;
   Page              mStreamPage;

   FileStreamer*                      mFirstPass;
   FileStreamerThread*                mFirstPassThread;
   std::unique_ptr<FileBlockStreamer> mBlockReader;
   std::vector<rv::MsgBase*>          mStreamedMessages;
};
} // namespace RvWsfPipe

#endif
