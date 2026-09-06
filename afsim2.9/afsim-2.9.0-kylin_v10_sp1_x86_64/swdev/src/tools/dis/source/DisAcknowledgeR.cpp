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

#include "DisAcknowledgeR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, Acknowledge, Response.
const DisUint16 sDisAcknowledgeRSize = (48 + 48 + 16 + 16 + 32) / 8;
} // namespace

DisAcknowledgeR::DisAcknowledgeR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mAcknowledgeFlag(0)
   , mResponseFlag(0)
   , mRequestId(0)
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::AcknowledgeR);
   SetLength(DisPdu::GetBaseLength() + sDisAcknowledgeRSize);
}


DisAcknowledgeR::DisAcknowledgeR(const DisAcknowledgeR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mAcknowledgeFlag(aSrc.mAcknowledgeFlag)
   , mResponseFlag(aSrc.mResponseFlag)
   , mRequestId(aSrc.mRequestId)
{
}

DisAcknowledgeR::DisAcknowledgeR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisAcknowledgeR::DisAcknowledgeR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisAcknowledgeR::~DisAcknowledgeR() {}

// virtual
DisPdu* DisAcknowledgeR::Clone() const
{
   return new DisAcknowledgeR(*this);
}

// virtual
int DisAcknowledgeR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::AcknowledgeR;
}

void DisAcknowledgeR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisAcknowledgeR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mAcknowledgeFlag;
   aGenI >> mResponseFlag;
   aGenI >> mRequestId;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisAcknowledgeR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mAcknowledgeFlag;
   aGenO << mResponseFlag;
   aGenO << mRequestId;
}

// virtual
void DisAcknowledgeR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisAcknowledgeR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisAcknowledgeR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisAcknowledgeR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisAcknowledgeR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisAcknowledgeR::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisAcknowledgeR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Acknowledge Flag = " << mAcknowledgeFlag << std::endl;
   aStream << "      => Response Flag    = " << mResponseFlag << std::endl;
   aStream << "      => Request ID       = " << mRequestId << std::endl;
   aStream << "      === End DisAcknowledgeR ===" << std::endl << std::endl;
}

bool DisAcknowledgeR::IsValid() const
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
