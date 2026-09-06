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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTPACKSTREAM_HPP
#define UTPACKSTREAM_HPP

#include <string>

class UtPackStream;
class UtPackSerializer;
class UtPackPrinter;

// Handles reading and writing of a data value
class UtPackSerializeFunctor
{
public:
   virtual ~UtPackSerializeFunctor() = default;

   virtual void Initialize(UtPackSerializer& aSerializer) {}

   // Read from the stream into memory pointed to by aDataPtr
   virtual void Read(UtPackStream& bs, void* aDataPtr) = 0;
   // Same as Read, except no data is stored.  Stream is advanced.
   virtual void SkipRead(UtPackStream& bs) = 0;

   // Write the value pointed to by aDataPtr to the stream
   virtual void Write(UtPackStream& bs, void* aDataPtr) = 0;

   // Print value as human readable text
   // should be implemented by every type
   virtual void Print(void* aDataPtr, UtPackPrinter& aStream) = 0;

   // Read value from string.  Should be implemented by
   // basic types.  Used for initialization of default values
   virtual bool FromString(void* aDataPtr, const std::string& aString) { return false; }
};

// A basic stream interface to a buffer
class WSF_UT_EXPORT UtPackStream
{
public:
   UtPackStream() { mBuffer.SetLittleEndian(); }
   void Write(const void* aDataPtr, int aBytes) { mBuffer.PutRaw((const char*)aDataPtr, aBytes); }
   void Read(void* aData, int aBytes)
   {
      assert((int)mBuffer.GetValidBytes() >= aBytes);
      mBuffer.GetRaw((char*)aData, aBytes);
   }
   void SkipRead(int aBytes)
   {
      assert((int)mBuffer.GetValidBytes() >= aBytes);
      mBuffer.SetGetPos(mBuffer.GetGetPos() + aBytes);
   }

   UtBuffer mBuffer;
};

class WSF_UT_EXPORT UtPackMessageStream
{
public:
   struct PacketHeaderInfo
   {
      // number of bytes that follow the size (total bytes - 4)
      int mSize;
      int mPacketType;
   };
   static const int cPACKET_HEADER_SIZE = sizeof(PacketHeaderInfo);

   static PacketHeaderInfo ReadPacketHeader(int aHeaderWord)
   {
      PacketHeaderInfo info = {aHeaderWord & 0x3fffffff, aHeaderWord >> 30};
      return info;
   }
   static unsigned int MakePacketHeader(int aPacketSize, int aPacketType) { return aPacketSize | (aPacketType << 30); }

   enum ReadResult
   {
      cREAD_MESSAGE,
      cSKIP_MESSAGE,
      cEND_OF_STREAM
   };

   static ReadResult ReadMessageFromIStream(UtPackSerializer& aSerializer, std::istream& aStream, UtBuffer& aBuffer);

   static void WriteMessageToOStream(const UtPackMessage& aMessage,
                                     UtPackSerializer&    aSerializer,
                                     std::ostream&        aStream,
                                     UtPackStream&        aPackStream);
};

// Abstract stream taking UtPackMessage objects, writing them to some output
class WSF_UT_EXPORT UtPackMessageStreamO : private UtPackMessageStream
{
public:
   UtPackMessageStreamO(UtPackSerializer* aSerializer);
   virtual ~UtPackMessageStreamO() = default;

   void Write(UtPackMessage& aMessage);

   virtual void Flush();

protected:
   //! Must be overridden to implement a message stream.  This is called for each
   //! block of data to be written.  A simple implementation would simply call fwrite()
   virtual void WritePacket(const void* aDataPtr, int aBytes) = 0;

private:
   // message stream holds one message at a time, with enough space left at the beginning to hold a packet header
   // WritePacket() is called with memory from this buffer, with care to avoid copies
   UtPackStream      mMessageStream;
   UtPackSerializer* mSerializer;
};

// Outputs UtPackMessages to a std::ostream (should use binary mode)
class WSF_UT_EXPORT UtPackMessageStdStreamO : public UtPackMessageStreamO
{
public:
   UtPackMessageStdStreamO(std::ostream* aStream, UtPackSerializer* aSerializer)
      : UtPackMessageStreamO(aSerializer)
   {
      mStreamPtr = aStream;
   }
   ~UtPackMessageStdStreamO() override = default;

   void Flush() override;

private:
   void WritePacket(const void* aDataPtr, int aBytes) override;

   std::ostream* mStreamPtr;
};
#endif
