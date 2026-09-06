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

#include "DisDataR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "math.h"

namespace
{
const DisUint16 sDisDataRSize = (48 + 48 + 32 + (8 + 24)) / 8;
}

DisDataR::DisDataR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::DataR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetLength(DisPdu::GetBaseLength() + sDisDataRSize + mData.GetLength());
}

DisDataR::DisDataR(const DisDataR& aSrc)
   : DisPdu(aSrc)
{
   Copy(aSrc);
}

DisDataR::DisDataR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mData()
{
   GetMemberData(aGenI);
}

DisDataR::DisDataR(GenI& aGenI)
   : DisPdu(aGenI)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisDataR::~DisDataR() {}

// virtual
DisPdu* DisDataR::Clone() const
{
   return new DisDataR(*this);
}

// virtual
int DisDataR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::DataR;
}

// Copy the data portion from one DisDataR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisDataR::Copy(const DisDataR& aSrc)
{
   mOriginatingEntity  = aSrc.mOriginatingEntity;
   mReceivingEntity    = aSrc.mReceivingEntity;
   mRequestId          = aSrc.mRequestId;
   mReliabilityService = aSrc.mReliabilityService;
   mData               = aSrc.mData;
}

// virtual
void DisDataR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   DisDataR::GetMemberData(aGenI);
}

// virtual
DisUint16 DisDataR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisDataRSize + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisDataR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint8 padding;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mReliabilityService;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> padding;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();

   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisDataR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.

   DisPdu::Put(aGenO);

   DisUint8 padding = 0;
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mReliabilityService;
   aGenO << padding;
   aGenO << padding;
   aGenO << padding;

   mData.Put(aGenO);
}

// virtual
void DisDataR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisDataR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisDataR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisDataR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
const DisEntityId& DisDataR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisDataR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisDataR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisDataR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
void DisDataR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId        = " << mRequestId << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisDataR ===" << std::endl << std::endl;
}
