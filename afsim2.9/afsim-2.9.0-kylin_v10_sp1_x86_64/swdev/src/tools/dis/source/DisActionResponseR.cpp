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

#include "DisActionResponseR.hpp"

#include <iostream> // Only for debugging

#include "DisActionEnums.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, ResponseStatus
const DisUint16 sDisActionResponseRSize = (48 + 48 + 32 + 32) / 8;
} // namespace

DisActionResponseR::DisActionResponseR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mRequestStatus(DisEnum::Action::Request::OTHER)
   , mData()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   SetPduType(DisEnum::Pdu::Type::ActionResponseR);
   SetLength(DisPdu::GetBaseLength() + sDisActionResponseRSize + mData.GetLength());
}

DisActionResponseR::DisActionResponseR(const DisActionResponseR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
   , mRequestStatus(aSrc.mRequestStatus)
   , mData(aSrc.mData)
{
}

DisActionResponseR::DisActionResponseR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mRequestStatus(0)
   , mData()
{
   GetMemberData(aGenI);
}

DisActionResponseR::DisActionResponseR(GenI& aGenI)
   : DisPdu()
   , mRequestStatus(0)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisActionResponseR::~DisActionResponseR() {}

// virtual
DisPdu* DisActionResponseR::Clone() const
{
   return new DisActionResponseR(*this);
}

// virtual
int DisActionResponseR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ActionResponseR;
}

// virtual
void DisActionResponseR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisActionResponseR::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mRequestStatus;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisActionResponseR::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mRequestStatus;

   mData.Put(aGenO);
}

// virtual
DisUint16 DisActionResponseR::GetLength()
{
   DisUint16 totalLength = DisPdu::GetBaseLength() + sDisActionResponseRSize + mData.GetLength();
   SetLength(totalLength);
   return totalLength;
}

// virtual
void DisActionResponseR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisActionResponseR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisActionResponseR::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisActionResponseR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisActionResponseR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisActionResponseR::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisActionResponseR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId           = " << mRequestId << std::endl;
   aStream << "      => Request Status      = " << mRequestStatus << std::endl;
   mData.Stream(aStream);
   aStream << "      === DisActionResponseR === " << std::endl << std::endl;
}

bool DisActionResponseR::IsValid() const
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
