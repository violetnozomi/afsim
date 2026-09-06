// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisIntercomSignal.hpp"

#include <cassert>
#include <cstring> // for memcpy
#include <iostream>

#include "Dis.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::hex;

DisIntercomSignal::DisIntercomSignal()
   : DisPdu()
   , mEncodingScheme()
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::IntercomSignal);
   SetProtocolFamily(DisEnum::Pdu::Family::RadioCommunication);
   SetLength(DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS);
}

DisIntercomSignal::DisIntercomSignal(const DisIntercomSignal& aSrc)
   : DisPdu(aSrc)
   , mIntercomReferenceId(aSrc.mIntercomReferenceId)
   , mIntercomNumber(aSrc.mIntercomNumber)
   , mEncodingScheme(aSrc.mEncodingScheme)
   , mTDLType(aSrc.mTDLType)
   , mSampleRate(aSrc.mSampleRate)
   , mDataLength(aSrc.mDataLength)
   , mSampleCount(aSrc.mSampleCount)
   , mData(aSrc.mData)
{
   GetLength();
   Copy(aSrc);
}

DisIntercomSignal::DisIntercomSignal(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mEncodingScheme()
   , mData()
{
   GetMemberData(aGenI);
}

DisIntercomSignal::DisIntercomSignal(GenI& aGenI)
   : DisPdu(aGenI)
   , mEncodingScheme()
   , mData()
{
   GetMemberData(aGenI);
}

DisIntercomSignal* DisIntercomSignal::Clone() const
{
   return new DisIntercomSignal(*this);
}

int DisIntercomSignal::GetClass() const
{
   return DisEnum::Pdu::Type::IntercomSignal;
}

int DisIntercomSignal::GetFamily() const
{
   return DisEnum::Pdu::Family::RadioCommunication;
}

const DisEntityId& DisIntercomSignal::GetOriginatingEntity() const
{
   return mIntercomReferenceId;
}

const DisEntityId& DisIntercomSignal::GetIntercomReferenceId() const
{
   return mIntercomReferenceId;
}

DisUint16 DisIntercomSignal::GetIntercomNumber() const
{
   return mIntercomNumber;
}

const DisEncodingSchemeRecord& DisIntercomSignal::GetEncodingScheme() const
{
   return mEncodingScheme;
}

DisEnum16 DisIntercomSignal::GetTDLType() const
{
   return mTDLType;
}

DisUint32 DisIntercomSignal::GetSampleRate() const
{
   return mSampleRate;
}

DisUint16 DisIntercomSignal::GetSampleCount() const
{
   return mSampleCount;
}

void DisIntercomSignal::GetData(std::vector<unsigned char>& aData, DisUint16& aDataLength) const
{
   aData       = mData;
   aDataLength = mDataLength;
}

void DisIntercomSignal::GetData(std::vector<unsigned char>& aData, DisUint16& aDataLength)
{
   aData       = mData;
   aDataLength = mDataLength;
}

std::vector<unsigned char> DisIntercomSignal::GetData() const
{
   return mData;
}

void DisIntercomSignal::SetIntercomReferenceId(const DisEntityId& aIntercomReferenceId)
{
   mIntercomReferenceId = aIntercomReferenceId;
}

void DisIntercomSignal::SetIntercomNumber(DisUint16 aIntercomNumber)
{
   mIntercomNumber = aIntercomNumber;
}

void DisIntercomSignal::SetEncodingScheme(const DisEncodingSchemeRecord& aEncodingScheme)
{
   mEncodingScheme = aEncodingScheme;
}

void DisIntercomSignal::SetTDLType(DisEnum16 aTDLType)
{
   mTDLType = aTDLType;
}

void DisIntercomSignal::SetSampleRate(DisUint32 aSampleRate)
{
   mSampleRate = aSampleRate;
}

void DisIntercomSignal::SetSampleCount(DisUint16 aSampleCount)
{
   mSampleCount = aSampleCount;
}

std::string DisIntercomSignal::GetStringId() const
{
   return "DisIntercomSignal";
}

std::string DisIntercomSignal::ToString() const
{
   std::string returnString = mIntercomReferenceId.ToString() + '\n';
   returnString += "Intercom Number: " + std::to_string(mIntercomNumber) + '\n';
   returnString += "Encoding Scheme: " + mEncodingScheme.ToString() + '\n';
   returnString += "TDL type: " + std::to_string(mTDLType) + '\n';
   returnString += "Sample Rate: " + std::to_string(mSampleRate) + '\n';
   returnString += "Data:        ";
   std::stringstream ss;
   for (int i = 0; i < static_cast<int>(mData.size()); i++)
   {
      ss << " " << std::hex << mData[i];
   }
   returnString += ss.str() + '\n';
   return returnString;
}

void DisIntercomSignal::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIntercomSignal::GetLength()
{
   // Get the length of the PDU (header + body of child class) without any records
   DisUint16 dataLength = DisPdu::GetBaseLength() + cPDU_MIN_LENGTH_OCTETS;
   // Add the length of the data associated with this PDU
   dataLength += ((mDataLength + 31) / 32) * 4;
   SetLength(dataLength);
   return dataLength;
}

void DisIntercomSignal::Put(GenO& aGenO)
{
   GetLength();
   DisPdu::Put(aGenO);
   aGenO << mIntercomReferenceId;
   aGenO << mIntercomNumber;
   mEncodingScheme.Put(aGenO);
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
      for (int i = 0; i < static_cast<int>(mData.size()); i++)
      {
         aGenO << mData[i];
      }

      // Send out any bytes that are needed to pad to a full word.
      DisUint8 pad8 = 0;
      while ((dataLength & 0x03) != 0)
      {
         aGenO << pad8;
         ++dataLength;
      }
   }
}

unsigned int DisIntercomSignal::IsValid()
{
   if (!DisPdu::IsValid())
   {
      return 0;
   }

   DisUint32 total_bits = GetLength() * 8;
   if (total_bits % 32 > 0)
   {
      return 0;
   }

   if (mEncodingScheme.GetEncodingClass() == 3 && mDataLength != 96)
   {
      return 0;
   }

   DisUint32 data_bits = 256 + mDataLength;
   if ((data_bits > total_bits) || ((total_bits - data_bits) > 31))
   {
      return 0;
   }
   return 1;
}

bool DisIntercomSignal::IsValid() const
{
   return DisPdu::IsValid() && mIntercomReferenceId.IsValid();
}

DisIntercomSignal& DisIntercomSignal::operator=(const DisIntercomSignal& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      DeleteClassData();
      Copy(aRhs);
   }
   return *this;
}

void DisIntercomSignal::Copy(const DisIntercomSignal& aSrc)
{
   mIntercomReferenceId = aSrc.mIntercomReferenceId;
   mIntercomNumber      = aSrc.mIntercomNumber;
   mEncodingScheme      = aSrc.mEncodingScheme;
   mTDLType             = aSrc.mTDLType;
   mSampleRate          = aSrc.mSampleRate;
   mDataLength          = aSrc.mDataLength;
   mSampleCount         = aSrc.mSampleCount;
   mData                = aSrc.mData;
}

void DisIntercomSignal::DeleteClassData()
{
   mData.clear();
   mDataLength = 0;
}

bool DisIntercomSignal::SetData(std::vector<unsigned char> aData, DisUint16 aDataLength)
{
   return SetTheData(aData, aDataLength);
}

void DisIntercomSignal::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DeleteClassData();

   aGenI >> mIntercomReferenceId;
   aGenI >> mIntercomNumber;
   mEncodingScheme.Get(aGenI);
   aGenI >> mTDLType;
   aGenI >> mSampleRate;
   aGenI >> mDataLength;
   aGenI >> mSampleCount;

   DisUint16 dataLength = ((mDataLength + 31) / 32) * 4;
   if (dataLength != 0)
   {
      for (int i = 0; i < static_cast<int>(dataLength); i++)
      {
         unsigned char temp;
         aGenI >> temp;
         mData.push_back(temp);
      }
   }

   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

bool DisIntercomSignal::SetTheData(std::vector<unsigned char> aData, DisUint16 aDataLength)
{
   DisUint16 numberOfElements = ((aDataLength + 31) / 32) * 4;

   // You can't have a non-zero length if you don't provide data or
   // no data with length
   if (aData.size() != numberOfElements)
   {
      return false;
   }

   mData       = aData;
   mDataLength = aDataLength;

   GetLength(); // Force length to be updated
   return true;
}
