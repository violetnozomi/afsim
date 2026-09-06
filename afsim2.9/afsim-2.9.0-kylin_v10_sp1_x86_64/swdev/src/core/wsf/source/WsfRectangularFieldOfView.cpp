// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfRectangularFieldOfView.hpp"

#include "UtInput.hpp"

// =================================================================================================
WsfRectangularFieldOfView::WsfRectangularFieldOfView()
   : WsfFieldOfView()
{
   SetType("rectangular");
}

// =================================================================================================
bool WsfRectangularFieldOfView::operator==(const WsfRectangularFieldOfView& aRhs) const
{
   return ((mMinAzFOV == aRhs.mMinAzFOV) && (mMaxAzFOV == aRhs.mMaxAzFOV) && (mMinElFOV == aRhs.mMinElFOV) &&
           (mMaxElFOV == aRhs.mMaxElFOV));
}

bool WsfRectangularFieldOfView::operator!=(const WsfRectangularFieldOfView& aRhs) const
{
   return !(*this == aRhs);
}

// =================================================================================================
// virtual
bool WsfRectangularFieldOfView::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "azimuth_field_of_view")
   {
      double minAzFOV;
      double maxAzFOV;
      aInput.ReadValueOfType(minAzFOV, UtInput::cANGLE);
      aInput.ReadValueOfType(maxAzFOV, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(minAzFOV, -UtMath::cPI);
      aInput.ValueLessOrEqual(maxAzFOV, UtMath::cPI);
      aInput.ValueLessOrEqual(minAzFOV, maxAzFOV);
      SetAzimuthFieldOfView(minAzFOV, maxAzFOV);
   }
   else if (command == "elevation_field_of_view")
   {
      double minElFOV;
      double maxElFOV;
      aInput.ReadValueOfType(minElFOV, UtInput::cANGLE);
      aInput.ReadValueOfType(maxElFOV, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(minElFOV, -UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(maxElFOV, UtMath::cPI_OVER_2);
      aInput.ValueLessOrEqual(minElFOV, maxElFOV);
      SetElevationFieldOfView(minElFOV, maxElFOV);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Set the azimuth field of view.
bool WsfRectangularFieldOfView::SetAzimuthFieldOfView(double aMinAzFOV, double aMaxAzFOV)
{
   mMinAzFOV = aMinAzFOV;
   mMaxAzFOV = aMaxAzFOV;
   return true;
}

// =================================================================================================
//! Set the elevation field of view.
bool WsfRectangularFieldOfView::SetElevationFieldOfView(double aMinElFOV, double aMaxElFOV)
{
   mMinElFOV = aMinElFOV;
   mMaxElFOV = aMaxElFOV;
   return true;
}

// =================================================================================================
//! Is the target within the field-of-view?
//!
//! @param aThisToTgtAz [input] Azimuth of the target with respect to the device (radians).
//! @param aThisToTgtEl [input] Elevation of the target with respect to the device (radians).
//! @returns 'true' if within the field of view or 'false' if not.
bool WsfRectangularFieldOfView::WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
{
   return ((aThisToTgtAz >= mMinAzFOV) && (aThisToTgtAz <= mMaxAzFOV) && (aThisToTgtEl >= mMinElFOV) &&
           (aThisToTgtEl <= mMaxElFOV));
}
