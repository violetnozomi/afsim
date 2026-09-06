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
#include "RvWsfPipe_FileStreamer.hpp"

#include <array>
#include <iostream>

#include <QMessageBox>

#include "RvEventPipeUtil.hpp"
#include "RvWsfPipe.hpp"
#include "UtCast.hpp"
#include "UtException.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPackReflect.hpp"
#include "UtPackSchema.hpp"
#include "UtTextDocument.hpp"
#include "Utml.hpp"
#include "WkfQueueableMessageObject.hpp"

#define RVWSFPIPEBUFFERSIZE 1024


RvWsfPipe::FileStreamer::FileStreamer(const std::string& aFileName, MessageCache* aCachePtr)
   : mFileName(aFileName)
   , mState(cREAD_HEADER)
   , mCachePtr(aCachePtr)
   , mWarningDisplayed(false)
{
   mStreamPtr = ut::make_unique<std::ifstream>(aFileName.c_str(), std::ios::binary);
   mPackBuffer.mBuffer.CheckPutSpace(RVWSFPIPEBUFFERSIZE);
}

// reads the files header
void RvWsfPipe::FileStreamer::ReadHeader()
{
   constexpr int                  cHEADER_SIZE = 11;
   std::array<char, cHEADER_SIZE> headerBuffer;
   mStreamPtr->seekg(0, std::ios::beg);
   mStreamPtr->read(headerBuffer.data(), cHEADER_SIZE);
   if (mStreamPtr->gcount() != cHEADER_SIZE)
   {
      // header not available yet
      mState = cREAD_HEADER;
      return;
   }
   // check against a 11 byte 'magic value'
   if (memcmp(headerBuffer.data(), "\0\0WSF_PIPE\n", cHEADER_SIZE) == 0)
   {
      // we are okay to continue reading
      ReadHeaderContinue();
      return;
   }
   else
   {
      wkf::QueueableMessageObject::DisplayQueuedMessage(QMessageBox::Warning,
                                                        "Event Pipe Error",
                                                        "Invalid Header, this may not be an event pipe file");
      // this is not an event pipe file.  Halt!
      std::cerr << "event_pipe: Read Invalid Header\n";
      for (auto c : headerBuffer)
      {
         std::cerr << "  Rcv: " << static_cast<int>(c) << '\t';
      }
      std::cerr << '\n';
      mState = cREAD_ERROR;
   }
}

// lets figure out what else is in the header
void RvWsfPipe::FileStreamer::ReadHeaderContinue()
{
   char c;
   // capture a null terminated header string
   while (mStreamPtr->get(c))
   {
      if (c == 0)
      {
         return HeaderComplete();
      }
      else
      {
         mHeaderString += c;
      }
   }
   mStreamPtr->clear();
   mState = cREAD_HEADER_CONTINUE;
}

void RvWsfPipe::FileStreamer::HeaderComplete()
{
   UtTextDocument localDoc;
   rv::GetSchema(localDoc);

   UtmlParser localParser(localDoc);
   UtmlObject localUtmlData = UtmlObject::makeContainer("");
   try
   {
      int orval = 0;
      if (localParser.Parse(localUtmlData))
      {
         int count = localUtmlData.GetObjectCount();
         for (int i = 0; i < count; ++i)
         {
            UtmlObject& obj = localUtmlData.GetObject(i);
            if (obj.GetType() == "Settings")
            {
               UtmlObject* orprop = obj.FindProperty("message_id_or");
               if (orprop)
               {
                  orval = std::stoi(orprop->GetValue());
               }
            }
            UtmlObject* messageIdProp = obj.FindProperty("message");
            if (messageIdProp)
            {
               if (messageIdProp->GetValue() != "virtual")
               {
                  messageIdProp->SetValue(std::to_string((orval << 24) + std::stoi(messageIdProp->GetValue())));
               }
            }
         }
      }
   }
   catch (UtmlParseError& e)
   {
      std::cerr << "Local UTML Read Error: " << e.what();
      mState = cREAD_ERROR;
      return;
   }

   UtTextDocument doc;
   doc.Insert(0, mHeaderString);
   UtmlParser parser(doc);
   UtmlObject utmlData = UtmlObject::makeContainer("");
   try
   {
      if (parser.Parse(utmlData))
      {
         if (UsesDoubleTime(utmlData) != UsesDoubleTime(localUtmlData))
         {
            mState = cREAD_ERROR_TIME_TYPE_MISMATCH;
            return;
         }

         CompareUtml(localUtmlData, utmlData);
         mSchema = ut::make_unique<UtPackSchema>();
         mSchema->Read(localUtmlData);
         mSchema->Resolve();
         mCurrentFileOffset = mStreamPtr->tellg();
         mState             = cREAD_MESSAGE;
         mSerializer        = ut::make_unique<UtPackSerializer>();
         rv::RegisterSerializer(*mSerializer);
         mSerializer->Initialize(*mSchema);
         if (mCachePtr)
         {
            mCachePtr->SetSchema(mSchema.get());
         }
         return;
      }
      else
      {
         std::cerr << "Failed to parse event_pipe schema\n";
      }
   }
   catch (UtmlParseError& e)
   {
      std::cerr << e.what();
      mState = cREAD_ERROR;
      return;
   }
   mState = cREAD_ERROR;
}

UtPackMessage* RvWsfPipe::FileStreamer::ReadMessageContinue(bool& aIgnored)
{
   // continue reading a message that may have been partially read into mPackBuffer
   // where mCurrentMessageHeader contains the header that has already been read
   size_t putPos    = mPackBuffer.mBuffer.GetPutPos();
   size_t bytesLeft = mCurrentMessageHeader.mSize - putPos + 4 /*header adjustment*/;
   // if we come across a message that is too big for the buffer
   if (bytesLeft - putPos > mPackBuffer.mBuffer.GetBytes())
   {
      int n = int(log2(bytesLeft)) + 1;
      mPackBuffer.mBuffer.CheckPutSpace(pow(2, n));
   }
   mStreamPtr->read(mPackBuffer.mBuffer.GetBuffer() + putPos, bytesLeft);
   size_t bytesRead = mStreamPtr->gcount();
   bytesLeft -= bytesRead;
   mPackBuffer.mBuffer.SetPutPos(putPos + bytesRead);
   if (bytesLeft <= 0)
   {
      UtPackSerializer::MessageInfo* msgInfo = nullptr;
      if (mIgnoredMessageIds.find(mCurrentMessageHeader.mPacketType) == mIgnoredMessageIds.end())
      {
         msgInfo = mSerializer->GetMessageInfo(mCurrentMessageHeader.mPacketType);
      }
      else
      {
         aIgnored = true;
         if (!mWarningDisplayed)
         {
            QString msg = QString("The recording's schema does not match the application schema.<br>") +
                          "This likely indicates a AFSIM/Mystic version mismatch.<br>" +
                          "Mystic will ignore the following messages:<br><br>";
            for (auto&& it : mIgnoredMessageNames)
            {
               msg += it + "<br>";
            }
            wkf::QueueableMessageObject::DisplayQueuedMessage(QMessageBox::Warning, "Event Pipe Error", msg);

            mWarningDisplayed = true;
         }
      }
      if (msgInfo)
      {
         UtPackMessage* msg = msgInfo->mMessageFactory();
         msgInfo->mSerializeFunction->Read(mPackBuffer, msg);
         // update file offset
         mCurrentMessageOffset = mCurrentFileOffset;
         mCurrentFileOffset += mPackBuffer.mBuffer.GetPutPos();

         mPackBuffer.mBuffer.Reset();
         mState = cREAD_MESSAGE;
         return msg;
      }
      else
      {
         // we don't know this message, but we can try to proceed anyway...
         mCurrentMessageOffset = mCurrentFileOffset; // I added this
         mCurrentFileOffset += mPackBuffer.mBuffer.GetPutPos();
         mPackBuffer.mBuffer.Reset();
         mState = cREAD_MESSAGE;
         return nullptr; // and this to skip unknown messages... we should still do something about this...
      }
   }
   else if (bytesRead == 0)
   {
      mStreamPtr->clear();
   }
   mState = cREAD_MESSAGE_CONTINUE;
   return nullptr;
}

std::string RvWsfPipe::FileStreamer::LookupMessageNameInSchema(unsigned int aId)
{
   if (mUnknownMessageMap.empty())
   {
      BuildUnknownMessageMap();
   }
   std::map<unsigned int, std::string>::const_iterator it = mUnknownMessageMap.find(aId);
   if (it != mUnknownMessageMap.end())
   {
      return it->second;
   }
   return "UNKNOWN";
}

void RvWsfPipe::FileStreamer::BuildUnknownMessageMap()
{
   std::vector<std::string> sl = mSchema->GetTypeNames();
   for (auto&& it : sl)
   {
      UtPackType*   t  = mSchema->FindType(it);
      UtPackStruct* ts = dynamic_cast<UtPackStruct*>(t);
      if (ts)
      {
         if (ts->IsMessage())
         {
            mUnknownMessageMap[ts->GetMessageId()] = it;
         }
      }
   }
}

bool RvWsfPipe::FileStreamer::CompareUtml(UtmlObject& aLocal, UtmlObject& aAER) const
{
   UtPackSchema localSchema;
   localSchema.Read(aLocal);
   localSchema.Resolve();

   UtPackSchema remoteSchema;
   remoteSchema.Read(aAER);
   remoteSchema.Resolve();
   bool retval = true;

   for (auto&& it : remoteSchema.GetTypeNames())
   {
      UtPackType* rtype = remoteSchema.FindType(it);
      if (rtype)
      {
         if (rtype->IsStruct())
         {
            UtPackStruct* rstr = static_cast<UtPackStruct*>(rtype);
            if (rstr->IsMessage())
            {
               UtPackStruct* lstr = static_cast<UtPackStruct*>(localSchema.FindType(it));
               bool          comp = CompareMessage(it, lstr, rstr);
               if (!comp)
               {
                  // system should ignore rstr->GetMessageId();
                  mIgnoredMessageNames.insert(it.c_str());
                  mIgnoredMessageIds.insert(rstr->GetMessageId());
               }
               retval = retval && comp;
            }
         }
      }
   }

   return retval;
}

bool RvWsfPipe::FileStreamer::CompareMessage(const std::string& aName, UtPackStruct* aLocal, UtPackStruct* aRemote) const
{
   if (aLocal)
   {
      if (aLocal->IsMessage())
      {
         std::stringstream rmessageStr;
         rmessageStr << "MESSAGE " << aRemote->GetMessageId() << std::endl;
         StructToStream(rmessageStr, aRemote);

         std::stringstream lmessageStr;
         lmessageStr << "MESSAGE " << aLocal->GetMessageId() << std::endl;
         StructToStream(lmessageStr, aLocal);
         if (rmessageStr.str() == lmessageStr.str())
         {
            return true;
         }
         else
         {
            ut::log::warning() << "-----Remote Schema " << aName << "-----\n"
                               << rmessageStr.str() << "-----Local Schema " << aName << "------\n"
                               << lmessageStr.str() << "=================";
            return false;
            // remote message is not the same as the local one... ignore it
         }
      }
      else
      {
         // remote message is not available locally... ignore it
         return false;
      }
   }
   else
   {
      // remote message is not available locally... ignore it
      return false;
   }
}

void RvWsfPipe::FileStreamer::StructToStream(std::ostream& aStream, UtPackStruct* aStruct, unsigned int aDepth) const
{
   const UtPackTypeReference& base = aStruct->GetBaseType();
   if (!base.IsNull())
   {
      UtPackType* basetype = base.GetType();
      if (basetype->IsStruct())
      {
         StructToStream(aStream, static_cast<UtPackStruct*>(basetype), aDepth);
      }
   }
   const std::vector<UtPackField>& fields = aStruct->GetFields();
   for (auto&& jt : fields)
   {
      const UtPackTypeReference& tr = jt.GetType();
      for (unsigned int i = 0; i < aDepth; ++i)
      {
         aStream << "   ";
      }
      if (tr.GetType()->IsList())
      {
         // the typename doesn't really matter if it has the same field name, and same contained type
         UtPackListType* listtype = static_cast<UtPackListType*>(tr.GetType());
         aStream << "list<" << listtype->mContainedType.GetType()->GetTypeName() << "> " << jt.GetFieldName();
      }
      else
      {
         aStream << tr.GetType()->GetTypeName() << " " << jt.GetFieldName();
      }
      TypeContentsToStream(aStream, tr.GetType(), aDepth);
   }
}

void RvWsfPipe::FileStreamer::TypeContentsToStream(std::ostream& aStream, UtPackType* aType, unsigned int aDepth) const
{
   if (aType->IsEnum())
   {
      UtPackEnumType* enumtype = static_cast<UtPackEnumType*>(aType);
      aStream << " { ";
      const std::map<int, std::string>& vals = enumtype->GetValueToName();
      for (auto&& kt : vals)
      {
         aStream << kt.second << " ";
      }
      aStream << "}" << std::endl;
   }
   else if (aType->IsList())
   {
      UtPackListType* listtype = static_cast<UtPackListType*>(aType);
      TypeContentsToStream(aStream, listtype->mContainedType.GetType(), aDepth);
   }
   else
   {
      aStream << std::endl;
      if (aType->IsStruct())
      {
         StructToStream(aStream, static_cast<UtPackStruct*>(aType), aDepth + 1);
      }
   }
}

UtPackMessage* RvWsfPipe::FileStreamer::ReadMessage(bool& aIgnored)
{
   int putPos            = ut::cast_to_int(mPackBuffer.mBuffer.GetPutPos());
   int headerBytesToRead = sizeof(mCurrentMessageHeader) - putPos;

   mStreamPtr->read(mPackBuffer.mBuffer.GetBuffer() + putPos, headerBytesToRead);
   int bytesRead = mStreamPtr->gcount();
   mPackBuffer.mBuffer.SetPutPos(putPos + bytesRead);

   if (!*mStreamPtr)
   {
      mStreamPtr->clear();
      return nullptr;
   }

   if (bytesRead >= headerBytesToRead)
   {
      memcpy(&mCurrentMessageHeader, mPackBuffer.mBuffer.GetBuffer(), sizeof(mCurrentMessageHeader));
      mPackBuffer.mBuffer.SetGetPos(sizeof(mCurrentMessageHeader));
      return ReadMessageContinue(aIgnored);
   }
   return nullptr;
}

// this will read a message, kind of like ReadMessage, but doesn't use any of the
// persistent buffers, and thus is not expected to have a lasting effect on the state
// of the streamer.  This is so that we can take a peek at the execution message
// (rv::MsgExedData) and note if the time stamp has changed and trigger a forced reload,
// as necessary
UtPackMessage* RvWsfPipe::FileStreamer::ReadMessageDirect()
{
   UtPackMessageStream::PacketHeaderInfo header;
   UtPackStream                          buffer;
   int                                   headerBytesToRead = sizeof(header);
   mStreamPtr->read(buffer.mBuffer.GetBuffer(), headerBytesToRead);
   int bytesRead = mStreamPtr->gcount();
   buffer.mBuffer.SetPutPos(bytesRead);

   if (!*mStreamPtr) // we failed to read
   {
      mStreamPtr->clear();
      return nullptr;
   }

   if (bytesRead >= headerBytesToRead)
   {
      memcpy(&header, buffer.mBuffer.GetBuffer(), sizeof(header));
      buffer.mBuffer.SetGetPos(sizeof(header));

      size_t putPos    = buffer.mBuffer.GetPutPos();
      size_t bytesLeft = header.mSize - putPos + 4 /*header adjustment*/;
      // if we come across a message that is too big for the buffer
      buffer.mBuffer.CheckPutSpace(bytesLeft);
      mStreamPtr->read(buffer.mBuffer.GetBuffer() + putPos, bytesLeft);
      size_t bytesRead = mStreamPtr->gcount();
      bytesLeft -= bytesRead;
      buffer.mBuffer.SetPutPos(putPos + bytesRead);
      if (bytesLeft <= 0)
      {
         UtPackSerializer::MessageInfo* msgInfo = mSerializer->GetMessageInfo(header.mPacketType);
         if (msgInfo)
         {
            UtPackMessage* msg = msgInfo->mMessageFactory();
            msgInfo->mSerializeFunction->Read(buffer, msg);
            return msg;
         }
         else
         {
            return nullptr; // and this to skip unknown messages... we should still do something about this...
         }
      }
      else if (bytesRead == 0)
      {
         mStreamPtr->clear();
      }
      return nullptr;
      ///////////////////////////////
   }
   return nullptr;
}

RvWsfPipe::FileStreamer::MessageData RvWsfPipe::FileStreamer::Update()
{
   bool           ignored = false;
   UtPackMessage* msgPtr  = nullptr;
   switch (mState)
   {
   case cREAD_HEADER:
      ReadHeader();
      break;
   case cREAD_HEADER_CONTINUE:
      ReadHeaderContinue();
      break;
   case cREAD_MESSAGE:
      msgPtr = ReadMessage(ignored);
      break;
   case cREAD_MESSAGE_CONTINUE:
      msgPtr = ReadMessageContinue(ignored);
      break;
   case cREAD_RESET_REQUIRED:
      break;
   case cREAD_ERROR:
      break;
   case cREAD_ERROR_TIME_TYPE_MISMATCH:
      break;
   }

   MessageData msg;
   msg.mIgnored    = ignored;
   msg.mMessagePtr = dynamic_cast<rv::MsgBase*>(msgPtr);
   if (msgPtr)
   {
      msg.mMessageSize       = mCurrentMessageHeader.mSize;
      msg.mMessageFileOffset = mCurrentMessageOffset;
      if (msgPtr->GetMessageId() == rv::MsgExecData::cMESSAGE_ID)
      {
         rv::MsgExecData* execMsg = (rv::MsgExecData*)msgPtr;
         mExecDataTimeStamp       = execMsg->timeStamp();
         mExecDataOffset          = mCurrentMessageOffset;
         mExecDataReviewTime      = mTimer.GetClock() + 5;
         // set up something to check on this at some frequency to make sure it doesn't change
      }
   }
   if (!mExecDataTimeStamp.empty())
   {
      double t = mTimer.GetClock();
      if (t > mExecDataReviewTime)
      {
         rv::MsgExecData* execMsg = ReadExecMessage();
         if (execMsg)
         {
            std::string timestamp = execMsg->timeStamp();
            delete execMsg;
            if (timestamp != mExecDataTimeStamp)
            {
               ut::log::warning() << "timestamp mismatch " + mExecDataTimeStamp + " " + timestamp;
               mExecDataTimeStamp  = "";
               mExecDataReviewTime = -1;
               mExecDataOffset     = 0;
               mState              = cREAD_RESET_REQUIRED;
            }
            else
            {
               mExecDataReviewTime = t + 5; // lets schedule another check
            }
         }
         else
         {
            mExecDataTimeStamp  = "";
            mExecDataReviewTime = -1;
            mExecDataOffset     = 0;
            mState              = cREAD_RESET_REQUIRED;
         }
      }
   }

   return msg;
}

RvWsfPipe::StreamState RvWsfPipe::FileStreamer::GetState() const
{
   switch (mState)
   {
   case (cREAD_RESET_REQUIRED):
      return cRESET_REQUIRED;
   case (cREAD_ERROR):
      return cFAILED;
   case (cREAD_ERROR_TIME_TYPE_MISMATCH):
      return cTIME_TYPE_MISMATCH;
   default:
      return cOK;
   }
}

rv::MsgExecData* RvWsfPipe::FileStreamer::ReadExecMessage()
{
   rv::MsgExecData* retval = nullptr;
   mStreamPtr->clear();
   size_t offset = mStreamPtr->tellg(); // get the get offset on the file
   mStreamPtr->seekg(mExecDataOffset);  // set the get offset to mExecDataOffset
   size_t         actual = mStreamPtr->tellg();
   UtPackMessage* msg    = ReadMessageDirect();
   mStreamPtr->seekg(offset); // set the get offset to the previous offset

   if (msg)
   {
      if (msg->GetMessageId() != rv::MsgExecData::cMESSAGE_ID)
      {
         ut::log::warning() << std::string("wrong message found at ") + std::to_string(mExecDataOffset) + "(" +
                                  std::to_string(actual) + ") " + std::to_string(msg->GetMessageId());

         delete msg;
      }
      else
      {
         retval = dynamic_cast<rv::MsgExecData*>(msg);
      }
   }
   else
   {
      ut::log::warning() << std::string("no message found at ") + std::to_string(mExecDataOffset) + "(" +
                               std::to_string(actual) + ")";
   }
   return retval;
}

RvWsfPipe::FileBlockStreamer::FileBlockStreamer(const FileStreamer& aSrc)
   : mFileName(aSrc.GetFileName())
   , mSerializer(aSrc.GetSerializer())
{
   mStreamPtr = ut::make_unique<std::ifstream>(mFileName.c_str(), std::ios::binary);
   mPackBuffer.mBuffer.CheckPutSpace(RVWSFPIPEBUFFERSIZE);
}

bool RvWsfPipe::FileBlockStreamer::ReadMessageP(UtPackMessage*& aMsgPtr)
{
   aMsgPtr               = nullptr;
   size_t bytesAvailable = mPackBuffer.mBuffer.GetValidBytes();
   if (sizeof(UtPackMessageStream::PacketHeaderInfo) > bytesAvailable)
      return false;

   UtPackMessageStream::PacketHeaderInfo header;
   mPackBuffer.mBuffer.GetRaw((char*)&header, sizeof(header));

   int bytesLeft = ut::cast_to_int(bytesAvailable - sizeof(header));
   if (bytesLeft + 4 >= header.mSize)
   {
      const UtPackSerializer::MessageInfo* msgInfo = mSerializer->GetMessageInfo(header.mPacketType);
      if (msgInfo)
      {
         aMsgPtr = msgInfo->mMessageFactory();
         msgInfo->mSerializeFunction->Read(mPackBuffer, aMsgPtr);
         return true;
      }
      else
      {
         mPackBuffer.mBuffer.SetPutPos(mPackBuffer.mBuffer.GetPutPos() + header.mSize - 4);
         return true;
      }
   }
   return false;
}

void RvWsfPipe::FileBlockStreamer::ReadBlock(size_t                     aOffset,
                                             size_t                     aBytes,
                                             unsigned int               aFirstMessageIndex,
                                             std::vector<rv::MsgBase*>& aMessages)
{
   mPackBuffer.mBuffer.Reset();
   mPackBuffer.mBuffer.CheckPutSpace(aBytes);
   mStreamPtr->seekg(aOffset);
   mStreamPtr->read(mPackBuffer.mBuffer.GetBuffer(), aBytes);
   assert((size_t)mStreamPtr->gcount() == aBytes); // should always get the bytes we request
   mPackBuffer.mBuffer.SetPutPos(aBytes);

   unsigned int   msgIndex = aFirstMessageIndex;
   UtPackMessage* msg      = nullptr;
   while (ReadMessageP(msg))
   {
      if (msg)
      {
         rv::MsgBase* epMsg = dynamic_cast<rv::MsgBase*>(msg);
         epMsg->SetMessageIndex(msgIndex);
         aMessages.push_back(epMsg);
         ++msgIndex;
      }
   }
}

RvWsfPipe::FileReader::FileReader(FileStreamer* aStream)
{
   mFirstPass       = aStream;
   mFirstPassThread = new FileStreamerThread(mFirstPass);
   mFirstPassThread->start();

   mStreamPage.mEventIndex        = 1;
   mStreamPage.mMessagesContained = 0;
   mStreamPage.mStreamOffset      = 0;
}

RvWsfPipe::FileReader::~FileReader()
{
   Stop();

   for (auto& message : mStreamedMessages)
   {
      message->Unref();
   }
}

void RvWsfPipe::FileReader::ReadPage(int aPageIndex, std::vector<rv::MsgBase*>& aMessages)
{
   if (!mBlockReader)
   {
      mBlockReader = ut::make_unique<FileBlockStreamer>(*mFirstPass);
   }
   Page p = mPages[aPageIndex];
   mBlockReader->ReadBlock(p.mStreamOffset, p.mPageBytes, p.mEventIndex, aMessages);
}

int RvWsfPipe::FileReader::UpdateStream(MessageAndSizeArray& aMessages, Page*& aPagePtr, StreamState& aStreamState)
{
   FileStreamer::MessageData data;
   unsigned int              nextEventIndex = mStreamPage.mEventIndex + mStreamPage.mMessagesContained;
   for (int i = 0; i < 100 && mFirstPassThread->try_pop(data); ++i)
   {
      // messages in the streaming page are ref-counted until the page is complete
      // at which point they are unreferenced
      // this lets us avoid the case where a partially-streamed page needs to be read back in
      data.mMessagePtr->Ref();
      data.mMessagePtr->SetMessageIndex(nextEventIndex);
      mStreamedMessages.push_back(data.mMessagePtr);

      aMessages.emplace_back(MessageAndSize(data.mMessagePtr, data.mMessageSize));
      if (mStreamPage.mMessagesContained == 0)
      {
         mStreamPage.mStreamOffset = data.mMessageFileOffset;
         mStreamPage.mPageBytes    = 0;
      }
      ++mStreamPage.mMessagesContained;
      ++nextEventIndex;
      mStreamPage.mPageBytes += data.mMessageSize + 4; // total message size is 4 + reported size
      if (mStreamPage.mPageBytes > Page::cMINIMUM_PAGE_BYTES)
      {
         // page is complete
         int pageNumber = (int)mPages.size();
         mPages.push_back(mStreamPage);
         mStreamPage.mEventIndex += mStreamPage.mMessagesContained;
         mStreamPage.mStreamOffset      = 0;
         mStreamPage.mMessagesContained = 0;
         aPagePtr                       = &mPages.back();
         for (auto& message : mStreamedMessages)
         {
            message->Unref();
         }
         mStreamedMessages.clear();
         return pageNumber;
      }
   }
   aStreamState = mFirstPass->GetState();

   aPagePtr = &mStreamPage;
   return -1;
}

void RvWsfPipe::FileReader::RegainStream(std::vector<rv::MsgBase*>& aAlreadyStreamedMessages)
{
   aAlreadyStreamedMessages = mStreamedMessages;
}

// Stop the thread and delete it.
void RvWsfPipe::FileReader::Stop()
{
   if (mFirstPassThread)
   {
      mFirstPassThread->quit();
      mFirstPassThread->wait();
      delete mFirstPassThread;
      mFirstPassThread = nullptr;
   }
}

RvWsfPipe::FileStreamerThread::~FileStreamerThread()
{
   // free messages not yet processed
   FileStreamer::MessageData data;
   while (try_pop(data))
   {
      delete data.mMessagePtr;
   }
}

void RvWsfPipe::FileStreamerThread::run()
{
   // when we get to the end of file, there may be another process writing data
   // or it could be a static file.  To adjust for either situation, we use a variable wait time
   // that increases as no more data is encountered.  Eventually we are waiting 2 seconds between each check
   const int cMIN_WAIT    = 10;
   const int cMAX_WAIT    = 2000;
   int       waitDuration = cMIN_WAIT;
   try
   {
      ut::SetupThreadErrorHandling();
      for (; !mQuit;)
      {
         FileStreamer::MessageData msgData = mStream->Update();
         if (msgData.mMessagePtr)
         {
            waitDuration = cMIN_WAIT;
            // if channel is full, check for quit flag every 200 ms
            while (!mChannel.push(msgData, 200))
            {
               if (mQuit)
               {
                  delete msgData.mMessagePtr;
                  return;
               }
            }
         }
         else if (!msgData.mIgnored)
         {
            // wait for more data
            QThread::msleep(waitDuration);
            if (waitDuration < cMAX_WAIT)
               waitDuration += 10;
         }
      }
   }
   catch (...) // a problem occurred while trying to read the file, offer a reload or quit
   {
      mStream->SetFailed();
   }
}

// if the first settings do not indicate that this file uses double time, we can not read it
bool RvWsfPipe::FileStreamer::UsesDoubleTime(UtmlObject& aObject) const
{
   int timeVal = 0;
   for (int i = 0; i < aObject.GetObjectCount(); ++i)
   {
      UtmlObject& obj = aObject.GetObject(i);
      if (obj.GetType() == "Settings")
      {
         UtmlObject* timeprop = obj.FindProperty("double_time");
         if (timeprop)
         {
            timeVal = std::stoi(timeprop->GetValue());
            if (timeVal)
            {
               return true;
            }
         }
      }
      return false;
   }
   return false;
}
