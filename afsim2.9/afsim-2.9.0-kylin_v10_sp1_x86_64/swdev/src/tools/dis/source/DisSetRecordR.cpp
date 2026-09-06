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

#include "DisSetRecordR.hpp"

#include <cassert>
#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each) +
//        RequestId (4 bytes) +
//        (Reliability (1 byte) + byte padding(7 bytes)) +
//        NumRecords (4 bytes) +
//        <N Records> (variable)
const DisUint16 sDisSetRecordRSize = (48 + 48 + 32 + (8 + 8 + 16 + 32) + 32) / 8;
} // namespace

DisSetRecordR::DisSetRecordR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mNumSpecRecords(0)
   , mRecords()
{
   SetPduType(DisEnum::Pdu::Type::SetRecordR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisSetRecordR::DisSetRecordR(const DisSetRecordR& aSrc)
   : DisPdu(aSrc)
   , mRecords()
{
   Copy(aSrc);
}

DisSetRecordR::DisSetRecordR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mRecords()
{
   GetMemberData(aGenI);
}

DisSetRecordR::DisSetRecordR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisSetRecordR::~DisSetRecordR() {}

// virtual
DisPdu* DisSetRecordR::Clone() const
{
   return new DisSetRecordR(*this);
}

const DisSpecRecord& DisSetRecordR::GetSpecRecord(unsigned int aIndex) const
{
   return mRecords[aIndex];
}

const std::vector<DisSpecRecord>& DisSetRecordR::GetSpecRecords() const
{
   return mRecords;
}

// virtual
DisUint32 DisSetRecordR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisSetRecordR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
int DisSetRecordR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::SetRecordR;
}

// virtual
void DisSetRecordR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisSetRecordR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

void DisSetRecordR::AddSpecRecord(const DisSpecRecord& aSpecRecord)
{
   mRecords.push_back(aSpecRecord);
   // Update the length
   GetLength();
   mNumSpecRecords++;
}

// Copy the data portion from one DisSetRecordR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisSetRecordR::Copy(const DisSetRecordR& aSrc)
{
   mOriginatingEntity  = aSrc.mOriginatingEntity;
   mReceivingEntity    = aSrc.mReceivingEntity;
   mRequestId          = aSrc.mRequestId;
   mReliabilityService = aSrc.mReliabilityService;
   mNumSpecRecords     = aSrc.mNumSpecRecords;

   if (mNumSpecRecords != mRecords.size())
   {
      mRecords.resize(mNumSpecRecords);
   }
   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mRecords[i] = aSrc.mRecords[i];
   }
}

// virtual
void DisSetRecordR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisSetRecordR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisSetRecordRSize;
   for (unsigned int i = 0; i != mRecords.size(); ++i)
   {
      byteCount += mRecords[i].GetLengthOfSet();
   }
   SetLength(byteCount);
   return byteCount;
}

// private
void DisSetRecordR::GetMemberData(GenI& aGenI)
{
   DisUint8 pad8 = 0;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mReliabilityService;
   aGenI >> pad8;
   aGenI >> pad8 >> pad8;
   aGenI >> pad8 >> pad8 >> pad8 >> pad8;
   aGenI >> mNumSpecRecords;

   if (mNumSpecRecords != mRecords.size())
   {
      mRecords.resize(mNumSpecRecords);
   }
   for (unsigned int i = 0; i != mRecords.size(); ++i)
   {
      mRecords[i].Get(aGenI);
   }
}

// virtual
void DisSetRecordR::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date

   DisUint8 pad8 = 0;
   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mReliabilityService;
   aGenO << pad8;                         // 8 bit padding
   aGenO << pad8 << pad8;                 // 16 bit padding
   aGenO << pad8 << pad8 << pad8 << pad8; // 32 bit padding
   aGenO << mNumSpecRecords;

   for (unsigned int i = 0; i != mRecords.size(); ++i)
   {
      mRecords[i].Put(aGenO);
   }
}

DisSetRecordR& DisSetRecordR::operator=(const DisSetRecordR& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisSetRecordR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisSetRecordR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisSetRecordR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisSetRecordR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisSetRecordR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId   = " << static_cast<int>(mRequestId) << std::endl;
   aStream << "      => Num Records = " << static_cast<int>(mNumSpecRecords) << std::endl;
   for (unsigned int i = 0; i != mNumSpecRecords; ++i)
   {
      mRecords[i].Stream(aStream);
   }
   aStream << "      === End DisSetRecordR ===" << std::endl << std::endl;
}
