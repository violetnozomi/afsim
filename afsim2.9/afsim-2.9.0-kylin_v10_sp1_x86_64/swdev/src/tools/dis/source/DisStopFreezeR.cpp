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

#include "DisStopFreezeR.hpp"

#include <iostream>

#include "DisControlEnums.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "math.h"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RealWorlTime, RequestId, Reason, Frozen, Reliability, PadByte).
const DisUint16 sDisStopFreezeRSize = (48 + 48 + 64 + 32 + 8 + 8 + 8 + 8) / 8;
} // namespace

DisStopFreezeR::DisStopFreezeR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRealWorldTime()
   , mRequestId(0)
   , mReason(0)
   , mFrozenBehavior(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::StopFreezeR);
   SetLength(DisPdu::GetBaseLength() + sDisStopFreezeRSize);
}

DisStopFreezeR::DisStopFreezeR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisStopFreezeR::DisStopFreezeR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisStopFreezeR::~DisStopFreezeR() {}

// virtual
DisPdu* DisStopFreezeR::Clone() const
{
   return new DisStopFreezeR(*this);
}

// virtual
int DisStopFreezeR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::StopFreezeR;
}

void DisStopFreezeR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisStopFreezeR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint8 pad8;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRealWorldTime;
   aGenI >> mReason;
   aGenI >> mFrozenBehavior;
   aGenI >> mReliabilityService;
   aGenI >> pad8;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisStopFreezeR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   DisUint8 pad8(0);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRealWorldTime;
   aGenO << mReason;
   aGenO << mFrozenBehavior;
   aGenO << mReliabilityService;
   aGenO << pad8;
   aGenO << mRequestId;
}

// virtual
void DisStopFreezeR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisStopFreezeR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisStopFreezeR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisStopFreezeR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
const DisEntityId& DisStopFreezeR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisStopFreezeR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisStopFreezeR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisStopFreezeR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
void DisStopFreezeR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RealWorldTime  = " << mRealWorldTime << std::endl;
   aStream << "      => Reason         = " << int(mReason) << std::endl;
   aStream << "      => FrozenBehavior = " << int(mFrozenBehavior) << std::endl;
   aStream << "      => RequestId      = " << int(mRequestId) << std::endl;
   aStream << "      === End DisStopFreezeR ===" << std::endl << std::endl;
}

bool DisStopFreezeR::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntity.IsValid())
      isValid = false;
   if (!mReceivingEntity.IsValid())
      isValid = false;
   if (!mRealWorldTime.IsValid())
      isValid = false;
   return isValid;
}
