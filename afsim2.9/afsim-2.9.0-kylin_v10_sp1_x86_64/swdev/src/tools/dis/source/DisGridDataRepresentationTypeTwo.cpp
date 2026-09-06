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

#include "DisGridDataRepresentationTypeTwo.hpp"

#include <cassert>

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGridDataRepresentationTypeTwo::DisGridDataRepresentationTypeTwo()
   : DisGridDataRepresentationType()
{
}

DisGridDataRepresentationTypeTwo::DisGridDataRepresentationTypeTwo(const DisGridDataRepresentationTypeTwo& aSrc)
   : DisGridDataRepresentationType(aSrc)
{
   Copy(aSrc);
}

DisGridDataRepresentationTypeTwo::DisGridDataRepresentationTypeTwo(GenI& aGenI)
   : DisGridDataRepresentationType(aGenI)
{
   GetMemberData(aGenI);
}

void DisGridDataRepresentationTypeTwo::Get(GenI& aGenI)
{
   DisGridDataRepresentationType::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGridDataRepresentationTypeTwo::GetLength() const
{
   DisUint16 lengthInOctets = DisGridDataRepresentationType::GetLength() + cMIN_LENGTH_IN_OCTETS;
   lengthInOctets += mNumberOfValues * 4;
   return lengthInOctets;
}

void DisGridDataRepresentationTypeTwo::Put(GenO& aGenO) const
{
   DisGridDataRepresentationType::Put(aGenO);
   aGenO << mPadding;
   for (int i = 0; i < mNumberOfValues; i++)
   {
      aGenO << mDataValues[i];
   }
}

std::vector<DisFloat32> DisGridDataRepresentationTypeTwo::GetDataValues() const
{
   return mDataValues;
}

void DisGridDataRepresentationTypeTwo::SetDataValues(std::vector<DisFloat32> aDataValues)
{
   mDataValues = aDataValues;
}

void DisGridDataRepresentationTypeTwo::AddDataValue(DisFloat32 aDataValue)
{
   mDataValues.push_back(aDataValue);
   mNumberOfValues++;
}

bool DisGridDataRepresentationTypeTwo::IsValid() const
{
   DisUint16 total_bits = GetLength() * 16;
   if (total_bits % 16 > 0)
   {
      return false;
   }
   return true;
}

DisGridDataRepresentationTypeTwo* DisGridDataRepresentationTypeTwo::Clone() const
{
   return new DisGridDataRepresentationTypeTwo(*this);
}

std::string DisGridDataRepresentationTypeTwo::ToString() const
{
   std::string returnString = DisGridDataRepresentationType::ToString();
   returnString += "------- Dis GridDataRecordType Two -------\n";
   returnString += "Data: ";
   for (int i = 0; i < mNumberOfValues; i++)
   {
      returnString += std::to_string(mDataValues[i]);
   }
   returnString += '\n';
   returnString += "----- End Dis GridDataRecordType Two -----\n";
   return returnString;
}

void DisGridDataRepresentationTypeTwo::Copy(const DisGridDataRepresentationTypeTwo& aSrc)
{
   DisGridDataRepresentationType::Copy(aSrc);
   if (aSrc.mNumberOfValues > 0)
   {
      mDataValues = aSrc.mDataValues;
   }
}

void DisGridDataRepresentationTypeTwo::GetMemberData(GenI& aGenI)
{
   aGenI >> mPadding;
   for (int i = 0; i < mNumberOfValues; i++)
   {
      DisFloat32 tempValue = 0;
      aGenI >> tempValue;
      mDataValues[i] = tempValue;
   }
}
