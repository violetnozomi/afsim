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

#include "DisDataQuery.hpp"

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "math.h"

DisDataQuery::DisDataQuery()
   : DisPdu()
   , mOriginatingEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mTimeInterval(0)
{
   SetPduType(DisEnum::Pdu::Type::DataQuery);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagement);
}

DisDataQuery::DisDataQuery(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisDataQuery::DisDataQuery(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisDataQuery::~DisDataQuery() {}

DisPdu* DisDataQuery::Clone() const
{
   return new DisDataQuery(*this);
}

// virtual
int DisDataQuery::GetClass() const
{
   return DisEnum::Pdu::Type::DataQuery;
}

// virtual
const DisEntityId& DisDataQuery::GetOriginatingEntity() const
{
   return mOriginatingEntityId;
}

// virtual
void DisDataQuery::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisDataQuery::GetLength()
{
   DisUint16 octetCount = GetBaseLength() + 20 + mDatumSpec.GetLength();
   SetLength(octetCount);
   return octetCount;
}

// private
void DisDataQuery::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntityId;
   aGenI >> mReceivingEntityId;
   aGenI >> mRequestId;

   aGenI >> mTimeInterval;

   aGenI >> mDatumSpec;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisDataQuery::Put(GenO& aGenO)
{
   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntityId;
   aGenO << mReceivingEntityId;
   aGenO << mRequestId;

   aGenO << mTimeInterval;

   aGenO << mDatumSpec;
}

bool DisDataQuery::IsValid() const
{
   bool isValid = true;
   // Perform data validation.
   if (!DisPdu::IsValid())
      isValid = false;
   if (!mOriginatingEntityId.IsValid())
      isValid = false;
   if (!mReceivingEntityId.IsValid())
      isValid = false;
   if (!mDatumSpec.IsValid())
      isValid = false;
   return isValid;
}
