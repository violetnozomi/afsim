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

#include "DisRepairComplete.hpp"

#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

namespace
{
const DisUint16 sDisRepairCompleteSize = (48 + 48 + (16 + 16)) / 8;
}

DisRepairComplete::DisRepairComplete()
   : DisPdu()
   , mRequestingEntityId()
   , mServicingEntityId()
   , mRepair(0)
{
   SetPduType(DisEnum::Pdu::Type::RepairComplete);
   SetProtocolFamily(DisEnum::Pdu::Family::Logistics);
   SetLength(DisPdu::GetBaseLength() + sDisRepairCompleteSize);
}

DisRepairComplete::DisRepairComplete(const DisRepairComplete& aSrc)
   : DisPdu(aSrc)
   , mRequestingEntityId(aSrc.mRequestingEntityId)
   , mServicingEntityId(aSrc.mServicingEntityId)
   , mRepair(aSrc.mRepair)
{
}

DisRepairComplete::DisRepairComplete(const DisPdu& aPdu, GenI& aGenI)
   : DisPdu(aPdu)
{
   GetMemberData(aGenI);
}

DisRepairComplete::DisRepairComplete(GenI& aGenI)
   : DisPdu(aGenI)
{
   GetMemberData(aGenI);
}

// virtual
DisPdu* DisRepairComplete::Clone() const
{
   return new DisRepairComplete(*this);
}

// virtual
const DisEntityId& DisRepairComplete::GetOriginatingEntity() const
{
   return mServicingEntityId;
}

int DisRepairComplete::GetClass() const
{
   return (int)DisEnum::Pdu::Type::RepairComplete;
}

// Virtual
void DisRepairComplete::Get(GenI& aGenI)
{
   DisPdu::Get(aGenI);
   GetMemberData(aGenI);
}

void DisRepairComplete::GetMemberData(GenI& aGenI)
{
   DisUint16 lengthToRead = DisPdu::GetLengthToRead();
   DisUint16 pad16        = 0;
   aGenI >> mRequestingEntityId;
   aGenI >> mServicingEntityId;
   aGenI >> mRepair;
   aGenI >> pad16;

   // Skip 'extra' data
   DisUint16 lengthRead = GetLength();
   ReadExtraData(aGenI, lengthToRead - lengthRead);
}

void DisRepairComplete::Put(GenO& aGenO)
{
   DisUint16 pad16 = 0;
   GetLength(); // Ensure length is up-to-date
   DisPdu::Put(aGenO);
   aGenO << mRequestingEntityId;
   aGenO << mServicingEntityId;
   aGenO << mRepair;
   aGenO << pad16;
}

// virtual
void DisRepairComplete::Stream(std::ostream& aStream) const
{
   DisPdu::Stream(aStream);

   aStream << "      RequestEntityId = " << GetRequestingEntityId() << std::endl;
   aStream << "      ServicingEntity = " << GetServicingEntityId() << std::endl;

   aStream << "      => Repair = " << static_cast<int>(mRepair) << std::endl;
   aStream << "      === End DisRepairComplete ===" << std::endl << std::endl;
}
