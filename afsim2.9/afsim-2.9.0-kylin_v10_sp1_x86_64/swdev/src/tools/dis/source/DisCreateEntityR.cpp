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

#include "DisCreateEntityR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, (Reliability + byte padding).
const DisUint16 sDisCreateEntityRSize = (48 + 48 + 32 + (8 + 24)) / 8;
} // namespace

DisCreateEntityR::DisCreateEntityR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::CreateEntityR);
   SetLength(DisPdu::GetBaseLength() + sDisCreateEntityRSize);
}

DisCreateEntityR::DisCreateEntityR(const DisCreateEntityR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
   , mReliabilityService(aSrc.mReliabilityService)
{
}

DisCreateEntityR::DisCreateEntityR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
{
   GetMemberData(aGenI);
}

DisCreateEntityR::DisCreateEntityR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisCreateEntityR::~DisCreateEntityR() {}

// virtual
DisPdu* DisCreateEntityR::Clone() const
{
   return new DisCreateEntityR(*this);
}

// virtual
int DisCreateEntityR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::CreateEntityR;
}

// virtual
void DisCreateEntityR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisCreateEntityR::GetMemberData(aGenI);
}

// private
void DisCreateEntityR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint8 padding;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mReliabilityService;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisCreateEntityR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   DisUint8 padding = 0;
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mReliabilityService;
   aGenO << padding;
   aGenO << padding;
   aGenO << padding;
   aGenO << mRequestId;
}

// virtual
void DisCreateEntityR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisCreateEntityR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisCreateEntityR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisCreateEntityR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
const DisEntityId& DisCreateEntityR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisCreateEntityR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisCreateEntityR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisCreateEntityR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
void DisCreateEntityR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId = " << int(mRequestId) << std::endl;
   aStream << "      === End DisCreateEntityR ===" << std::endl << std::endl;
}

bool DisCreateEntityR::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntity.IsValid())
      isValid = false;
   if (!mReceivingEntity.IsValid())
      isValid = false;
   return isValid;
}
