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

#include "DisDataQueryR.hpp"

#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "math.h"


DisDataQueryR::DisDataQueryR()
   : DisPdu()
   , mOriginatingEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mReceivingEntityId(DisEntityId::ENTITY_ID_UNKNOWN)
   , mRequestId(0)
   , mTimeInterval(0)
{
   SetPduType(DisEnum::Pdu::Type::DataQueryR);
   SetProtocolFamily(DisEnum::Pdu::Family::SimulationManagementWithReliability);
}

DisDataQueryR::DisDataQueryR(DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisDataQueryR::DisDataQueryR(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisDataQueryR::~DisDataQueryR() {}

DisPdu* DisDataQueryR::Clone() const
{
   return new DisDataQueryR(*this);
}

// virtual
int DisDataQueryR::GetClass() const
{
   return (DisEnum::Pdu::Type::DataQueryR);
}

// virtual
const DisEntityId& DisDataQueryR::GetOriginatingEntity() const
{
   return mOriginatingEntityId;
}

// virtual
void DisDataQueryR::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

// virtual
DisUint16 DisDataQueryR::GetLength()
{
   DisUint16 octetCount = GetBaseLength() + 24 + mDatumSpec.GetLength();
   SetLength(octetCount);
   return octetCount;
}

// private
void DisDataQueryR::GetMemberData(GenI& aGenI)
{
   DisUint8  padding;
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mOriginatingEntityId;
   aGenI >> mReceivingEntityId;
   aGenI >> mRequiredReliabilityService;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> padding;
   aGenI >> mRequestId;

   aGenI >> mTimeInterval;

   aGenI >> mDatumSpec;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

// virtual
void DisDataQueryR::Put(GenO& aGenO)
{
   DisUint8 padding = 0;
   GetLength(); // Ensure length is up-to-date

   DisPdu::Put(aGenO);

   aGenO << mOriginatingEntityId;
   aGenO << mReceivingEntityId;
   aGenO << mRequiredReliabilityService;
   aGenO << padding;
   aGenO << padding;
   aGenO << padding;
   aGenO << mRequestId;

   aGenO << mTimeInterval;

   aGenO << mDatumSpec;
}
