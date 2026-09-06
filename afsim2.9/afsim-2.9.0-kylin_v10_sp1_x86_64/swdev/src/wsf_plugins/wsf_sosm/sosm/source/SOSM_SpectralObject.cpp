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

#include "SOSM_SpectralObject.hpp"

#include <algorithm>
#include <cmath>

// =================================================================================================
//! Return the string associated with the supplied status value.
//! @param aStatus The Status enumeration value whose string value is to be returned.
//! @returns The string associated with the supplied Status value.
// static
std::string SOSM_SpectralObject::StatusString(Status aStatus)
{
   std::string statusString;
   switch (aStatus)
   {
   case cOK:
      statusString = "Success";
      break;
   case cINCOMPATIBLE_INCREMENTS:
      statusString = "Incompatible increments";
      break;
   case cINCOMPATIBLE_ORIGINS:
      statusString = "Incompatible origins";
      break;
   default:
      statusString = "Unknown error";
      break;
   }
   return statusString;
}

// =================================================================================================
//! Compute the union of this and another spectral object.
//!
//! This method assigns 'this' to be the maximum spectral extent (the union) of this and the
//! supplied object.
//!
//! @param aObject The other spectral object.
//! @returns a Status indicating the success or failure of the union.
SOSM_SpectralObject::Status SOSM_SpectralObject::Union(const SOSM_SpectralObject& aObject)
{
   return Union(*this, aObject);
}

// =================================================================================================
//! Determine the union of two spectral objects.
//!
//! This method updates 'this' to be the maximum spectral extent of two supplied objects.
//! The two objects must have the same increment and must have compatible origins.
//!
//! @param aObject1 The first spectral  object.
//! @param aObject2 The second spectral object.
//! @returns a Status indicating the success or failure of the union.
//! @note It is safe to pass the result object as either of the arguments.
SOSM_SpectralObject::Status SOSM_SpectralObject::Union(const SOSM_SpectralObject& aObject1,
                                                       const SOSM_SpectralObject& aObject2)
{
   // If one of the objects is 'null' (no increment size), the union is just the limits of the other.
   // If both objects are 'null' then the union is null.
   if (aObject1.Increment() == 0.0F)
   {
      DefineSpectralLimits(aObject2);
      return cOK;
   }
   else if (aObject2.Increment() == 0.0F)
   {
      DefineSpectralLimits(aObject1);
      return cOK;
   }

   // Make sure the objects have compatible increment.
   if (aObject1.Increment() != aObject2.Increment())
   {
      return cINCOMPATIBLE_INCREMENTS;
   }

   mIncrement     = aObject1.Increment();
   double origin1 = aObject1.Origin();
   double origin2 = aObject2.Origin();

   Status status = cINCOMPATIBLE_ORIGINS;
   if (origin1 <= origin2)
   {
      // First object has the 'left-most' origin.
      // Determine the number of samples the origin of second object is offset from the origin of the first object.
      int offset = static_cast<int>((origin2 - origin1 + 0.01 * mIncrement) / mIncrement);
      // Ensure the distance between the origins is a multiple of the sampling increment.
      if (fabs((origin1 + offset * mIncrement) - origin2) < 0.01)
      {
         status  = cOK;
         mOrigin = static_cast<float>(origin1);
         mCount  = std::max(aObject1.Count(), offset + aObject2.Count());
      }
   }
   else
   {
      // Second object has the 'left-most' origin.
      // Determine the number of samples the origin of first object is offset from the origin of the second object.
      int offset = static_cast<int>((origin1 - origin2 + 0.01 * mIncrement) / mIncrement);
      // Ensure the distance between the origins is a multiple of the sampling increment.
      if (fabs((origin2 + offset * mIncrement) - origin1) < 0.01)
      {
         status  = cOK;
         mOrigin = static_cast<float>(origin2);
         mCount  = std::max(aObject2.Count(), offset + aObject1.Count());
      }
   }
   return status;
}

// =================================================================================================
// private
void SOSM_SpectralObject::UpdateWavelengthLimits()
{
   mLowerWavelength = 0.01F; // Assume a continuous spectrum
   mUpperWavelength = 10000.0F;
   if ((mOrigin > 0.0F) && (mIncrement > 0.0F) && (mCount > 1))
   {
      mUpperWavelength = 1.0E+4F / mOrigin;
      mLowerWavelength = 1.0E+4F / (mOrigin + (mCount - 1) * mIncrement);
   }
}
