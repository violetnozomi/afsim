// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "SOSM_Selector.hpp"

#include <algorithm>
#include <cmath>

#include "SOSM_SpectralObject.hpp"

// =================================================================================================
//! Default constructor
SOSM_Selector::SOSM_Selector()
   : mOrigin(0.0F)
   , mIncrement(0.0F)
   , mCount(0)
   , mInputStartIndex(0)
   , mOutputStartIndex(0)
   , mOutputCount(0)
{
}

// =================================================================================================
//! An alternative constructor used for creating selectors for testing.
SOSM_Selector::SOSM_Selector(const SOSM_SpectralObject& aInput)
   : mOrigin(aInput.Origin())
   , mIncrement(aInput.Increment())
   , mCount(aInput.Count())
   , mInputStartIndex(0)
   , mOutputStartIndex(0)
   , mOutputCount(aInput.Count())
{
}

// =================================================================================================
//! Return the string associated with the supplied status value.
//! @param aStatus The Status enumeration value whose string value is to be returned.
//! @returns The string associated with the supplied Status value.
// static
std::string SOSM_Selector::StatusString(Status aStatus)
{
   std::string statusString;
   switch (aStatus)
   {
   case cINTERSECTS:
      statusString = "Intersects";
      break;
   case cOUTPUT_IS_CONTINUOUS:
      statusString = "Output (sensor) must have a discrete spectral band";
      break;
   case cINCOMPATIBLE_INCREMENTS:
      statusString = "Incompatible increments";
      break;
   case cINCOMPATIBLE_ORIGINS:
      statusString = "Incompatible origins";
      break;
   case cNO_INTERSECTION:
      statusString = "No intersection";
      break;
   default:
      statusString = "Unknown error";
      break;
   }
   return statusString;
}

// =================================================================================================
//! Determine the intersection between two spectral objects.
//! @param aInput  The object providing the data. This will generally be an SOSM_Atmosphere or
//!                SOSM_Target object.
//! @param aOutput The object that will be receiving the data from the input object. This is
//!                generally the SOSM_Sensor object.
SOSM_Selector::Status SOSM_Selector::Intersect(const SOSM_SpectralObject& aInput, const SOSM_SpectralObject& aOutput)
{
   Status status = cINTERSECTS;
   mOrigin       = aOutput.Origin();
   mIncrement    = aOutput.Increment();
   mOutputCount  = aOutput.Count();

   if ((aOutput.Increment() <= 0.0F) || (aOutput.Count() <= 0))
   {
      // ERROR: output must have a discrete spectrum.
      status = cOUTPUT_IS_CONTINUOUS;
   }
   else if (aInput.Count() == 0)
   {
      // The input provides a continuous spectrum.

      mOrigin           = aOutput.Origin();
      mIncrement        = aOutput.Increment();
      mCount            = aOutput.Count();
      mInputStartIndex  = 0;
      mOutputStartIndex = 0;
   }
   else if (aInput.Increment() == aOutput.Increment())
   {
      // The input provides a discrete spectrum and the input and output have compatible increments.

      float inputOrigin  = aInput.Origin();
      float outputOrigin = aOutput.Origin();

      status = cINCOMPATIBLE_ORIGINS;
      mCount = -1;
      if (inputOrigin <= outputOrigin)
      {
         mOrigin           = outputOrigin;
         mOutputStartIndex = 0;
         mInputStartIndex  = static_cast<int>((outputOrigin - inputOrigin + 0.01F * mIncrement) / mIncrement);
         if (fabs((inputOrigin + mInputStartIndex * mIncrement) - outputOrigin) < 0.01F)
         {
            mCount = std::min(aOutput.Count(), aInput.Count() - mInputStartIndex);
            status = cNO_INTERSECTION; // Tentative
         }
      }
      else
      {
         mOrigin           = inputOrigin;
         mInputStartIndex  = 0;
         mOutputStartIndex = static_cast<int>((inputOrigin - outputOrigin + 0.01F * mIncrement) / mIncrement);
         if (fabs((outputOrigin + mOutputStartIndex * mIncrement) - inputOrigin) < 0.01F)
         {
            mCount = std::min(aInput.Count(), aOutput.Count() - mOutputStartIndex);
            status = cNO_INTERSECTION; // Tentative
         }
      }

      if ((status != cINCOMPATIBLE_ORIGINS) && (mCount > 0))
      {
         if (((mInputStartIndex + mCount) <= aInput.Count()) && ((mOutputStartIndex + mCount) <= aOutput.Count()))
         {
            status = cINTERSECTS;
         }
      }
   }
   else
   {
      // ERROR: The input provides a discrete spectrum but the input and output have incompatible increment.

      status = cINCOMPATIBLE_INCREMENTS;
   }

   if (status != cINTERSECTS)
   {
      mOrigin           = 0.0F;
      mIncrement        = 0.0F;
      mCount            = 0;
      mInputStartIndex  = 0;
      mOutputStartIndex = 0;
   }
   return status;
}

// =================================================================================================
void SOSM_Selector::InitializeOutput(std::vector<float>& aOutput, float aValue) const
{
   if (static_cast<int>(aOutput.size()) < mOutputCount)
   {
      aOutput.resize(mOutputCount);
   }

   // Fill unused areas at the front and the back that will not be filled by other mechanisms.

   int i;
   for (i = 0; i < mOutputStartIndex; ++i)
   {
      aOutput[i] = aValue;
   }
   for (i = mOutputStartIndex + mCount; i < mOutputCount; ++i)
   {
      aOutput[i] = aValue;
   }
}
