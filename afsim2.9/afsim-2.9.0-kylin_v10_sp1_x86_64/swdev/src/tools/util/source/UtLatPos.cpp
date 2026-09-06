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

#include "UtLatPos.hpp"

#include <iostream>

// extraction operator
std::istream& operator>>(std::istream& aIn, UtLatPos& aLat)
{
   UtAngle::GetAngle(aIn, aLat.mLat, aLat.mFormat, 'N', 'S', 90.0);
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtLatPos& aLat)
{
   UtAngle::PutAngle(aOut, aLat.mLat, aLat.mFormat, 'N', 'S');
   return aOut;
}
