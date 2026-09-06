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

#include "DisSetDataR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
DisUint16 sDisSetDataRSize = (48 + 48 + (8 + 24) + 32) / 8;
}

DisSetDataR::DisSetDataR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReliabilityService(DisEnum::Pdu::Reliability::Acknowledged)
   , mRequestId(0)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::SetDataR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisSetDataR::DisSetDataR(const DisSetDataR& aSrc)
   : DisPdu(aSrc)
   , mData()
{
   Copy(aSrc);
}

DisSetDataR::DisSetDataR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mData()
{
   GetMemberData(aGenI);
}

DisSetDataR::DisSetDataR(GenI& aGenI)
   : DisPdu(aGenI)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisSetDataR::~DisSetDataR() {}

// virtual
DisPdu* DisSetDataR::Clone() const
{
   return new DisSetDataR(*this);
}

// virtual
int DisSetDataR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::SetDataR;
}

// Copy the data portion from one DisSetDataR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisSetDataR::Copy(const DisSetDataR& aSrc)
{
   mOriginatingEntity  = aSrc.mOriginatingEntity;
   mReceivingEntity    = aSrc.mReceivingEntity;
   mReliabilityService = aSrc.mReliabilityService;
   mRequestId          = aSrc.mRequestId;
   mData               = aSrc.mData;
}

// virtual
void DisSetDataR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisSetDataR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisSetDataRSize + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisSetDataR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint8 padding8;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mReliabilityService;
   aGenI >> padding8;
   aGenI >> padding8;
   aGenI >> padding8;
   aGenI >> mRequestId;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisSetDataR::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date

   DisUint8 padding8 = 0;

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mReliabilityService;
   aGenO << padding8;
   aGenO << padding8;
   aGenO << padding8;
   aGenO << mRequestId;

   mData.Put(aGenO);
}

const DisEntityId& DisSetDataR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisSetDataR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId = " << static_cast<int>(mRequestId) << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisSetDataR ===" << std::endl << std::endl;
}

bool DisSetDataR::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntity.IsValid())
      isValid = false;
   if (!mReceivingEntity.IsValid())
      isValid = false;
   if (!mData.IsValid())
      isValid = false;
   return isValid;
}
