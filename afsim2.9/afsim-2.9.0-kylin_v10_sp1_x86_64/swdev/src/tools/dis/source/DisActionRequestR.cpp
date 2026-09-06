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

#include "DisActionRequestR.hpp"

#include "Dis.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, (Reliability + byte padding), + ActionId.
const DisUint16 sDisActionRequestRSize = (48 + 48 + 32 + (8 + 24) + 32) / 8;
} // namespace

DisActionRequestR::DisActionRequestR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReliabilityService(0)
   , mRequestId(0)
   , mActionId(0)
   , mData()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::ActionRequestR);
   SetLength(DisPdu::GetBaseLength() + sDisActionRequestRSize + mData.GetLength());
}

DisActionRequestR::DisActionRequestR(const DisActionRequestR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mReliabilityService(aSrc.mReliabilityService)
   , mRequestId(aSrc.mRequestId)
   , mActionId(aSrc.mActionId)
   , mData(aSrc.mData)
{
}

DisActionRequestR::DisActionRequestR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mData()
{
   GetMemberData(aGenI);
}

DisActionRequestR::DisActionRequestR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisActionRequestR::~DisActionRequestR() {}

// virtual
DisPdu* DisActionRequestR::Clone() const
{
   return new DisActionRequestR(*this);
}

// virtual
int DisActionRequestR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ActionRequestR;
}

// virtual
DisUint16 DisActionRequestR::GetLength()
{
   // The 32 bit lengths for number of both fixed and variable records is in the mData.GetLength().
   DisUint16 totalLength = DisPdu::GetBaseLength() + sDisActionRequestRSize + mData.GetLength();
   SetLength(totalLength);
   return totalLength;
}

// virtual
void DisActionRequestR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisActionRequestR::GetMemberData(GenI& aGenI)
{
   // The base DIS PDU data is already "gotten" separately.
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint8 padding;
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mReliabilityService;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> mRequestId;
   aGenI >> mActionId;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisActionRequestR::Put(GenO& aGenO)
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
   aGenO << mActionId;
   mData.Put(aGenO);
}

/*void DisActionRequestR::Copy(const DisActionRequestR& aRhs)
{
   mOriginatingEntity   = aRhs.mOriginatingEntity;
   mReceivingEntity     = aRhs.mReceivingEntity;
   mReliabilityService  = aRhs.mReliabilityService;
   mRequestId           = aRhs.mRequestId;

   mActionId            = aRhs.mActionId;
   mData                = aRhs.mData;
}*/

// virtual
void DisActionRequestR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisActionRequestR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisActionRequestR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
void DisActionRequestR::SetReliabilityService(DisEnum8 aReliabilityService)
{
   mReliabilityService = aReliabilityService;
}

// virtual
const DisEntityId& DisActionRequestR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisActionRequestR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisActionRequestR::GetRequestId() const
{
   return mRequestId;
}

// virtual
DisEnum8 DisActionRequestR::GetReliabilityService() const
{
   return mReliabilityService;
}

// virtual
void DisActionRequestR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId   = " << mRequestId << std::endl;
   aStream << "      => ActionId    = " << mActionId << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisActionRequestR ===" << std::endl << std::endl;
}

bool DisActionRequestR::IsValid() const
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
