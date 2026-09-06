// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DisExpendableDescriptorRecord.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisExpendableDescriptorRecord::DisExpendableDescriptorRecord()
   : DisDescriptorBaseRecord()
{
}

DisExpendableDescriptorRecord::DisExpendableDescriptorRecord(const DisExpendableDescriptorRecord& aSrc)
   : DisDescriptorBaseRecord(aSrc)
{
}

DisExpendableDescriptorRecord::DisExpendableDescriptorRecord(GenI& aGenI)
   : DisDescriptorBaseRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisExpendableDescriptorRecord::GetLength() const
{
   return DisDescriptorBaseRecord::GetLength() + cMIN_RECORD_LENGTH_OCTETS;
}

void DisExpendableDescriptorRecord::Get(GenI& aGenI)
{
   DisDescriptorBaseRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisExpendableDescriptorRecord::Put(GenO& aGenO) const
{
   DisDescriptorBaseRecord::Put(aGenO);
   aGenO << mPadding;
}

bool DisExpendableDescriptorRecord::IsValid() const
{
   return DisDescriptorBaseRecord::IsValid();
}

std::string DisExpendableDescriptorRecord::ToString() const
{
   return DisDescriptorBaseRecord::ToString();
}

void DisExpendableDescriptorRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mPadding;
}
