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

#include "DisGridDataRepresentationType.hpp"

#include "GenI.hpp"
#include "GenO.hpp"

DisGridDataRepresentationType::DisGridDataRepresentationType(DisUint16 aNumberOfValues)
   : mNumberOfValues(aNumberOfValues)
{
}

DisGridDataRepresentationType::DisGridDataRepresentationType(GenI& aGenI)
{
   Get(aGenI);
}

void DisGridDataRepresentationType::Get(GenI& aGenI)
{
   aGenI >> mNumberOfValues;
}

DisUint16 DisGridDataRepresentationType::GetLength() const
{
   return cBASE_LENGTH_IN_OCTETS;
}

void DisGridDataRepresentationType::Put(GenO& aGenO) const
{
   aGenO << mNumberOfValues;
}

DisUint16 DisGridDataRepresentationType::GetNumberOfValues() const
{
   return mNumberOfValues;
}

void DisGridDataRepresentationType::SetNumberOfValues(DisUint16 aNumberOfValues)
{
   mNumberOfValues = aNumberOfValues;
}

void DisGridDataRepresentationType::Stream(std::ostream& aStream) const
{
   aStream << ToString() << '\n';
}

std::string DisGridDataRepresentationType::ToString() const
{
   std::string returnString = "-------Dis Grid Data Base Record-------";
   returnString += "Number of Values: " + std::to_string(mNumberOfValues) + '\n';
   returnString += "-----End Dis Grid Data Base Record-----";
   return returnString;
}

void DisGridDataRepresentationType::Copy(const DisGridDataRepresentationType& aSrc)
{
   mNumberOfValues = aSrc.mNumberOfValues;
}

DisUint16 DisGridDataRepresentationType::CalculateNumberOfPaddingOctets() const
{
   return 0;
}
