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

#include "DisGridDataRepresentationTypeZero.hpp"

#include <cassert>

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGridDataRepresentationTypeZero::DisGridDataRepresentationTypeZero()
   : DisGridDataRepresentationType()
{
}

DisGridDataRepresentationTypeZero::DisGridDataRepresentationTypeZero(const DisGridDataRepresentationTypeZero& aSrc)
   : DisGridDataRepresentationType(aSrc)
{
   Copy(aSrc);
}

DisGridDataRepresentationTypeZero::DisGridDataRepresentationTypeZero(GenI& aGenI)
   : DisGridDataRepresentationType(aGenI)
{
   GetMemberData(aGenI);
}

void DisGridDataRepresentationTypeZero::Get(GenI& aGenI)
{
   DisGridDataRepresentationType::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisGridDataRepresentationTypeZero::GetLength() const
{
   DisUint16 lengthInOctets = DisGridDataRepresentationType::GetLength() + cMIN_LENGTH_IN_OCTETS;
   lengthInOctets += mNumberOfValues + (mNumberOfValues % 2);
   return lengthInOctets;
}

void DisGridDataRepresentationTypeZero::Put(GenO& aGenO) const
{
   DisGridDataRepresentationType::Put(aGenO);
   if (mNumberOfValues > 0)
   {
      for (int i = 0; i < mNumberOfValues; i++)
      {
         aGenO << mDataValues[i];
      }

      if (mNumberOfValues % 2 != 0)
      {
         aGenO << mPadding;
      }
   }
}

std::vector<DisUint8> DisGridDataRepresentationTypeZero::GetDataValues() const
{
   return mDataValues;
}

void DisGridDataRepresentationTypeZero::SetDataValues(std::vector<DisUint8> aDataValues)
{
   mDataValues = aDataValues;
}

void DisGridDataRepresentationTypeZero::AddDataValue(DisUint8 aDataValue)
{
   mDataValues.push_back(aDataValue);
   mNumberOfValues++;
}


bool DisGridDataRepresentationTypeZero::IsValid() const
{
   DisUint16 total_bits = GetLength() * 8;
   return total_bits % 16 == 0;
}

DisGridDataRepresentationTypeZero* DisGridDataRepresentationTypeZero::Clone() const
{
   return new DisGridDataRepresentationTypeZero(*this);
}

std::string DisGridDataRepresentationTypeZero::ToString() const
{
   std::string returnString = DisGridDataRepresentationType::ToString();
   returnString += "------- Dis GridDataRecordType Zero -------\n";
   returnString += "Number of Octets:         " + std::to_string(mNumberOfValues);
   returnString += "Data: ";
   for (int i = 0; i < (mNumberOfValues / 8); i++)
   {
      returnString += std::to_string(mDataValues[i]);
   }
   returnString += '\n';
   returnString += "----- End Dis GridDataRecordType Zero -----\n";
   return returnString;
}

void DisGridDataRepresentationTypeZero::Copy(const DisGridDataRepresentationTypeZero& aSrc)
{
   DisGridDataRepresentationType::Copy(aSrc);
   if (aSrc.mNumberOfValues > 0)
   {
      mDataValues = aSrc.mDataValues;
   }
}

void DisGridDataRepresentationTypeZero::GetMemberData(GenI& aGenI)
{
   if (mNumberOfValues > 0)
   {
      for (int i = 0; i < mNumberOfValues; i++)
      {
         DisUint8 tempValue = 0;
         aGenI >> tempValue;
         mDataValues[i] = tempValue;
      }
      if (mNumberOfValues % 2 != 0)
      {
         aGenI >> mPadding;
      }
   }
}

DisUint16 DisGridDataRepresentationTypeZero::CalculateNumberOfPaddingOctets() const
{
   return static_cast<DisUint16>(std::ceil(mNumberOfValues / 2) * 2 - mNumberOfValues);
}
