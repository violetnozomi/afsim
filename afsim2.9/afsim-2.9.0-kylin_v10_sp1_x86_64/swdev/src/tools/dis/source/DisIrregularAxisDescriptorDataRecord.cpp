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

#include "DisIrregularAxisDescriptorDataRecord.hpp"

#include "DisValidationUtils.hpp"
#include "GenI.hpp"
#include "GenO.hpp"
#include "UtException.hpp"

DisIrregularAxisDescriptorDataRecord::DisIrregularAxisDescriptorDataRecord()
   : DisGridAxisDescriptorBaseRecord()
{
}

DisIrregularAxisDescriptorDataRecord::DisIrregularAxisDescriptorDataRecord(const DisIrregularAxisDescriptorDataRecord& aSrc)
   : DisGridAxisDescriptorBaseRecord(aSrc)
   , mNumberOfPointsOnAxis(aSrc.mNumberOfPointsOnAxis)
   , mInitialIndexX(aSrc.mInitialIndexX)
   , mCoordinateScale(aSrc.mCoordinateScale)
   , mCoordinateOffset(aSrc.mCoordinateOffset)
   , mXiNiValues(aSrc.mXiNiValues)
{
}

DisIrregularAxisDescriptorDataRecord::DisIrregularAxisDescriptorDataRecord(GenI& aGenI)
   : DisGridAxisDescriptorBaseRecord(aGenI)
{
   GetMemberData(aGenI);
}

void DisIrregularAxisDescriptorDataRecord::Get(GenI& aGenI)
{
   DisGridAxisDescriptorBaseRecord::Get(aGenI);
   GetMemberData(aGenI);
}

DisUint16 DisIrregularAxisDescriptorDataRecord::GetLength()
{
   DisUint16 length = DisGridAxisDescriptorBaseRecord::GetLength() + cMIN_LENGTH_IN_OCTETS;
   // Number of points where each value is two octets
   length += 2 * mNumberOfPointsOnAxis;
   length += CalculateNumberOfPaddingOctets();
   return length;
}

void DisIrregularAxisDescriptorDataRecord::Put(GenO& aGenO) const
{
   DisGridAxisDescriptorBaseRecord::Put(aGenO);
   aGenO << mNumberOfPointsOnAxis;
   aGenO << mInitialIndexX;
   aGenO << mCoordinateScale;
   aGenO << mCoordinateOffset;

   for (int i = 0; i < mNumberOfPointsOnAxis; i++)
   {
      aGenO << mXiNiValues[i];
   }
   /**
    * Get the number of padding octets and read them to the outbound stream
    */
   int      numberOfPaddingOctets = CalculateNumberOfPaddingOctets();
   DisUint8 padding               = 0;
   for (int i = 0; i < numberOfPaddingOctets; i++)
   {
      aGenO << padding; // padding size
   }
}

DisUint16 DisIrregularAxisDescriptorDataRecord::GetNumberOfPointsOnAxis() const
{
   return mNumberOfPointsOnAxis;
}

DisUint16 DisIrregularAxisDescriptorDataRecord::GetInitialIndexX() const
{
   return mInitialIndexX;
}

DisFloat64 DisIrregularAxisDescriptorDataRecord::GetCoordinateScale() const
{
   return mCoordinateScale;
}

DisFloat64 DisIrregularAxisDescriptorDataRecord::GetCoordinateOffset() const
{
   return mCoordinateOffset;
}

std::vector<DisUint16> DisIrregularAxisDescriptorDataRecord::GetXiNiValues() const
{
   return mXiNiValues;
}

DisUint16 DisIrregularAxisDescriptorDataRecord::GetValueAtIndex(int aIndex) const
{
   if (aIndex >= 0 || aIndex < static_cast<int>(mXiNiValues.size()))
   {
      return mXiNiValues[aIndex];
   }
   else
   {
      throw UtException("Index out of bounds");
   }
}

void DisIrregularAxisDescriptorDataRecord::SetNumberOfPointsOnAxis(DisUint16 aNumberOfPointsOnAxis)
{
   mNumberOfPointsOnAxis = aNumberOfPointsOnAxis;
}

void DisIrregularAxisDescriptorDataRecord::SetInitialIndexX(DisUint16 aInitialIndexX)
{
   mInitialIndexX = aInitialIndexX;
}

void DisIrregularAxisDescriptorDataRecord::SetCoordinateScale(DisFloat64 aCoordinateScale)
{
   mCoordinateScale = aCoordinateScale;
}

void DisIrregularAxisDescriptorDataRecord::SetCoordinateOffset(DisFloat64 aCoordinateOffset)
{
   mCoordinateOffset = aCoordinateOffset;
}

void DisIrregularAxisDescriptorDataRecord::AddXiNiValue(DisUint16 aXiNiValue)
{
   mXiNiValues.push_back(aXiNiValue);
   mNumberOfPointsOnAxis = static_cast<DisUint16>(mXiNiValues.size());
}

bool DisIrregularAxisDescriptorDataRecord::IsValid() const
{
   return DisGridAxisDescriptorBaseRecord::IsValid() && ValidateScalar(mCoordinateScale) &&
          ValidateScalar(mCoordinateOffset);
}

DisIrregularAxisDescriptorDataRecord* DisIrregularAxisDescriptorDataRecord::Clone() const
{
   return new DisIrregularAxisDescriptorDataRecord(*this);
}

std::string DisIrregularAxisDescriptorDataRecord::ToString() const
{
   std::string returnString = DisGridAxisDescriptorBaseRecord::ToString();
   returnString += "------Irregular Axis Descriptor Data Object-------\n";
   returnString += "Coordinate Scale:  " + std::to_string(mCoordinateScale) + '\n';
   returnString += "Coordinate Offset: " + std::to_string(mCoordinateOffset) + '\n';
   returnString += "Xi and Ni Values:  \n";

   for (int i = 0; i < mNumberOfPointsOnAxis; i++)
   {
      returnString += "Index " + std::to_string(i) + ": " + std::to_string(mXiNiValues[i]) + '\n';
   }

   returnString += "----End Irregular Axis Descriptor Data Object-----\n";
   return returnString;
}

DisUint16 DisIrregularAxisDescriptorDataRecord::CalculateNumberOfPaddingOctets() const
{
   return static_cast<DisUint16>(2 * (std::ceil(mNumberOfPointsOnAxis / 4) * 4 - mNumberOfPointsOnAxis));
}

void DisIrregularAxisDescriptorDataRecord::GetMemberData(GenI& aGenI)
{
   aGenI >> mNumberOfPointsOnAxis;
   aGenI >> mInitialIndexX;
   aGenI >> mCoordinateScale;
   aGenI >> mCoordinateOffset;
   mXiNiValues.clear();
   for (int i = 0; i < mNumberOfPointsOnAxis; i++)
   {
      DisUint16 currentValue = 0;
      aGenI >> currentValue;
      mXiNiValues.push_back(currentValue);
   }

   mNumberOfPointsOnAxis = static_cast<DisUint16>(mXiNiValues.size());
   /**
    * Get the number of padding octets remaining in the inbound stream then read
    * them in.
    */
   int      numberOfPaddingOctets = CalculateNumberOfPaddingOctets();
   DisUint8 padding               = 0;
   for (int i = 0; i < numberOfPaddingOctets; i++)
   {
      aGenI >> padding; // padding size
   }
}
