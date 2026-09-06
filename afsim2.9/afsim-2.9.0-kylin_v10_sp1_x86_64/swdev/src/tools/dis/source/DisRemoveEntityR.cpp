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

#include "DisRemoveEntityR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each) + (Reliability (8-bits) + byte padding) + RequestId (4 bytes)
const DisUint16 sDisRemoveEntityRSize = (48 + 48 + (8 + 24) + 32) / 8;
} // namespace

DisRemoveEntityR::DisRemoveEntityR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mRequestId(0)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::RemoveEntityR);
   SetLength(DisPdu::GetBaseLength() + sDisRemoveEntityRSize);
}

DisRemoveEntityR::DisRemoveEntityR(const DisRemoveEntityR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mReliabilityService(aSrc.mReliabilityService)
   , mRequestId(aSrc.mRequestId)
{
}

DisRemoveEntityR::DisRemoveEntityR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisRemoveEntityR::DisRemoveEntityR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisRemoveEntityR::~DisRemoveEntityR() {}

// virtual
DisPdu* DisRemoveEntityR::Clone() const
{
   return new DisRemoveEntityR(*this);
}

// virtual
int DisRemoveEntityR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RemoveEntityR;
}

// virtual
void DisRemoveEntityR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisRemoveEntityR::GetMemberData(aGenI);
}

// private
void DisRemoveEntityR::GetMemberData(GenI& aGenI)
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
void DisRemoveEntityR::Put(GenO& aGenO)
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
void DisRemoveEntityR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisRemoveEntityR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisRemoveEntityR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisRemoveEntityR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
const DisEntityId& DisRemoveEntityR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisRemoveEntityR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisRemoveEntityR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisRemoveEntityR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
void DisRemoveEntityR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId = " << mRequestId << std::endl;
   aStream << "      === End DisRemoveEntityR ===" << std::endl << std::endl;
}
