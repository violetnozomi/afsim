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

#include "DisActionRequest.hpp"

#include <iostream> // Only for debugging

#include "Dis.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Sizes: Two DisEntityIds (6 bytes each), RequestId, ActionId.
const DisUint16 sDisActionRequestSize = (48 + 48 + 32 + 32) / 8;
} // namespace

DisActionRequest::DisActionRequest()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mActionId(0)
   , mData()
{
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetPduType(DisEnum::Pdu::Type::ActionRequest);
   SetLength(DisPdu::GetBaseLength() + sDisActionRequestSize + mData.GetLength());
}

DisActionRequest::DisActionRequest(const DisActionRequest& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mRequestId(aSrc.mRequestId)
   , mActionId(aSrc.mActionId)
   , mData(aSrc.mData)
{
}

DisActionRequest::DisActionRequest(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mActionId(0)
   , mData()
{
   GetMemberData(aGenI);
}

DisActionRequest::DisActionRequest(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisActionRequest::~DisActionRequest() {}

// virtual
DisPdu* DisActionRequest::Clone() const
{
   return new DisActionRequest(*this);
}

// virtual
int DisActionRequest::GetClass() const
{
   return (int)DisEnum::Pdu::Type::ActionRequest;
}

// virtual
DisUint16 DisActionRequest::GetLength()
{
   // The 32 bit lengths for number of both fixed and variable records is in the mData.GetLength().
   DisUint16 totalLength = DisPdu::GetBaseLength() + sDisActionRequestSize + mData.GetLength();
   SetLength(totalLength);
   return totalLength;
}

// virtual
void DisActionRequest::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// private
void DisActionRequest::GetMemberData(GenI& aGenI)
{
   // The base DIS PDU data is already "gotten" separately.
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> mActionId;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisActionRequest::Put(GenO& aGenO)
{
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << mActionId;
   mData.Put(aGenO);
}

/*void DisActionRequest::Copy(const DisActionRequest& aRhs)
{
   mOriginatingEntity   = aRhs.mOriginatingEntity;
   mReceivingEntity     = aRhs.mReceivingEntity;
   mRequestId           = aRhs.mRequestId;

   mActionId            = aRhs.mActionId;
   mData                = aRhs.mData;
}*/

// virtual
void DisActionRequest::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisActionRequest::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisActionRequest::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisActionRequest::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisActionRequest::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisActionRequest::GetRequestId() const
{
   return mRequestId;
}

// virtual
void DisActionRequest::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId   = " << mRequestId << std::endl;
   aStream << "      => ActionId    = " << mActionId << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisActionRequest ===" << std::endl << std::endl;
}

bool DisActionRequest::IsValid() const
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
