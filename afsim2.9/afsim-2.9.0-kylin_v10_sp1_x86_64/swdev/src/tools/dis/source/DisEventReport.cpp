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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "DisEventReport.hpp"

#include <iostream> // Only for debugging

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
// DO NOT INCLUDE BITS FOR NUMBER OF FIXED AND VARIABLE RECORDS = ( 32 + 32 ) / 8;
const DisUint16 sDisEventReportSize = (48 + 48 + 32 + (32)) / 8;
} // namespace

DisEventReport::DisEventReport()
   : DisPdu()
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   SetPduType(DisEnum::Pdu::Type::EventReport);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
   GetLength();
}

DisEventReport::DisEventReport(const DisEventReport& aSrc)
   : DisPdu(aSrc)
   , mOriginatingEntity(aSrc.mOriginatingEntity)
   , mReceivingEntity(aSrc.mReceivingEntity)
   , mEventType(aSrc.mEventType)
   , mData(aSrc.mData)
{
   Copy(aSrc);
}

DisEventReport::DisEventReport(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   GetMemberData(aGenI);
}

DisEventReport::DisEventReport(GenI& aGenI)
   : DisPdu(aGenI)
   , mOriginatingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntity(DisEntityId::ENTITY_ID_UNKNOWN)
   , mEventType(0)
   , mData()
{
   GetMemberData(aGenI);
}

// virtual
DisEventReport::~DisEventReport() {}

// virtual
DisPdu* DisEventReport::Clone() const
{
   return new DisEventReport(*this);
}

// virtual
int DisEventReport::GetClass() const
{
   return (int)DisEnum::Pdu::Type::EventReport;
}

// Copy the data portion from one DisEventReport to another.
// The new data is always owned by the class, even if
// the source was owned by the user.

// private
void DisEventReport::Copy(const DisEventReport& aSrc)
{
   mOriginatingEntity = aSrc.mOriginatingEntity;
   mReceivingEntity   = aSrc.mReceivingEntity;
   mEventType         = aSrc.mEventType;
   mData              = aSrc.mData;
}

// virtual
void DisEventReport::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisEventReport::GetLength()
{
   DisUint16 byteCount = DisPdu::GetBaseLength() + sDisEventReportSize + mData.GetLength();
   SetLength(byteCount);
   return byteCount;
}

// private
void DisEventReport::GetMemberData(GenI& aGenI)
{
   DisUint32 pad32 = 0;
   aGenI >> mOriginatingEntity;
   aGenI >> mReceivingEntity;
   aGenI >> mEventType;
   aGenI >> pad32;
   mData.Get(aGenI);
}

// virtual
void DisEventReport::Put(GenO& aGenO)
{
   DisUint32 pad32 = 0;
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mOriginatingEntity;
   aGenO << mReceivingEntity;
   aGenO << mEventType;
   aGenO << pad32;
   mData.Put(aGenO);
}

DisEventReport& DisEventReport::operator=(const DisEventReport& aRhs)
{
   if (this != &aRhs)
   {
      DisPdu::operator=(aRhs);
      Copy(aRhs);
   }
   return *this;
}

// virtual
const DisEntityId& DisEventReport::GetReceivingEntity() const
{
   return mReceivingEntity;
}

// virtual
const DisEntityId& DisEventReport::GetOriginatingEntity() const
{
   return mOriginatingEntity;
}

void DisEventReport::SetData(const DisDatumSpec& aData)
{
   mData = aData;
   GetLength();
}

// virtual
void DisEventReport::SetOriginatingEntity(const DisEntityId& aEntityId)
{
   mOriginatingEntity = aEntityId;
}

// virtual
void DisEventReport::SetReceivingEntity(const DisEntityId& aEntityId)
{
   mReceivingEntity = aEntityId;
}

// virtual
void DisEventReport::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);
   aStream << "      => Event Type         = " << mEventType << std::endl;
   mData.Stream(aStream);
   aStream << "      === End DisEventReport ===" << std::endl << std::endl;
}
