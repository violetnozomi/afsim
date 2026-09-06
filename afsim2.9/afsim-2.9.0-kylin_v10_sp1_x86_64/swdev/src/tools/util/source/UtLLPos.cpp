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

#include "UtLLPos.hpp"

#include <iomanip>
#include <iostream>

// stream extractor
std::istream& operator>>(std::istream& aIn, UtLLPos& aLL)
{
   UtLatPos lat;
   UtLonPos lon;

   aIn >> lat >> lon;
   aLL.mLat       = lat;
   aLL.mLatFormat = lat.GetFormat();
   aLL.mLon       = lon;
   aLL.mLonFormat = lon.GetFormat();
   return aIn;
}

// stream inserter
std::ostream& operator<<(std::ostream& aOut, const UtLLPos& aLL)
{
   char savedFill      = aOut.fill('0');
   int  savedPrecision = static_cast<int>(aOut.precision());
   aOut << std::setprecision(2) << UtLatPos(aLL.mLat, aLL.mLatFormat) << ' ';
   aOut << std::setprecision(2) << UtLonPos(aLL.mLon, aLL.mLonFormat) << ' ';
   aOut.precision(savedPrecision);
   aOut.fill(savedFill);
   return aOut;
}
