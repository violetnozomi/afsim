// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtAzElTable.hpp"

#include <cmath>

#include "UtMath.hpp"

//! The following is a convenience method to create a minimal
//! table that contains a constant value.
void UtAzElTable::CreateConstantTable(float aValue)
{
   mIsConstant = true;

   mAzValues.SetValues({static_cast<float>(-UtMath::cPI), static_cast<float>(UtMath::cPI)});
   mElValues.SetValues({static_cast<float>(-UtMath::cPI_OVER_2), static_cast<float>(UtMath::cPI_OVER_2)});

   mDataValues.Resize(2, 2);
   mDataValues.Set(aValue, 0, 0);
   mDataValues.Set(aValue, 0, 1);
   mDataValues.Set(aValue, 1, 0);
   mDataValues.Set(aValue, 1, 1);

   Validate(2, 2); // Just for completeness (ensures the table context is built)
}

//! Convert each data value according to the formula:
//!
//! new = aReference * pow(10.0, old / 10.0);
void UtAzElTable::ConvertValuesFromDB(float /*aReference*/)
{
   size_t azCount = mAzValues.GetSize();
   size_t elCount = mElValues.GetSize();
   for (size_t azIndex = 0; azIndex < azCount; ++azIndex)
   {
      for (size_t elIndex = 0; elIndex < elCount; ++elIndex)
      {
         float oldValue = mDataValues.Get(azIndex, elIndex);
         float newValue = static_cast<float>(pow(10.0, oldValue / 10.0));
         mDataValues.Set(newValue, azIndex, elIndex);
      }
   }
}

//! Convert each data value according to the formula:
//!
//! new = 10.0 * log10(old / aReference);
void UtAzElTable::ConvertValuesToDB(float aReference)
{
   size_t azCount = mAzValues.GetSize();
   size_t elCount = mElValues.GetSize();
   for (size_t azIndex = 0; azIndex < azCount; ++azIndex)
   {
      for (size_t elIndex = 0; elIndex < elCount; ++elIndex)
      {
         float oldValue = mDataValues.Get(azIndex, elIndex);
         float newValue = static_cast<float>(10.0 * log10(oldValue / aReference));
         mDataValues.Set(newValue, azIndex, elIndex);
      }
   }
}

//! Set the interpolation flag in the context.
void UtAzElTable::SetInterpolationFlag(bool aInterpolate)
{
   mContext.SetInterpolate(aInterpolate);
}

//! Get the interpolation flag from the context
bool UtAzElTable::GetInterpolationFlag() const
{
   return mContext.Interpolate();
}

//! Set the Interpolation type in the context.
void UtAzElTable::SetInterpolationType(InterpolationType aInterpolationType)
{
   mContext.SetInterpolationType(aInterpolationType);
}

//! Get the Interpolation type from the context.
ut::azel::InterpolationType UtAzElTable::GetInterpolationType() const
{
   return mContext.GetInterpolationType();
}

//! Convert each data value according to the formula:
//!
//!   new = old * aMultiplier;
void UtAzElTable::MultiplyValues(float aMultiplier)
{
   size_t azCount = mAzValues.GetSize();
   size_t elCount = mElValues.GetSize();
   for (size_t azIndex = 0; azIndex < azCount; ++azIndex)
   {
      for (size_t elIndex = 0; elIndex < elCount; ++elIndex)
      {
         float oldValue = mDataValues.Get(azIndex, elIndex);
         float newValue = oldValue * aMultiplier;
         mDataValues.Set(newValue, azIndex, elIndex);
      }
   }
}

void UtAzElTable::SetAzValues(float aMinvalue, float aMaxValue, size_t aNumValues)
{
   mContext.SetEqualInterval(true);
   mAzValuesE.SetValues(aMinvalue, aMaxValue, aNumValues);
}

void UtAzElTable::SetElValues(float aMinvalue, float aMaxValue, size_t aNumValues)
{
   mContext.SetEqualInterval(true);
   mElValuesE.SetValues(aMinvalue, aMaxValue, aNumValues);
}

//! Ensure the table contains valid data.  This should ALWAYS be called right
//! after loading the data into the table.
//!
//! @returns 'true' if everything is OK, otherwise 'false'.
bool UtAzElTable::Validate(size_t aAzCount, size_t aElCount)
{
   bool   ok      = false;
   size_t azCount = 0;
   size_t elCount = 0;
   if (mContext.IsEqualInterval())
   {
      azCount = mAzValuesE.GetSize();
      elCount = mElValuesE.GetSize();
   }
   else
   {
      azCount = mAzValues.GetSize();
      elCount = mElValues.GetSize();
   }

   if ((azCount >= 2) && (elCount >= 2) && (azCount == aAzCount) && (elCount == aElCount) &&
       (mElValues.Get(0) >= -(UtMath::cPI_OVER_2 + 1.0E-6)) &&
       (mElValues.Get(elCount - 1) <= (UtMath::cPI_OVER_2 + 1.0E-6)) && (mAzValues.Get(0) >= -(UtMath::cPI + 1.0E-6)) &&
       (mAzValues.Get(azCount - 1) <= (UtMath::cPI + 1.0E-6)))
   {
      ok = true;
      size_t i;
      for (i = 1; i < elCount; ++i)
      {
         if (mElValues.Get(i) <= mElValues.Get(i - 1))
         {
            ok = false;
         }
      }
      for (i = 1; i < azCount; ++i)
      {
         if (mAzValues.Get(i) <= mAzValues.Get(i - 1))
         {
            ok = false;
         }
      }
   }
   mContext.SetTable(this); // Point the lookup context at this table.
   return ok;
}
