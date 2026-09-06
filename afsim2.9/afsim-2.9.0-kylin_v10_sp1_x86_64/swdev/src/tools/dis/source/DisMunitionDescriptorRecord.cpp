// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// *****************************************************************************

#include "DisMunitionDescriptorRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisMunitionDescriptorRecord::DisMunitionDescriptorRecord()
   : DisDescriptorBaseRecord()
{
}

DisMunitionDescriptorRecord::DisMunitionDescriptorRecord(const DisMunitionDescriptorRecord& aSrc)
   : DisDescriptorBaseRecord(aSrc)
   , mWarhead(aSrc.mWarhead)
   , mFuse(aSrc.mFuse)
   , mQuantity(aSrc.mQuantity)
   , mRate(aSrc.mRate)
{
}

DisMunitionDescriptorRecord::DisMunitionDescriptorRecord(GenI& aGenI)
   : DisDescriptorBaseRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisMunitionDescriptorRecord::GetLength() const
{
   return DisDescriptorBaseRecord::GetLength() + cMIN_RECORD_LENGTH;
}

void DisMunitionDescriptorRecord::Get(GenI& aGenI)
{
   DisDescriptorBaseRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisMunitionDescriptorRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mWarhead;
   aGenI >> mFuse;
   aGenI >> mQuantity;
   aGenI >> mRate;
}

void DisMunitionDescriptorRecord::Put(GenO& aGenO) const
{
   DisDescriptorBaseRecord::Put(aGenO);
   aGenO << mWarhead;
   aGenO << mFuse;
   aGenO << mQuantity;
   aGenO << mRate;
}

bool DisMunitionDescriptorRecord::IsValid() const
{
   return DisDescriptorBaseRecord::IsValid();
}

std::string DisMunitionDescriptorRecord::ToString() const
{
   std::string returnString = DisDescriptorBaseRecord::ToString();
   returnString += "----- Munition Descriptor Record -----\n";
   returnString += "Warhead  = " + std::to_string(mWarhead) + "\n";
   returnString += "Fuse     = " + std::to_string(mFuse) + "\n";
   returnString += "Quantity =   " + std::to_string(mQuantity) + "\n";
   returnString += "Rate     = " + std::to_string(mRate) + "\n";
   returnString += "--- End Munition Descriptor Record ---\n";
   return returnString;
}
