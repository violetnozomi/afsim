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

#include "DisGridDataRepresentationTypeOne.hpp"

#include <cassert>

#include "Dis.hpp"
#include "GenI.hpp"
#include "GenO.hpp"

DisGridDataRepresentationTypeOne::DisGridDataRepresentationTypeOne(const DisGridDataRepresentationTypeOne& aSrc)
   : DisGridDataRepresentationType(aSrc)
   , mFieldScale(aSrc.mFieldScale)
   , mFieldOffset(aSrc.mFieldOffset)
{
   Copy(aSrc);
}

DisGridDataRepresentationTypeOne::DisGridDataRepresentationTypeOne(GenI& aGenI)
{
   GetMemberData(aGenI);
}

void DisGridDataRepresentationTypeOne::Get(GenI& aGenI)
{
   // Parent class Get method is called inside GetMemberData this time to prevent data ordering issues
   GetMemberData(aGenI);
}

DisUint16 DisGridDataRepresentationTypeOne::GetLength() const
{
   DisUint16 lengthInOctets = DisGridDataRepresentationType::GetLength() + cMIN_LENGTH_IN_OCTETS;
   lengthInOctets += mNumberOfValues * 2 + (CalculateNumberOfPaddingOctets());
   return lengthInOctets;
}

void DisGridDataRepresentationTypeOne::Put(GenO& aGenO) const
{
   aGenO << mFieldScale;
   aGenO << mFieldOffset;
   // Put for parent class has been put here to maintain variable placement consistency with spec
   DisGridDataRepresentationType::Put(aGenO);
   if (mNumberOfValues > 0)
   {
      for (int i = 0; i < mNumberOfValues; i++)
      {
         aGenO << mDataValues[i];
      }

      DisUint8 tempPadding = 0;
      for (int j = 0; j < CalculateNumberOfPaddingOctets(); j++)
      {
         aGenO << tempPadding;
      }
   }
}

std::vector<DisUint16> DisGridDataRepresentationTypeOne::GetDataValues() const
{
   return mDataValues;
}

DisFloat32 DisGridDataRepresentationTypeOne::GetFieldScale() const
{
   return mFieldScale;
}

DisFloat32 DisGridDataRepresentationTypeOne::GetFieldOffset() const
{
   return mFieldOffset;
}

void DisGridDataRepresentationTypeOne::SetDataValues(std::vector<DisUint16> aDataValues)
{
   mDataValues = aDataValues;
}

void DisGridDataRepresentationTypeOne::AddDataValue(DisUint16 aDataValue)
{
   mDataValues.push_back(aDataValue);
   mNumberOfValues++;
}

void DisGridDataRepresentationTypeOne::SetFieldScale(DisFloat32 aFieldScale)
{
   mFieldScale = aFieldScale;
}

void DisGridDataRepresentationTypeOne::SetFieldOffset(DisFloat32 aFieldOffset)
{
   mFieldOffset = aFieldOffset;
}

bool DisGridDataRepresentationTypeOne::IsValid() const
{
   DisUint16 total_bits = GetLength() * 16;
   if (total_bits % 16 > 0)
   {
      return false;
   }
   return true;
}

DisGridDataRepresentationTypeOne* DisGridDataRepresentationTypeOne::Clone() const
{
   return new DisGridDataRepresentationTypeOne(*this);
}

std::string DisGridDataRepresentationTypeOne::ToString() const
{
   std::string returnString = DisGridDataRepresentationType::ToString();
   returnString += "------- Dis GridDataRecordTypeOne -------\n";
   returnString += "Field Scale:      " + std::to_string(mFieldScale) + '\n';
   returnString += "Field Offset:     " + std::to_string(mFieldOffset) + '\n';
   returnString += "Data: ";
   for (int i = 0; i < mNumberOfValues; i++)
   {
      returnString += std::to_string(mDataValues[i]);
   }
   returnString += '\n';
   returnString += "----- End Dis GridDataRecordTypeOne -----\n";
   return returnString;
}

void DisGridDataRepresentationTypeOne::Copy(const DisGridDataRepresentationTypeOne& aSrc)
{
   DisGridDataRepresentationType::Copy(aSrc);
   mFieldScale  = aSrc.mFieldScale;
   mFieldOffset = aSrc.mFieldOffset;
   mDataValues  = aSrc.mDataValues;
}

void DisGridDataRepresentationTypeOne::GetMemberData(GenI& aGenI)
{
   aGenI >> mFieldScale;
   aGenI >> mFieldOffset;
   // Get for parent class has been put here to maintain variable placement consistency with spec
   DisGridDataRepresentationType::Get(aGenI);
   if (mNumberOfValues > 0)
   {
      for (int i = 0; i < mNumberOfValues; i++)
      {
         aGenI >> mDataValues[i];
      }

      DisUint8 tempPadding = 0;
      for (int j = 0; j < CalculateNumberOfPaddingOctets(); j++)
      {
         aGenI >> tempPadding;
      }
   }
}

DisUint16 DisGridDataRepresentationTypeOne::CalculateNumberOfPaddingOctets() const
{
   return static_cast<DisUint16>(2 * (std::ceil((mNumberOfValues + 1) / 2) * 2 - (mNumberOfValues + 1)));
}
