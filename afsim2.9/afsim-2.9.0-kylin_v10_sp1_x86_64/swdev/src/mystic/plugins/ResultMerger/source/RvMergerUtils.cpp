// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvMergerUtils.hpp"

#include "RvEventPipeClassesRegister.hpp"
#include "RvEventPipeUtil.hpp"
#include "UtPackReflect.hpp"
#include "UtTextDocument.hpp"
#include "Utml.hpp"

static bool CompareFields(const UtPackField& aLeft, const UtPackField& aRight);
static bool CompareStructs(const UtPackStruct* aLeft, const UtPackStruct* aRight);
static bool CompareUnions(const UtPackUnion* aLeft, const UtPackUnion* aRight);
static bool CompareBasicTypes(const UtPackBasicType* aLeft, const UtPackBasicType* aRight);
static bool CompareEnums(const UtPackEnumType* aLeft, const UtPackEnumType* aRight);

static bool CompareTypes(const UtPackType* aLeft, const UtPackType* aRight)
{
   if (aLeft && aRight)
   {
      if (aLeft->GetTypeName() != aRight->GetTypeName())
      {
         return false;
      }
      else if (aLeft->GetEncoding() != aRight->GetEncoding())
      {
         return false;
      }
      for (const auto& it : aLeft->GetNestedTypes())
      {
         if (!CompareTypes(it.second.get(), aRight->FindType(it.second->GetTypeName())))
         {
            return false;
         }
      }

      return CompareStructs(dynamic_cast<const UtPackStruct*>(aLeft), dynamic_cast<const UtPackStruct*>(aRight)) &&
             CompareUnions(dynamic_cast<const UtPackUnion*>(aLeft), dynamic_cast<const UtPackUnion*>(aRight)) &&
             CompareBasicTypes(dynamic_cast<const UtPackBasicType*>(aLeft), dynamic_cast<const UtPackBasicType*>(aRight)) &&
             CompareEnums(dynamic_cast<const UtPackEnumType*>(aLeft), dynamic_cast<const UtPackEnumType*>(aRight));
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

static bool CompareFields(const UtPackField& aLeft, const UtPackField& aRight)
{
   return aLeft.GetFieldName() == aRight.GetFieldName() && aLeft.GetDefaultValue() == aRight.GetDefaultValue() &&
          aLeft.GetOptionIndex() == aRight.GetOptionIndex() && aLeft.GetBitIndex() == aRight.GetBitIndex() &&
          aLeft.IsIndex() == aRight.IsIndex() && aLeft.IsTime() == aRight.IsTime();
}

static bool CompareStructs(const UtPackStruct* aLeft, const UtPackStruct* aRight)
{
   if (aLeft && aRight)
   {
      if (aLeft->GetFields().size() != aRight->GetFields().size())
      {
         return false;
      }
      else if (!aLeft->GetBaseType().IsNull() && !aRight->GetBaseType().IsNull())
      {
         if (aLeft->GetBaseType().GetType()->GetTypeName() != aRight->GetBaseType().GetType()->GetTypeName())
         {
            return false;
         }
      }
      else if (!aLeft->GetBaseType().IsNull() || !aRight->GetBaseType().IsNull())
      {
         return false;
      }

      const size_t count = aLeft->GetFields().size();
      for (size_t i = 0; i < count; i++)
      {
         if (!CompareFields(aLeft->GetFields()[i], aRight->GetFields()[i]))
         {
            return false;
         }
      }
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

static bool CompareUnions(const UtPackUnion* aLeft, const UtPackUnion* aRight)
{
   if (aLeft && aRight)
   {
      if (aLeft->GetFields().size() != aRight->GetFields().size())
      {
         return false;
      }

      const size_t count = aLeft->GetFields().size();
      for (size_t i = 0; i < count; i++)
      {
         if (!CompareFields(aLeft->GetFields()[i], aRight->GetFields()[i]))
         {
            return false;
         }
      }
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

static bool CompareBasicTypes(const UtPackBasicType* aLeft, const UtPackBasicType* aRight)
{
   if (aLeft && aRight)
   {
      return aLeft->GetCPP_Type() == aRight->GetCPP_Type() && aLeft->GetSize() == aRight->GetSize();
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

static bool CompareEnums(const UtPackEnumType* aLeft, const UtPackEnumType* aRight)
{
   if (aLeft && aRight)
   {
      if (aLeft->GetUnderlyingType() != aRight->GetUnderlyingType())
      {
         return false;
      }

      if (aLeft->GetEnumValues().size() != aRight->GetEnumValues().size())
      {
         return false;
      }

      for (const auto& it : aLeft->GetEnumValues())
      {
         auto jt = aRight->GetEnumValues().find(it.first);
         if (jt == aRight->GetEnumValues().end())
         {
            return false;
         }
         else if (it != *jt)
         {
            return false;
         }
      }
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

static bool CompareLists(const UtPackListType* aLeft, const UtPackListType* aRight)
{
   if (aLeft && aRight)
   {
      if (aLeft->GetTypeName() != aRight->GetTypeName())
      {
         return false;
      }
      else if (aLeft->GetEncoding() != aRight->GetEncoding())
      {
         return false;
      }
      for (const auto& it : aLeft->GetNestedTypes())
      {
         if (!CompareTypes(it.second.get(), aRight->FindType(it.second->GetTypeName())))
         {
            return false;
         }
      }

      return CompareStructs(dynamic_cast<const UtPackStruct*>(aLeft), dynamic_cast<const UtPackStruct*>(aRight)) &&
             CompareUnions(dynamic_cast<const UtPackUnion*>(aLeft), dynamic_cast<const UtPackUnion*>(aRight)) &&
             CompareBasicTypes(dynamic_cast<const UtPackBasicType*>(aLeft), dynamic_cast<const UtPackBasicType*>(aRight)) &&
             CompareEnums(dynamic_cast<const UtPackEnumType*>(aLeft), dynamic_cast<const UtPackEnumType*>(aRight)) &&
             CompareLists(dynamic_cast<const UtPackListType*>(aLeft), dynamic_cast<const UtPackListType*>(aRight));
   }
   else if (aLeft || aRight)
   {
      return false;
   }
   return true;
}

//! Reads a single message from a recording.
static std::unique_ptr<rv::MsgBase> ReadMessage(std::istream& aInputStream, UtPackSerializer& aSerializer)
{
   int msgSize;
   aInputStream.read(reinterpret_cast<char*>(&msgSize), sizeof(int));
   if (aInputStream)
   {
      UtPackStream stream;
      UtBuffer&    buf = stream.mBuffer;

      buf.Grow(msgSize);
      buf.SetPutPos(msgSize); // Reading fails if putPos < getPos
      aInputStream.read(buf.GetBuffer(), msgSize);

      buf.SetGetPos(0);
      int msgId;
      buf.Get(msgId);

      UtPackSerializer::MessageInfo* msgInfo = aSerializer.GetMessageInfo(msgId);

      if (aInputStream && msgInfo != nullptr)
      {
         std::unique_ptr<UtPackMessage> msg{msgInfo->mMessageFactory()};
         msgInfo->mSerializeFunction->Read(stream, msg.get());

         // "dynamic_cast" with std::unique_ptr's
         auto* der = dynamic_cast<rv::MsgBase*>(msg.get());
         if (der != nullptr)
         {
            msg.release();
            return std::unique_ptr<rv::MsgBase>{der};
         }
      }
   }

   return nullptr;
}

void RvMerger::InitializeSerializer(const std::string& aFileName, UtPackSerializer& aSerializer)
{
   std::ifstream file;
   file.open(aFileName, std::ios::binary);
   if (!file.is_open())
   {
      throw std::runtime_error("File could not be opened: " + aFileName);
   }
   char magic[11];
   file.read(magic, 11);
   if (std::memcmp(magic, "\0\0WSF_PIPE\n", 11) != 0)
   {
      throw std::runtime_error("File does not contain a valid AER header: " + aFileName);
   }

   std::string aerText;
   std::getline(file, aerText, '\0');

   UtTextDocument aerDoc;
   aerDoc.Insert(0, aerText); // create a document that contains the read schema text
   UtmlParser aerParser(aerDoc);
   UtmlObject aerData = UtmlObject::makeContainer(std::string());
   aerParser.Parse(aerData); // parse the schema-document into a schema mark-up object
   UtPackSchema aerSchema;
   aerSchema.Read(aerData); // read the schema object into a schema
   aerSchema.Resolve();

   UtTextDocument localDoc;
   rv::GetSchema(localDoc);
   UtmlParser localParser(localDoc);
   UtmlObject localData = UtmlObject::makeContainer(std::string());
   localParser.Parse(localData);
   UtPackSchema localSchema;
   localSchema.Read(localData);
   localSchema.Resolve();

   for (auto&& it : aerSchema.GetTypeNames())
   {
      const UtPackType* aerType   = aerSchema.FindType(it);
      const UtPackType* localType = localSchema.FindType(it);
      if (!CompareTypes(aerType, localType))
      {
         throw std::runtime_error("The reference recording's schema does not match the application schema.");
      }
   }

   rv::RegisterSerializer(aSerializer);

   aSerializer.Initialize(aerSchema);
}

// Recording File Reader Methods
namespace RvMerger
{
RecordingFileReader::RecordingFileReader(const std::string& aFileName, UtPackSerializer& aSerializer)
   : mFileName(aFileName)
   , mSerializer(aSerializer)
   , mTimeOffset(0)
{
   mFileStream.open(aFileName, std::ios::binary);
   if (!mFileStream.is_open())
   {
      throw std::runtime_error("File could not be opened: " + aFileName);
   }
   char magic[11];
   mFileStream.read(magic, 11);
   if (std::memcmp(magic, "\0\0WSF_PIPE\n", 11) != 0)
   {
      throw std::runtime_error("File does not contain a valid AER header: " + aFileName);
   }
   std::getline(mFileStream, mAerSchema, '\0');

   mBeginningOfMessageData = mFileStream.tellg();

   DetermineUniqueId();

   Restart();
}

const std::string& RecordingFileReader::GetFileName() const noexcept
{
   return mFileName;
}

rv::ApplicationUniqueId RecordingFileReader::GetUniqueId() const noexcept
{
   return mUniqueId;
}

double RecordingFileReader::DetermineTimeOffset(rv::ApplicationUniqueId aReferenceId)
{
   Restart();

   while (mFileStream)
   {
      auto msg = ReadMessage(mFileStream, mSerializer);
      if ((msg != nullptr) && (msg->GetMessageId() == rv::MsgXioHeartbeat::cMESSAGE_ID))
      {
         // "dynamic_cast" of std::unique_ptr
         auto* heartbeat = dynamic_cast<const rv::MsgXioHeartbeat*>(msg.get());
         if (heartbeat != nullptr)
         {
            if (heartbeat->applicationId() == aReferenceId)
            {
               mTimeOffset = heartbeat->timeStamp() - heartbeat->simTime();
               return mTimeOffset;
            }
         }
      }
   }

   throw std::runtime_error(
      "Could not detect reference heartbeat. DIS and XIO must be enabled in all scenarios to merge.\nFile: " + mFileName);
}

double RecordingFileReader::GetTimeOffset() const noexcept
{
   return mTimeOffset;
}

std::unique_ptr<rv::MsgBase> RecordingFileReader::TakeNext() noexcept
{
   auto retval = std::move(mNextMessage);

   if (retval != nullptr)
   {
      mNextMessage = ReadMessage(mFileStream, mSerializer);
   }

   return retval;
}

const rv::MsgBase* RecordingFileReader::PeekNext() const noexcept
{
   return mNextMessage.get();
}

bool RecordingFileReader::CompareSchema(const RecordingFileReader& aReferenceRecording) const noexcept
{
   return mAerSchema == aReferenceRecording.GetSchema();
}

const std::string& RecordingFileReader::GetSchema() const noexcept
{
   return mAerSchema;
}

void RecordingFileReader::Restart() noexcept
{
   mFileStream.clear();
   mFileStream.seekg(mBeginningOfMessageData);
   mNextMessage = ReadMessage(mFileStream, mSerializer);
}

void RvMerger::RecordingFileReader::DetermineUniqueId()
{
   while (mFileStream)
   {
      auto msg = ReadMessage(mFileStream, mSerializer);
      if ((msg != nullptr) && (msg->GetMessageId() == rv::MsgXioHeartbeat::cMESSAGE_ID) && (msg->simIndex() == 0))
      {
         const auto& heartbeat = dynamic_cast<rv::MsgXioHeartbeat&>(*msg);
         mUniqueId             = heartbeat.applicationId();
         return;
      }
   }

   throw std::runtime_error(
      "Could not determine Application ID. DIS and XIO must be enabled in all scenarios to merge.\nFile: " + mFileName);
}
} // namespace RvMerger

// Recording File Writer Methods
namespace RvMerger
{
RecordingFileWriter::RecordingFileWriter(const std::string& aFileName,
                                         const std::string& aAerSchema,
                                         UtPackSerializer&  aSerializer)
   : mFileName(aFileName)
   , mSerializer(aSerializer)
{
   mFileStream.open(mFileName, std::ios::binary);
   if (!mFileStream.is_open())
   {
      throw std::runtime_error("Output file could not be opened: " + mFileName);
   }

   mFileStream.write("\0\0WSF_PIPE\n", 11); // Magic 11
   mFileStream << aAerSchema;
   mFileStream.put('\0');
}

void RecordingFileWriter::Write(const rv::MsgBase& aMessage)
{
   // Add single message to stream
   const int                      msgId   = aMessage.GetMessageId();
   UtPackSerializer::MessageInfo* msgInfo = mSerializer.GetMessageInfo(msgId);

   if (msgInfo != nullptr)
   {
      UtPackStream stream;
      UtBuffer&    buf = stream.mBuffer;

      buf.SetPutPos(4);
      buf.Put(msgId);
      msgInfo->mSerializeFunction->Write(stream, (void*)&aMessage);

      size_t msgSize = buf.GetPutPos();
      buf.SetPutPos(0);
      buf.Put(static_cast<int>(msgSize) - 4);

      mFileStream.write(buf.GetBuffer(), msgSize);
   }
}
} // namespace RvMerger

// Platform Database Methods
namespace RvMerger
{
PlatformDatabase::PlatformDatabase(size_t aSimCount)
   : mCounter(0U)
{
   mData.resize(aSimCount);
}

unsigned int PlatformDatabase::Add(size_t aSimIndex, unsigned int aPlatformIndex, rv::DisEntityId aDisIndex)
{
   auto& simData = mData.at(aSimIndex);

   // Check if platform already exists
   auto it = simData.find(aPlatformIndex);
   if (it != simData.end())
   {
      return it->second;
   }

   // Check if DIS Index already exists
   auto it2 = mDisIds.find(aDisIndex);
   if (it2 != mDisIds.end())
   {
      simData[aPlatformIndex] = it2->second;
      return it2->second;
   }

   // Add platform
   mCounter++;
   simData[aPlatformIndex] = mCounter;
   mDisIds[aDisIndex]      = mCounter;
   return mCounter;
}

unsigned int PlatformDatabase::Add(size_t aSimIndex, unsigned int aPlatformIndex)
{
   auto& simData = mData.at(aSimIndex);

   // Check if platform already exists
   auto it = simData.find(aPlatformIndex);
   if (it != simData.end())
   {
      return it->second;
   }

   // Add platform
   mCounter++;
   simData[aPlatformIndex] = mCounter;
   return mCounter;
}

unsigned int PlatformDatabase::Find(size_t aSimIndex, unsigned int aPlatformIndex) const
{
   const auto& simData = mData.at(aSimIndex);

   // Check if platform exists
   auto it = simData.find(aPlatformIndex);
   if (it != simData.end())
   {
      return it->second;
   }

   // Return default
   return 0;
}
} // namespace RvMerger
