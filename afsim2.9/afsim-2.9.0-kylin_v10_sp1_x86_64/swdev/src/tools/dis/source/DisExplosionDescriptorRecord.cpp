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

#include "DisExplosionDescriptorRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisExplosionDescriptorRecord::DisExplosionDescriptorRecord()
   : DisDescriptorBaseRecord()
{
}

DisExplosionDescriptorRecord::DisExplosionDescriptorRecord(const DisExplosionDescriptorRecord& aSrc)
   : DisDescriptorBaseRecord(aSrc)
   , mExplosiveMaterial(aSrc.mExplosiveMaterial)
   , mExplosiveForce(aSrc.mExplosiveForce)
{
}

DisExplosionDescriptorRecord::DisExplosionDescriptorRecord(GenI& aGenI)
   : DisDescriptorBaseRecord(aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisExplosionDescriptorRecord::GetLength() const
{
   return DisDescriptorBaseRecord::GetLength() + cMIN_RECORD_LENGTH_OCTETS;
}

void DisExplosionDescriptorRecord::Get(GenI& aGenI)
{
   DisDescriptorBaseRecord::Get(aGenI);
   GetMemberData(aGenI);
}

void DisExplosionDescriptorRecord::Put(GenO& aGenO) const
{
   DisDescriptorBaseRecord::Put(aGenO);
   aGenO << mExplosiveMaterial;
   aGenO << mPadding;
   aGenO << mExplosiveForce;
}

std::string DisExplosionDescriptorRecord::ToString() const
{
   std::string returnString = DisDescriptorBaseRecord::ToString();
   returnString += "Explosive Material: " + std::to_string(mExplosiveMaterial);
   returnString += "Explosive Force:    " + std::to_string(mExplosiveForce);
   return returnString;
}

bool DisExplosionDescriptorRecord::IsValid() const
{
   return DisDescriptorBaseRecord::IsValid() && ValidateScalar(mExplosiveForce);
}

void DisExplosionDescriptorRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mExplosiveMaterial;
   aGenI >> mPadding;
   aGenI >> mExplosiveForce;
}
