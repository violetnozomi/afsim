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

#include "DisData.hpp"

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "GenOConvertBigEndian.hpp"

namespace
{
const DisUint16 sDisDataSize = (48 + 48 + (32 + 32)) / 8;
}

DisData::DisData()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::Data);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(DisPdu::GetBaseLength() + sDisDataSize + mData.GetLength());
}

DisData::DisData(const DisData& aSrc)
   : DisPdu(aSrc)
   , mData()
{
   Copy(aSrc);
   SetPduType(DisEnum::Pdu::Type::Data);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(DisPdu::GetBaseLength() + sDisDataSize + mData.GetLength());
}

DisData::DisData(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mData()
{
   GetMemberData(aGenI);
}

DisData::DisData(GenI& aGenI)
   : DisPdu(aGenI)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisData::~DisData() {}

// virtual
DisPdu* DisData::Clone() const
{
   return new DisData(*this);
}

// virtual
int DisData::GetClass() const
{
   return (int)DisEnum::Pdu::Type::Data;
}

// Copy the data portion from one DisData to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisData::Copy(const DisData& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;
   mRequestId         = aSrc.mRequestId;
   mData              = aSrc.mData;
}

// virtual
void DisData::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisData::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisDataSize + mData.GetLength();

   SetLength(byteCount);
   return byteCount;
}

// private
void DisData::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   DisUint32 padding;

   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mRequestId;
   aGenI >> padding;

   mData.Get(aGenI);

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();

   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisData::Put(GenO& aGenO)
{
   DisUint32 padding = 0;

   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mRequestId;
   aGenO << padding;
   mData.Put(aGenO);
}

// virtual
void DisData::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisData::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisData::SetRequestId(DisUint32 aRequestId)
{
   mRequestId = aRequestId;
}

// virtual
const DisEntityId& DisData::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisData::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
DisUint32 DisData::GetRequestId() const
{
   return mRequestId;
}

DisUint32 DisData::GetNumFixedDatums() const
{
   return GetData().GetNumFixedDatums();
}

DisUint32 DisData::GetNumVariableDatums() const
{
   return GetData().GetNumVariableDatums();
}

// virtual
void DisData::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId      = " << int(mRequestId) << std::endl;
   aStream << "      === End DisSetData ===" << std::endl << std::endl;
}

bool DisData::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
   {
      isValid = false;
   }
   if (!mOriginatingEntity.IsValid())
   {
      isValid = false;
   }
   if (!mReceivingEntity.IsValid())
   {
      isValid = false;
   }
   if (!mData.IsValid())
   {
      isValid = false;
   }
   return isValid;
}
