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

#include "DisDescriptorBaseRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisDescriptorBaseRecord::DisDescriptorBaseRecord(const DisDescriptorBaseRecord& aSrc)
   : mEntityType(aSrc.mEntityType)
{
}

DisDescriptorBaseRecord::DisDescriptorBaseRecord(GenI& aGenI)
   : mEntityType()
{
   GetMemberData(aGenI);
}

DisUint16 DisDescriptorBaseRecord::GetLength() const
{
   return cBASE_RECORD_LENGTH;
}

void DisDescriptorBaseRecord::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisDescriptorBaseRecord::GetMemberData(GenI& aGenI)
{
   mEntityType.Get(aGenI);
}

void DisDescriptorBaseRecord::Put(GenO& aGenO) const
{
   mEntityType.Put(aGenO);
}

bool DisDescriptorBaseRecord::IsValid() const
{
   return mEntityType.IsValid();
}

DisDescriptorBaseRecord& DisDescriptorBaseRecord::operator=(const DisDescriptorBaseRecord& aRhs)
{
   if (this != &aRhs)
   {
      mEntityType = aRhs.mEntityType;
   }
   return *this;
}

std::string DisDescriptorBaseRecord::ToString() const
{
   std::string returnString = "------DisDescriptorBaseRecord------\n";
   returnString += "Entity Type: " + mEntityType.ToString();
   returnString += "----End DisDescriptorBaseRecord----\n";
   return returnString;
}

void DisDescriptorBaseRecord::Stream(std::ostream& aStream)
{
   aStream << ToString() << '\n';
}
