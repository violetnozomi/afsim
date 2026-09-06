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

#include "DisAcknowledge.hpp"

#include <iostream> // Only for debugging
#include <string.h> // for memcpy

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "math.h"

DisAcknowledge::DisAcknowledge()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mAcknowledgeFlag(0)
   , mResponseFlag(0)
   , mRequestId(0)
{
   SetPduType(DisEnum::Pdu::Type::Acknowledge);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(32); // 256 bits which represents the header information
}

DisAcknowledge::DisAcknowledge(const DisAcknowledge& aSrc)
   : DisPdu()
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mAcknowledgeFlag(aSrc.mAcknowledgeFlag)
   , mResponseFlag(aSrc.mResponseFlag)
   , mRequestId(aSrc.mRequestId)
{
}

DisAcknowledge::DisAcknowledge(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisAcknowledge::DisAcknowledge(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisAcknowledge::~DisAcknowledge() {}

// virtual
DisPdu* DisAcknowledge::Clone() const
{
   return new DisAcknowledge(*this);
}

// virtual
int DisAcknowledge::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Acknowledge;
}

// virtual
void DisAcknowledge::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisAcknowledge::GetMemberData(aGenI);
}

// private
void DisAcknowledge::GetMemberData(GenI& aGenI)
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
void DisAcknowledge::Put(GenO& aGenO)
{
   // GetLength(); // Ensure length is up-to-date (fixed at 32)

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mAcknowledgeFlag;
   aGenO << mResponseFlag;
   aGenO << mRequestId;
}

// virtual
void DisAcknowledge::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisAcknowledge::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisAcknowledge::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisAcknowledge::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisAcknowledge::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisAcknowledge::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Acknowledge Flag = " << mAcknowledgeFlag << std::endl;
   aStream << "      => Response Flag    = " << mResponseFlag << std::endl;
   aStream << "      => Request ID       = " << mRequestId << std::endl;
   aStream << "      === End DisAcknowledge ===" << std::endl << std::endl;
}

bool DisAcknowledge::IsValid() const
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
