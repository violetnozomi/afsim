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

#include "DisRecordQueryR.hpp"

#include <cassert>
#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each) + RequestId (4 bytes) +
//            (Reliability (8-bits) + one byte padding) + two bytes event type +
//            four bytes Time + four bytes NumRecords + <N Records> * Record Size
const DisUint16 sDisRecordQueryRSize = (48 + 48 + 32 + (8 + 8) + 16 + 32 + 32) / 8;
const DisUint16 sRecordLength        = 32 / 8;
} // namespace

DisRecordQueryR::DisRecordQueryR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mEventType(0)
   , mTime(0)
   , mNumRecords(0)
   , mRecords()
{
   SetPduType(DisEnum::Pdu::Type::RecordQueryR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisRecordQueryR::DisRecordQueryR(const DisRecordQueryR& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

DisRecordQueryR::DisRecordQueryR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisRecordQueryR::DisRecordQueryR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisRecordQueryR::~DisRecordQueryR() {}

// virtual
DisPdu* DisRecordQueryR::Clone() const
{
   return new DisRecordQueryR(*this);
}

// virtual
DisEnum8 DisRecordQueryR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
DisUint32 DisRecordQueryR::GetRequestId() const
{
   return mRequestId;
}

// virtual
int DisRecordQueryR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RecordQueryR;
}

// virtual
void DisRecordQueryR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
void DisRecordQueryR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// Copy the data portion from one DisRecordQueryR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisRecordQueryR::Copy(const DisRecordQueryR& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;

   mRequestId          = aSrc.mRequestId;
   mReliabilityService = aSrc.mReliabilityService;
   mEventType          = aSrc.mEventType;
   mTime               = aSrc.mTime;
   mNumRecords         = aSrc.mNumRecords;

   if (mRecords.size() != mNumRecords)
   {
      mRecords.resize(mNumRecords);
   }
   for (unsigned int i = 0; i != mNumRecords; ++i)
   {
      mRecords[i] = aSrc.mRecords[i];
   }
}

// virtual
void DisRecordQueryR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisRecordQueryR::GetMemberData(aGenI);
}

DisUint32 DisRecordQueryR::GetRecord(unsigned int aIndex) const
{
   DisUint32 result = 0;
   if (aIndex < mRecords.size())
   {
      result = mRecords[aIndex];
   }
   return result;
}

// virtual
DisUint16 DisRecordQueryR::GetLength()
{
   DisUint16 byteCount =
      DisPdu::GetBaseLength() + sDisRecordQueryRSize + sRecordLength * static_cast<DisUint16>(mNumRecords);
   SetLength(byteCount);
   return byteCount;
}

// private
void DisRecordQueryR::GetMemberData(GenI& aGenI)
{
   DisUint8 dummy = 0;
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mReliabilityService;
   aGenI >> dummy; // padding
   aGenI >> mEventType;
   aGenI >> mTime;
   aGenI >> mNumRecords;

   if (mRecords.size() != mNumRecords)
   {
      mRecords.resize(mNumRecords);
   }
   for (unsigned int i = 0; i != mNumRecords; ++i)
   {
      aGenI >> mRecords[i];
   }
}

// virtual
void DisRecordQueryR::Put(GenO& aGenO)
{
   DisUint8 dummy = 0;
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mReliabilityService;
   aGenO << dummy; // padding
   aGenO << mEventType;
   aGenO << mTime;
   aGenO << mNumRecords;

   assert(mNumRecords == mRecords.size());
   for (unsigned int i = 0; i != mNumRecords; ++i)
   {
      aGenO << mRecords[i];
   }
}

DisRecordQueryR& DisRecordQueryR::operator=(const DisRecordQueryR& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisRecordQueryR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisRecordQueryR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisRecordQueryR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisRecordQueryR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisRecordQueryR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId   = " << static_cast<int>(mRequestId) << std::endl;
   aStream << "      => Event Type  = " << static_cast<int>(mEventType) << std::endl;
   aStream << "      => Time        = " << static_cast<int>(mTime) << std::endl;
   aStream << "      => Num Records = " << static_cast<int>(mNumRecords) << std::endl;
   for (unsigned int i = 0; i != mRecords.size(); ++i)
   {
      aStream << "             *** " << mRecords[i] << std::endl;
   }
   aStream << "      === End DisRecordQueryR ===" << std::endl << std::endl;
}
