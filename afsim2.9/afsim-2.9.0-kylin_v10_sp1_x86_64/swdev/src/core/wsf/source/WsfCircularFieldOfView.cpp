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

#include "WsfCircularFieldOfView.hpp"

#include "UtInput.hpp"
#include "UtVec3.hpp"

// =================================================================================================
WsfCircularFieldOfView::WsfCircularFieldOfView()
   : WsfFieldOfView()
{
   SetType("circular");
}

// =================================================================================================
// virtual
bool WsfCircularFieldOfView::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "half_angle")
   {
      aInput.ReadValueOfType(mHalfAngle, UtInput::cANGLE);
      aInput.ValueGreaterOrEqual(mHalfAngle, 0.0);
      aInput.ValueLessOrEqual(mHalfAngle, UtMath::cPI_OVER_2);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfCircularFieldOfView::WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl)
{
   bool inFOV = false;
   if ((aThisToTgtAz >= -UtMath::cPI) && (aThisToTgtAz <= UtMath::cPI))
   {
      double              sinAz        = sin(aThisToTgtAz);
      double              cosAz        = cos(aThisToTgtAz);
      double              sinEl        = sin(aThisToTgtEl);
      double              cosEl        = cos(aThisToTgtEl);
      double              targetVec[3] = {cosEl * cosAz, cosEl * sinAz, sinEl};
      static const double boresight[3] = {1.0, 0.0, 0.0};
      double              targetAngleFromBoresight =
         acos(UtMath::NormalizeSinCosMinusOne_One(UtVec3d::DotProduct(targetVec, boresight)));
      inFOV = (targetAngleFromBoresight <= mHalfAngle);
   }

   return inFOV;
}
