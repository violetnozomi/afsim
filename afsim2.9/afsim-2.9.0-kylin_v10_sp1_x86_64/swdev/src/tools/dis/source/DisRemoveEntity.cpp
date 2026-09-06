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

#include "DisRemoveEntity.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId (4 bytes).
const DisUint16 sDisRemoveEntitySize = (48 + 48 + 32) / 8;
} // namespace

DisRemoveEntity::DisRemoveEntity()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetPduType(DisEnum::Pdu::Type::RemoveEntity);
   SetLength(DisPdu::GetBaseLength() + sDisRemoveEntitySize);
}

DisRemoveEntity::DisRemoveEntity(const DisRemoveEntity& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
{
}

DisRemoveEntity::DisRemoveEntity(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
{
   GetMemberData(aGenI);
}

DisRemoveEntity::DisRemoveEntity(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisRemoveEntity::~DisRemoveEntity() {}

// virtual
DisPdu* DisRemoveEntity::Clone() const
{
   return new DisRemoveEntity(*this);
}

// virtual
int DisRemoveEntity::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RemoveEntity;
}

// virtual
void DisRemoveEntity::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisRemoveEntity::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisRemoveEntity::Put(GenO& aGenO)
{
   GetLength(); // Make sure size is up to date.
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
}

// virtual
void DisRemoveEntity::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisRemoveEntity::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisRemoveEntity::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisRemoveEntity::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisRemoveEntity::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisRemoveEntity::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisRemoveEntity::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId = " << mRequestId << std::endl;
   aStream << "      === End DisRemoveEntity === " << std::endl << std::endl;
}
