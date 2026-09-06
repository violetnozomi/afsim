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

#include "UtLLAPos.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>

// stream extractor
std::istream& operator>>(std::istream& aIn, UtLLAPos& aLLA)
{
   UtLatPos lat;
   UtLonPos lon;
   UtLength alt;

   aIn >> lat >> lon >> alt;
   aLLA.mLat       = lat;
   aLLA.mLatFormat = lat.GetFormat();
   aLLA.mLon       = lon;
   aLLA.mLonFormat = lon.GetFormat();
   aLLA.mAlt       = alt;
   aLLA.mAltFormat = alt.GetFormat();
   return aIn;
}

// stream inserter
std::ostream& operator<<(std::ostream& aOut, const UtLLAPos& aLLA)
{
   char savedFill      = aOut.fill('0');
   int  savedPrecision = static_cast<int>(aOut.precision());
   aOut << std::setprecision(2) << UtLatPos(aLLA.mLat, aLLA.mLatFormat) << ' ';
   aOut << std::setprecision(2) << UtLonPos(aLLA.mLon, aLLA.mLonFormat) << ' ';
   aOut << std::setprecision(savedPrecision) << std::setfill(' ');
   aOut << UtLength(aLLA.mAlt, aLLA.mAltFormat);
   aOut.precision(savedPrecision);
   aOut.fill(savedFill);
   return aOut;
}
