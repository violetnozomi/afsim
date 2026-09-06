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

#include "DisStopFreeze.hpp"

#include <iostream>

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

static const DisUint16 sDisStopFreezePduSize = 40; // 320 bits

DisStopFreeze::DisStopFreeze()
   : DisPdu()
   , mOriginatingEntity()
   , mReceivingEntity()
   , mRealWorldTime()
   , mReason(0)
   , mFrozenBehavior(0)
   , mRequestId(0)
{
   SetPduType(DisEnum::Pdu::Type::StopFreeze);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(sDisStopFreezePduSize);
}

DisStopFreeze::DisStopFreeze(const DisStopFreeze& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRealWorldTime(aSrc.mRealWorldTime)
   , mReason(aSrc.mReason)
   , mFrozenBehavior(aSrc.mFrozenBehavior)
   , mRequestId(aSrc.mRequestId)
{
}

DisStopFreeze::DisStopFreeze(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisStopFreeze::DisStopFreeze(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisStopFreeze::~DisStopFreeze() {}

// virtual
DisPdu* DisStopFreeze::Clone() const
{
   return new DisStopFreeze(*this);
}

// virtual
int DisStopFreeze::GetClass() const
{
   return (int)DisEnum::Pdu::Type::StopFreeze;
}

// virtual
void DisStopFreeze::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisStopFreeze::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint16 pad16;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRealWorldTime;
   aGenI >> mReason;
   aGenI >> mFrozenBehavior;
   aGenI >> pad16;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisStopFreeze::Put(GenO& aGenO)
{
   DisUint16 pad16(0);

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRealWorldTime;
   aGenO << mReason;
   aGenO << mFrozenBehavior;
   aGenO << pad16;
   aGenO << mRequestId;
}


// virtual
void DisStopFreeze::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}
void DisStopFreeze::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}
void DisStopFreeze::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}
const DisEntityId& DisStopFreeze::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisStopFreeze::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisStopFreeze::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisStopFreeze::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RealWorldTime  = " << mRealWorldTime << std::endl;
   aStream << "      => Reason         = " << int(mReason) << std::endl;
   aStream << "      => FrozenBehavior = " << int(mFrozenBehavior) << std::endl;
   aStream << "      => RequestId      = " << int(mRequestId) << std::endl;
   aStream << "      === End DisStopFreezeR ===" << std::endl << std::endl;
}

bool DisStopFreeze::IsValid() const
{
   if (!DisPdu::IsValid())
      return false;
   if (!mOriginatingEntity.IsValid())
      return false;
   if (!mReceivingEntity.IsValid())
      return false;
   if (!mRealWorldTime.IsValid())
      return false;
   return true;
}
