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

#include "DisSilentAggregateSystem.hpp"

#include "DisValidationUtils.hpp"

DisSilentAggregateSystem::DisSilentAggregateSystem(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisSilentAggregateSystem::DisSilentAggregateSystem(const DisSilentAggregateSystem& aSrc)
   : mNumberOfAggregates(aSrc.mNumberOfAggregates)
   , mAggregateSystem(aSrc.mAggregateSystem)
{
}

void DisSilentAggregateSystem::Get(GenI& aGenI)
{
   GetMemberData(aGenI);
}

DisUint16 DisSilentAggregateSystem::GetLength()
{
   return cBASE_RECORD_LENGTH_IN_OCTETS + mAggregateSystem.GetLength();
}

void DisSilentAggregateSystem::Put(GenO& aGenO) const
{
   aGenO << mNumberOfAggregates;
   aGenO << mPadding;
   mAggregateSystem.Put(aGenO);
}

bool DisSilentAggregateSystem::IsValid() const
{
   return mAggregateSystem.IsValid();
}

DisSilentAggregateSystem* DisSilentAggregateSystem::Clone() const
{
   return new DisSilentAggregateSystem(*this);
}

void DisSilentAggregateSystem::Stream(std::ostream& aStream, const std::string& aSpacing)
{
   aStream << ToString() << '\n';
}

std::string DisSilentAggregateSystem::ToString()
{
   std::string returnString = "-------Silent Aggregate System-------\n";
   returnString += "Number of aggregate systems: " + std::to_string(mNumberOfAggregates) + "\n";
   returnString += "---------- Aggregate System ----------\n";
   returnString += mAggregateSystem.ToString();
   returnString += "-------- End Aggregate System --------\n";
   returnString += "-----End Silent Aggregate System-----\n";
   return returnString;
}

DisUint16 DisSilentAggregateSystem::GetNumberOfAggregates() const
{
   return mNumberOfAggregates;
}

DisAggregateTypeRecord DisSilentAggregateSystem::GetAggregateSystem() const
{
   return mAggregateSystem;
}

void DisSilentAggregateSystem::SetNumberOfAggregates(DisUint16 aNumberofAggregates)
{
   mNumberOfAggregates = aNumberofAggregates;
}

void DisSilentAggregateSystem::SetAggregateSystem(const DisAggregateTypeRecord& aAggregateSytem)
{
   mAggregateSystem = aAggregateSytem;
}

void DisSilentAggregateSystem::GetMemberData(GenI& aGenI)
{
   aGenI >> mNumberOfAggregates;
   aGenI >> mPadding;
   mAggregateSystem.Get(aGenI);
}

// Operators
bool DisSilentAggregateSystem::operator==(const DisSilentAggregateSystem& aRhs) const
{
   return ((mNumberOfAggregates == aRhs.mNumberOfAggregates) && (mAggregateSystem == aRhs.mAggregateSystem));
}
