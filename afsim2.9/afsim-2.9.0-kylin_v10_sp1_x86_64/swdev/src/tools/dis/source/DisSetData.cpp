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

#include "DisSetData.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "GenOConvertBigEndian.hpp"

namespace
{
const DisUint16 sDisSetDataSize = (48 + 48 + (32 + 32)) / 8;
}

DisSetData::DisSetData()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::SetData);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   SetLength(DisPdu::GetBaseLength() + sDisSetDataSize + mData.GetLength());
}

DisSetData::DisSetData(const DisSetData& aSrc)
   : DisPdu(aSrc)
   , mData()
{
   Copy(aSrc);
}

DisSetData::DisSetData(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mData()
{
   GetMemberData(aGenI);
}

DisSetData::DisSetData(GenI& aGenI)
   : DisPdu(aGenI)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisSetData::~DisSetData() {}

// virtual
DisPdu* DisSetData::Clone() const
{
   return new DisSetData(*this);
}

// virtual
int DisSetData::GetClass() const
{
   return (int)DisEnum::Pdu::Type::SetData;
}

// Copy the data portion from one DisSetData to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisSetData::Copy(const DisSetData& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;
   mRequestId         = aSrc.mRequestId;
   mData              = aSrc.mData;
}

// virtual
void DisSetData::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisSetData::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisSetDataSize + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisSetData::GetMemberData(GenI& aGenI)
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
void DisSetData::Put(GenO& aGenO)
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

const DisEntityId& DisSetData::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

DisUint32 DisSetData::GetNumFixedDatums() const
{
   return GetData().GetNumFixedDatums();
}

DisUint32 DisSetData::GetNumVariableDatums() const
{
   return GetData().GetNumVariableDatums();
}

// virtual
void DisSetData::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => RequestId      = " << int(mRequestId) << std::endl;
   aStream << "      === End DisSetData ===" << std::endl << std::endl;
}

bool DisSetData::IsValid() const
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
