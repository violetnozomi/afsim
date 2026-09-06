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

#include "DisRecordR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// DisEnum32 sSerialNumber = 0;
DisUint16 sDisRecordRSize = (48 + 48 + 32 + (8 + 8) + 16 + 32 + 32) / 8;
} // namespace

DisRecordR::DisRecordR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mEventType(0)
   , mResponseSerial(0)
   , mNumSpecRecords(0)
   , mSpecRecordSet()
{
   SetPduType(DisEnum::Pdu::Type::RecordR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisRecordR::DisRecordR(const DisRecordR& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

// virtual
void DisRecordR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
void DisRecordR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
DisUint32 DisRecordR::GetRequestId() const
{
   return mRequestId;
}

DisRecordR::DisRecordR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisRecordR::DisRecordR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisRecordR::~DisRecordR() {}

// virtual
DisPdu* DisRecordR::Clone() const
{
   return new DisRecordR(*this);
}

// virtual
int DisRecordR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RecordR;
}

const DisSpecRecord& DisRecordR::GetRecordSet(unsigned int aIndex) const
{
   return mSpecRecordSet[aIndex];
}

DisSpecRecord& DisRecordR::GetRecordSet(unsigned int aIndex)
{
   return mSpecRecordSet[aIndex];
}

// Copy the data portion from one DisRecordR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisRecordR::Copy(const DisRecordR& aSrc)
{
   mOriginatingEntity  = aSrc.mOriginatingEntity;
   mReceivingEntity    = aSrc.mReceivingEntity;
   mRequestId          = aSrc.mRequestId;
   mReliabilityService = aSrc.mReliabilityService;
   mEventType          = aSrc.mEventType;
   mResponseSerial     = aSrc.mResponseSerial;
   mNumSpecRecords     = aSrc.mNumSpecRecords;

   if (mNumSpecRecords != mSpecRecordSet.size())
   {
      mSpecRecordSet.resize(mNumSpecRecords);
   }
   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mSpecRecordSet[i] = aSrc.mSpecRecordSet[i];
   }
}

// virtual
void DisRecordR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisRecordR::GetMemberData(aGenI);
}

// virtual
DisUint16 DisRecordR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisRecordRSize;
   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      byteCount += mSpecRecordSet[i].GetLength();
   }
   SetLength(byteCount);
   return byteCount;
}

// private
void DisRecordR::GetMemberData(GenI& aGenI)
{
   DisUint8 dummy8 = 0;
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mReliabilityService;
   aGenI >> dummy8;
   aGenI >> mEventType;
   aGenI >> mResponseSerial;
   aGenI >> mNumSpecRecords;

   if (mNumSpecRecords != mSpecRecordSet.size())
   {
      mSpecRecordSet.resize(mNumSpecRecords);
   }
   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mSpecRecordSet[i].Get(aGenI);
   }
}

void DisRecordR::AddSpecRecord(const DisSpecRecord& aRecord)
{
   mSpecRecordSet.push_back(aRecord);
   mNumSpecRecords = static_cast<DisUint32>(mSpecRecordSet.size());
}

// virtual
void DisRecordR::Put(GenO& aGenO)
{
   DisUint8 dummy8 = 0;
   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mReliabilityService;
   aGenO << dummy8;
   aGenO << mEventType;
   aGenO << mResponseSerial;
   aGenO << mNumSpecRecords;

   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mSpecRecordSet[i].Put(aGenO);
   }
}

DisRecordR& DisRecordR::operator=(const DisRecordR& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisRecordR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisRecordR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisRecordR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisRecordR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisRecordR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      => RequestId           = " << mRequestId << std::endl;
   aStream << "      => Event Type          = " << mEventType << std::endl;
   aStream << "      => Resp Serial Number  = " << mResponseSerial << std::endl;
   aStream << "      => Number Spec Records = " << mNumSpecRecords << std::endl;

   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mSpecRecordSet[i].Stream(aStream);
      aStream << std::endl;
   }
   aStream << "      === End DisRecordR ===" << std::endl << std::endl;
}
