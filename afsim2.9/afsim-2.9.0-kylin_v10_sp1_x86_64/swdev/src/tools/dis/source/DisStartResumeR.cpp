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

#include "DisStartResumeR.hpp"

#include <iostream>

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RealWorlTime, SimulationTime, RequestId, Reliability, 3PadBytes).
const DisUint16 sDisStartResumeRSize = (48 + 48 + 64 + 64 + (8 + 24) + 32) / 8;
} // namespace

DisStartResumeR::DisStartResumeR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRealWorldTime()
   , mSimulationTime()
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mRequestId(0)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::StartResumeR);
   SetLength(DisPdu::GetBaseLength() + sDisStartResumeRSize);
}

DisStartResumeR::DisStartResumeR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu) // default copy into base class
{
   GetMemberData(aGenI);
}

DisStartResumeR::DisStartResumeR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisStartResumeR::~DisStartResumeR() {}

// virtual
DisPdu* DisStartResumeR::Clone() const
{
   return new DisStartResumeR(*this);
}

// Virtual
int DisStartResumeR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::StartResumeR;
}

void DisStartResumeR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisStartResumeR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   DisUint8 padding8 = 0;
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRealWorldTime;
   aGenO << mSimulationTime;
   aGenO << mReliabilityService;
   aGenO << padding8;
   aGenO << padding8;
   aGenO << padding8;
   aGenO << mRequestId;
}

void DisStartResumeR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint8  padding8     = 0;
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRealWorldTime;
   aGenI >> mSimulationTime;
   aGenI >> mReliabilityService;
   aGenI >> padding8;
   aGenI >> padding8;
   aGenI >> padding8;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
DisUint16 DisStartResumeR::GetLength()
{
   const DisUint16 byteCount = DisPdu::GetBaseLength() + sDisStartResumeRSize;
   SetLength(byteCount);
   return byteCount;
}

const DisEntityId& DisStartResumeR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisStartResumeR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Originating Entity = " << GetOriginatingEntity() << std::endl;
   aStream << "      => Receiving Entity   = " << GetReceivingEntity() << std::endl;
   aStream << "      => RealWorldTime      = " << mRealWorldTime << std::endl;
   aStream << "      => SimulationTime     = " << mSimulationTime << std::endl;
   aStream << "      => RequestId          = " << int(mRequestId) << std::endl;
   aStream << "      => Reliability        = " << int(mReliabilityService) << " (Ack = 0, NoAck = 1)" << std::endl;
   aStream << "      === End DisStartResumeR ===" << std::endl << std::endl;
}

bool DisStartResumeR::IsValid() const
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
   if (!mSimulationTime.IsValid())
      isValid = false;
   return isValid;
}
