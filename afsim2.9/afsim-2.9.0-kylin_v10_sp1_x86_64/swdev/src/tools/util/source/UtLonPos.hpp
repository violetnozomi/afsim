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

#ifndef UTLONPOS_HPP
#define UTLONPOS_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtAngle.hpp"

class UT_EXPORT UtLonPos
{
public:
   UtLonPos() = default;

   UtLonPos(double aLon, int aFormat = 0)
      : mLon(aLon)
      , mFormat(aFormat)
      , mFiller(0)
   {
   }

   UtLonPos(int aDegrees, int aMinutes = 0, int aSeconds = 0, double aFraction = 0.0)
      : mFormat(0)
      , mFiller(0)
   {
      mLon = aDegrees + (aMinutes / 60.0) + ((aSeconds + aFraction) / 3600.0);
   }

   // Using default copy constructor
   UtLonPos(const UtLonPos&) = default;

   UtLonPos& operator=(const UtLonPos&) = default;

   UtLonPos& operator=(double aLon)
   {
      mLon = aLon;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mLon; }

   void Set(double aLon, int aFormat)
   {
      mLon    = aLon;
      mFormat = aFormat;
   }

   // See UtAngle for formatting codes
   void SetFormat(int aFormat) { mFormat = aFormat; }
   int  GetFormat() const { return mFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtLonPos& aLon);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtLonPos& aLon);

private:
   double mLon    = 0;
   int    mFormat = 0;
   // Explicitly add the packing bytes and zero them.  Allows byte-level comparison between values.
   int mFiller = 0;
};

#endif
