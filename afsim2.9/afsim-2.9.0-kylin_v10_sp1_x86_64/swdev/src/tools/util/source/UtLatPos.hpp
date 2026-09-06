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

#ifndef UTLATPOS_HPP
#define UTLATPOS_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtAngle.hpp"

class UT_EXPORT UtLatPos
{
public:
   UtLatPos() = default;

   UtLatPos(double aLat, int aFormat = 0)
      : mLat(aLat)
      , mFormat(aFormat)
      , mFiller(0)
   {
   }

   UtLatPos(int aDegrees, int aMinutes = 0, int aSeconds = 0, double aFraction = 0.0)
      : mFormat(0)
      , mFiller(0)
   {
      mLat = aDegrees + (aMinutes / 60.0) + ((aSeconds + aFraction) / 3600.0);
   }

   // Using default copy constructor
   UtLatPos(const UtLatPos&) = default;

   UtLatPos& operator=(const UtLatPos&) = default;

   UtLatPos& operator=(double aLat)
   {
      mLat = aLat;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mLat; }

   void Set(double aLat, int aFormat)
   {
      mLat    = aLat;
      mFormat = aFormat;
   }

   // See UtAngle for formatting codes
   void SetFormat(int aFormat) { mFormat = aFormat; }
   int  GetFormat() const { return mFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtLatPos& aLat);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtLatPos& aLat);

private:
   double mLat    = 0;
   int    mFormat = 0;
   // Explicitly add the packing bytes and zero them.  Allows byte-level comparison between values.
   int mFiller = 0;
};

#endif
