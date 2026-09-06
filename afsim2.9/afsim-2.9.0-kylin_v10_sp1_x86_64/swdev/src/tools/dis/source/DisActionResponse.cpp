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

#include "DisActionResponse.hpp"

#include "DisActionEnums.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, ResponseStatus
const DisUint16 sDisActionResponseSize = (48 + 48 + 32 + 32) / 8;
} // namespace

DisActionResponse::DisActionResponse()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mRequestStatus(DisEnum::Action::Request::OTHER)
   , mData()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetPduType(DisEnum::Pdu::Type::ActionResponse);
   SetLength(DisPdu::GetBaseLength() + sDisActionResponseSize + mData.GetLength());
}

DisActionResponse::DisActionResponse(const DisActionResponse& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
   , mRequestStatus(aSrc.mRequestStatus)
   , mData(aSrc.mData)
{
}

DisActionResponse::DisActionResponse(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mRequestStatus(0)
   , mData()
{
   GetMemberData(aGenI);
}

DisActionResponse::DisActionResponse(GenI& aGenI)
   : DisPdu()
   , mRequestStatus(0)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisActionResponse::~DisActionResponse() {}

// virtual
DisPdu* DisActionResponse::Clone() const
{
   return new DisActionResponse(*this);
}

// virtual
int DisActionResponse::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ActionResponse;
}

// virtual
void DisActionResponse::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisActionResponse::GetMemberData(GenI& aGenI)
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
void DisActionResponse::Put(GenO& aGenO)
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
DisUint16 DisActionResponse::GetLength()
{
   DisUint16 totalLength = DisPdu::GetBaseLength() + sDisActionResponseSize + mData.GetLength();
   SetLength(totalLength);
   return totalLength;
}

// virtual
void DisActionResponse::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisActionResponse::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisActionResponse::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisActionResponse::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisActionResponse::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisActionResponse::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisActionResponse::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId          = " << mRequestId << std::endl;
   aStream << "      => Request Status     = " << mRequestStatus << std::endl;
   mData.Stream(aStream);
   aStream << "      === DisActionResponse === " << std::endl << std::endl;
}

bool DisActionResponse::IsValid() const
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
