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

#include "DisCreateEntity.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisCreateEntity::DisCreateEntity()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
{
   SetPduType(DisEnum::Pdu::Type::CreateEntity);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(28); // 224 bits which represents the header information
}

DisCreateEntity::DisCreateEntity(const DisCreateEntity& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
{
}

DisCreateEntity::DisCreateEntity(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisCreateEntity::DisCreateEntity(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisCreateEntity::~DisCreateEntity() {}

// virtual
DisPdu* DisCreateEntity::Clone() const
{
   return new DisCreateEntity(*this);
}

// virtual
int DisCreateEntity::GetClass() const
{
   return (int)DisEnum::Pdu::Type::CreateEntity;
}

// virtual
void DisCreateEntity::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisCreateEntity::GetMemberData(aGenI);
}

// private
void DisCreateEntity::GetMemberData(GenI& aGenI)
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
void DisCreateEntity::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
}

// virtual
void DisCreateEntity::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId = " << int(mRequestId) << std::endl;
   aStream << "      === End DisCreateEntity ===" << std::endl << std::endl;
}

bool DisCreateEntity::IsValid() const
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
