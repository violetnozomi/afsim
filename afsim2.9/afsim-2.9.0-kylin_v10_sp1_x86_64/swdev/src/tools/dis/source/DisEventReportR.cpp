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

#include "DisEventReportR.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// Do not include here the bits for fixed and variable record numbers... booked with mData;
DisUint16 sDisEventReportRSize = (48 + 48 + 32) / 8;
} // namespace

DisEventReportR::DisEventReportR()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::EventReportR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
   GetLength();
}

DisEventReportR::DisEventReportR(const DisEventReportR& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mEventType(aSrc.mEventType)
   , mData(aSrc.mData)
{
   Copy(aSrc);
}

DisEventReportR::DisEventReportR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   GetMemberData(aGenI);
}

DisEventReportR::DisEventReportR(GenI& aGenI)
   : DisPdu(aGenI)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisEventReportR::~DisEventReportR() {}

// virtual
DisPdu* DisEventReportR::Clone() const
{
   return new DisEventReportR(*this);
}

// virtual
int DisEventReportR::GetClass() const
{
   return (int)DisEnum::Pdu::Type::EventReportR;
}

// Copy the data portion from one DisEventReportR to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisEventReportR::Copy(const DisEventReportR& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;
   mEventType         = aSrc.mEventType;
   mData              = aSrc.mData;
}

// virtual
void DisEventReportR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisEventReportR::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisEventReportRSize + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisEventReportR::GetMemberData(GenI& aGenI)
{
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mEventType;
   mData.Get(aGenI);
}

// virtual1
void DisEventReportR::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mEventType;
   mData.Put(aGenO);
}

DisEventReportR& DisEventReportR::operator=(const DisEventReportR& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisEventReportR::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisEventReportR::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

// virtual
void DisEventReportR::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisEventReportR::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisEventReportR::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Event Type = " << static_cast<int>(mEventType) << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisEventReportR ===" << std::endl << std::endl;
}
