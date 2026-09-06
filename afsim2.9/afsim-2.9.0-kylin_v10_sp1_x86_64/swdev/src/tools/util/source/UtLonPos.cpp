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

#include "UtLonPos.hpp"

#include <iostream>

#include "UtMath.hpp"

// extraction operator
std::istream& operator>>(std::istream& aIn, UtLonPos& aLon)
{
   UtAngle::GetAngle(aIn, aLon.mLon, aLon.mFormat, 'E', 'W', 180.0);
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtLonPos& aLon)
{
   double angle = UtMath::NormalizeAngleMinus180_180(aLon.mLon);
   UtAngle::PutAngle(aOut, angle, aLon.mFormat, 'E', 'W');
   return aOut;
}
