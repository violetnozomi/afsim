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

#include "DisRepairResponse.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisRepairResponse::DisRepairResponse()
   : DisPdu()
   , mRequestingEntityId()
   , mServicingEntityId()
   , mResult(0)
{
   SetPduType(DisEnum::Pdu::Type::RepairResponse);
   SetProtocolFamily(DisEnum::Pdu::Family::Logistics);
   SetLength(28); // 224 bits
}

DisRepairResponse::DisRepairResponse(const DisRepairResponse& aSrc)
   : DisPdu(aSrc)
   , mRequestingEntityId(aSrc.mRequestingEntityId)
   , mServicingEntityId(aSrc.mServicingEntityId)
   , mResult(aSrc.mResult)
{
}

DisRepairResponse::DisRepairResponse(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisRepairResponse::DisRepairResponse(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

DisRepairResponse::~DisRepairResponse() {}

// virtual
DisPdu* DisRepairResponse::Clone() const
{
   return new DisRepairResponse(*this);
}

// virtual
const DisEntityId& DisRepairResponse::GetOriginatingEntity() const
{
   return mRequestingEntityId;
}

int DisRepairResponse::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RepairResponse;
}

// Virtual
void DisRepairResponse::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisRepairResponse::GetMemberData(GenI& aGenI)
{
   DisUint8  dummy8       = 0;
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();

   aGenI >> mRequestingEntityId;
   aGenI >> mServicingEntityId;
   aGenI >> mResult;
   aGenI >> dummy8;
   aGenI >> dummy8;
   aGenI >> dummy8;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisRepairResponse::Put(GenO& aGenO)
{
   DisUint8 dummy8 = 0;
   GetLength(); // Assure length is up to date.
   DisPdu::Put(aGenO);
   aGenO << mRequestingEntityId;
   aGenO << mServicingEntityId;
   aGenO << mResult;
   aGenO << dummy8;
   aGenO << dummy8;
   aGenO << dummy8;
}

// virtual
void DisRepairResponse::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      RequestEntityId = " << GetRequestingEntityId() << std::endl;
   aStream << "      ServicingEntity = " << GetServicingEntityId() << std::endl;

   aStream << "      Repair Result = " << static_cast<int>(mResult) << std::endl;
   aStream << "      === End DisRepairResponse ===" << std::endl << std::endl;
}
