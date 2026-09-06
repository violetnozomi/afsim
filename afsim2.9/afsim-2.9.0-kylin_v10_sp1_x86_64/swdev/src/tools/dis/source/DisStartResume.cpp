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

#include "DisStartResume.hpp"

#include <iostream>

#include "DisClockTime.hpp"
#include "DisEntityId.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

static const DisUint16 sDisStartResumePduSize = 44; // 352 bytes

DisStartResume::DisStartResume()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRealWorldTime()
   , mSimulationTime()
   , mRequestId(0)
{
   SetPduType(DisEnum::Pdu::Type::StartResume);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(sDisStartResumePduSize);
}

DisStartResume::DisStartResume(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity()
   , mReceivingEntity()
   , mRealWorldTime()
   , mSimulationTime()
   , mRequestId(0)
{
   GetMemberData(aGenI);
}

DisStartResume::DisStartResume(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisStartResume::~DisStartResume() {}

// virtual
DisPdu* DisStartResume::Clone() const
{
   return new DisStartResume(*this);
}

// Virtual
int DisStartResume::GetClass() const
{
   return (int)DisEnum::Pdu::Type::StartResume;
}

void DisStartResume::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisStartResume::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRealWorldTime;
   aGenI >> mSimulationTime;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisStartResume::Put(GenO& aGenO)
{
   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRealWorldTime;
   aGenO << mSimulationTime;
   aGenO << mRequestId;
}

const DisEntityId& DisStartResume::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisStartResume::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Originating Entity = " << GetOriginatingEntity() << std::endl;
   aStream << "      => Receiving Entity   = " << GetReceivingEntity() << std::endl;
   aStream << "      => Real World Time    = " << mRealWorldTime << std::endl;
   aStream << "      => Simulation Time    = " << mSimulationTime << std::endl;
   aStream << "      => RequestId   = " << static_cast<int>(mRequestId) << std::endl;
   aStream << "      === End DisStartResume ===" << std::endl << std::endl;
}

bool DisStartResume::IsValid() const
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
