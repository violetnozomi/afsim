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

#include "DisSignal.hpp"

#include <cassert>
#include <cstring> // for memcpy
#include <iomanip>
#include <iosfwd>
#include <iostream>

#include "Dis.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtLog.hpp"

DisSignal::DisSignal()
   : DisPdu()
   , mEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRadioId(0)
   , mEncodingScheme(0)
   , mTDLType(0)
   , mSampleRate(0)
   , mDataLength(0)
   , mSampleCount(0)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)

{
   SetPduType(DisEnum::Pdu::Type::Signal);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(32); // 256 bits
}

DisSignal::DisSignal(const DisSignal& aSrc)
   : DisPdu(aSrc)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   Copy(aSrc);
}

DisSignal::DisSignal(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   GetMemberData(aGenI);
}

DisSignal::DisSignal(GenI& aGenI)
   : DisPdu(aGenI)
   , mDataPtr(nullptr)
   , mDataIsClassOwned(false)
{
   GetMemberData(aGenI);
}

// virtual
DisSignal::~DisSignal()
{
   DeleteClassData();
}

// virtual
DisPdu* DisSignal::Clone() const
{
   return new DisSignal(*this);
}

// virtual
const DisEntityId& DisSignal::GetOriginatingEntity() const
{
   return mEntityId;
}

// virtual
int DisSignal::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Signal;
}

/*
//virtual
const DisEntityId& DisSignal::GetOriginatingEntity() const
{
   return mEntityId;
}
*/

// Copy the data portion from one DisSignal to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisSignal::Copy(const DisSignal& aSrc)
{
   mEntityId         = aSrc.mEntityId;
   mRadioId          = aSrc.mRadioId;
   mEncodingScheme   = aSrc.mEncodingScheme;
   mTDLType          = aSrc.mTDLType;
   mSampleRate       = aSrc.mSampleRate;
   mDataLength       = aSrc.mDataLength;
   mSampleCount      = aSrc.mSampleCount;
   mDataPtr          = nullptr;
   mDataIsClassOwned = false;

   if (aSrc.mDataLength != 0)
   {
      if (aSrc.mDataPtr != nullptr)
      {
         mDataIsClassOwned    = true;
         DisUint16 dataLength = (aSrc.mDataLength + 7) / 8;
         mDataPtr             = new unsigned char[dataLength];
         memcpy(mDataPtr, aSrc.mDataPtr, dataLength);
      }
      else if (Dis::ShowError(Dis::cIO_ERRORS))
      {
         ut::log::error() << "DisSignal::Copy was passed a signal pdu with a invalid data pointer.";
         assert(aSrc.mDataPtr != nullptr);
      }
   }
}

// DeleteClassData deletes the data portion if the memory
// pointed to by the data pointer is owned by the class.

// private
void DisSignal::DeleteClassData()
{
   if (mDataPtr != nullptr)
   {
      if (mDataIsClassOwned)
      {
         delete[] mDataPtr;
      }
      mDataPtr = nullptr;
   }
   mDataLength       = 0;
   mDataIsClassOwned = false;
}

// GetData returns a pointer to the data portion and the number
// of BITS in the data portion.  The caller can use a GenMemIO
// object to interpret the data in a platform independent manner.

void DisSignal::GetData(const unsigned char*& aDataPtr, DisUint16& aDataLength) const
{
   aDataPtr    = mDataPtr;
   aDataLength = mDataLength;
}

// SetData allows the caller to replace the data portion of the
// PDU.  The memory pointed to the supplied pointer becomes the
// property of the object and will be deleted when the objected
// is deleted or if the pointer is later replaced.

void DisSignal::SetData(unsigned char* aDataPtr, DisUint16 aDataLength)
{
   SetTheData(aDataPtr, aDataLength);
   mDataIsClassOwned = true; // Data owned by the class
}

void DisSignal::SetUserData(unsigned char* aDataPtr, DisUint16 aDataLength)
{
   SetTheData(aDataPtr, aDataLength);
   mDataIsClassOwned = false; // Data owned by the user
}

// virtual
void DisSignal::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisSignal::GetMemberData(aGenI);
}

// virtual
DisUint16 DisSignal::GetLength()
{
   DisUint16 dataLength = ((mDataLength + 31) / 32) * 4;
   DisUint16 octetCount = 32 + dataLength;
   SetLength(octetCount);
   return octetCount;
}

// private
void DisSignal::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DeleteClassData(); // Release existing class-owned data

   aGenI >> mEntityId;
   aGenI >> mRadioId;
   aGenI >> mEncodingScheme;
   aGenI >> mTDLType;
   aGenI >> mSampleRate;
   aGenI >> mDataLength;
   aGenI >> mSampleCount;

   // The last word of the data portion is supposed to be padded with zeros.
   // We will allocate and read this amount and the 'Data Length' will
   // represent how much is actually usable.

   DisUint16 dataLength = ((mDataLength + 31) / 32) * 4;
   if (dataLength != 0)
   {
      mDataIsClassOwned = true; // Data is owned by the class
      mDataPtr          = new unsigned char[dataLength];
      assert(mDataPtr != nullptr);
      aGenI.GetArray(mDataPtr, dataLength);
   }

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// Private
void DisSignal::SetTheData(unsigned char* aDataPtr, DisUint16 aDataLength)
{
   // You can't have a non-zero length if you don't provide data or
   // no data with length
   assert((aDataPtr && aDataLength) || !(aDataPtr || aDataLength));

   DeleteClassData(); // Release existing class-owned data

   mDataPtr    = aDataPtr;
   mDataLength = aDataLength;

   GetLength(); // Force length to be updated
}

// virtual
void DisSignal::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mEntityId;
   aGenO << mRadioId;
   aGenO << mEncodingScheme;
   aGenO << mTDLType;
   aGenO << mSampleRate;
   aGenO << mDataLength;
   aGenO << mSampleCount;

   // When sending the data part we don't assume that the array has been
   // padded to a multiple of 4 bytes.  We reference only what is declared
   // by the data length and then send out explicit padding.

   DisUint16 dataLength = (mDataLength + 7) / 8;
   if (dataLength != 0)
   {
      aGenO.PutArray(mDataPtr, dataLength);
      // Send out any bytes that are needed to pad to a full word.
      DisUint8 pad8 = 0;
      while ((dataLength & 0x03) != 0)
      {
         aGenO << pad8;
         ++dataLength;
      }
   }
}

void DisSignal::Stream(std::ostream& aStream) const
{
   aStream << ToString();
}

std::string DisSignal::ToString() const
{
   using namespace std;
   std::ostringstream ss("");
   enum
   {
      EncodedAudio,
      RawBinaryData,
      ApplicationSpecificData,
      DatabaseIndex
   };
   static const string ENCODING_CLASS[] = {" (Encoded audio)",
                                           " (Raw Binary Data)",
                                           " (Application-Specific Data)",
                                           " (Database index)"};


   const unsigned char* data;
   DisUint16            dataLength;
   GetData(data, dataLength);

   ss << setiosflags(ios::fixed) << DisPdu::ToString() << '\n'
      << "Orig. Entity:           " << GetOriginatingEntity() << '\n'
      << "Entity Id:              " << GetEntityId() << '\n'
      << "Radio Id:               " << GetRadioId() << '\n';
   DisUint16 ec = (GetEncodingScheme() >> 14) & 0x0003;
   DisUint16 et = (GetEncodingScheme() & 0x3FFF);
   ss << "Encoding Class:         " << ec << ENCODING_CLASS[ec] << '\n';
   if (ec == EncodedAudio)
   {
      ss << "Encoding Type:          " << et;
      switch (et)
      {
      case 1:
         ss << "   8-bit mu-law (ITU-T G.711)\n";
         break;
      case 2:
         ss << "   CVSD (MIL-STD-188-113)\n";
         break;
      case 3:
         ss << "   ADPCM (ITU-T G.726)\n";
         break;
      case 4:
         ss << "   16-bit Linear PCM 2's complement, Big Endian\n";
         break;
      case 5:
         ss << "   8-bit Linear PCM, unsigned\n";
         break;
      case 6:
         ss << "   VQ (Vector Quantization) (deprecated)\n";
         break;
      case 7:
         ss << "   (unavailable for use)\n";
         break;
      case 8:
         ss << "   GSM Full-Rate (ETSI 06.10)\n";
         break;
      case 9:
         ss << "   GSM Half-Rate (ETSI 06.20)\n";
         break;
      case 10:
         ss << "   Speex Narrow Band\n";
         break;
      case 100:
         ss << "   16-bit Linear PCM 2's complement, Little Endian\n";
         break;
      case 255:
         ss << "   (unavailable for use)\n";
         break;
      default:
         ss << "   UNKNOWN\n";
         break;
      }
   }
   else
   {
      if (GetTDLType() != 0)
      {
         ss << "Number of TDL Messages: " << et << '\n';
      }
   }
   locale tmp = ss.getloc();
   ss.imbue(locale(""));
   ss << "TDL Type:               " << GetTDLType() << '\n' << "Sample Rate:            " << GetSampleRate();
   switch (ec)
   {
   case EncodedAudio:
      ss << " (samples per second)\n";
      break;
   case RawBinaryData:
   case ApplicationSpecificData:
      ss << " (bps)\n";
      break;
   default:
      ss << '\n';
      break;
   }
   ss << "Sample Count:           " << GetSampleCount() << '\n'
      << "Data Length:            " << dataLength << " bits\n";
   ss << "Data:";
   unsigned int bytes = (static_cast<unsigned int>(dataLength) + 7) / 8;
   for (unsigned int i = 0; i < bytes; ++i)
   {
      if ((i % 8) == 0)
      {
         ss << "\n  0x";
      }
      else if ((i % 4) == 0)
      {
         ss << ", 0x";
      }
      else
      {
         ss << " ";
      }
      ss << hex << setfill('0') << setw(2) << right << static_cast<unsigned int>(data[i]);
   }
   ss << dec << setfill(' ') << '\n';
   ss.imbue(tmp);
   return ss.str();
}

DisSignal& DisSignal::operator=(const DisSignal& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      DeleteClassData();
      Copy(aRhs);
   }
   return *this;
}

unsigned int DisSignal::IsValid()
{
   if (!DisPdu::IsValid())
   {
      return 0;
   }

   DisUint32 total_bits = GetLength() * 8;
   if (total_bits % 32 > 0)
   {
      auto out = ut::log::error() << "DisSignal::IsValid(): PDU does not have a length on a 32-bit boundary.";
      out.AddNote() << "PDU Sent From: " << GetEntityId() << ":" << GetRadioId();

      return 0;
   }

   if (((mEncodingScheme >> 14) & 0x0003) == 3 && mDataLength != 96)
   {
      auto out = ut::log::error() << "Data Length field must be 96 if the Encoding Class is Database index.";
      out.AddNote() << "Encoding: " << std::hex << mEncodingScheme;
      out.AddNote() << "PDU Sent From: " << GetEntityId() << ":" << GetRadioId();
      return 0;
   }

   DisUint32 data_bits = 256 + mDataLength;
   if ((data_bits > total_bits) || ((total_bits - data_bits) > 31))
   {
      auto out = ut::log::error() << "Data Length field does not match the number of bits specified in the PDU Header.";
      out.AddNote() << "Data Length: " << mDataLength;
      out.AddNote() << "Specified Bits: " << total_bits;
      out.AddNote() << "Total PDU Size: 256 + Data Length + (0-31 padding) bits = " << data_bits << " bits";
      out.AddNote() << "PDU Sent From: " << GetEntityId() << ":" << GetRadioId();
      return 0;
   }
   return 1;
}

bool DisSignal::IsValid() const
{
   bool isValid = true;
   if (!DisPdu::IsValid())
   {
      isValid = false;
   }
   if (!mEntityId.IsValid())
   {
      isValid = false;
   }
   return isValid;
}
