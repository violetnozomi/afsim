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

#ifndef UTSPEED_HPP
#define UTSPEED_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtReal.hpp"

class UT_EXPORT UtSpeed
{
public:
   UtSpeed()
      : mSpeed(0)
      , mFormat(0)
      , mAlt(0)
   {
   }

   UtSpeed(double aSpeed, int aFormat = 0, double aAlt = 0)
      : mSpeed(aSpeed)
      , mFormat(aFormat)
      , mAlt(aAlt)
   {
   }

   // Using default copy constructor
   // UtSpeed(const UtSpeed& aSpeed);

   // Using default destructor
   //~UtSpeed();

   UtSpeed& operator=(const UtSpeed& aSpeed) = default;

   UtSpeed& operator=(double aSpeed)
   {
      mSpeed = aSpeed;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mSpeed; }

   void SetAlt(double aAlt) { mAlt = aAlt; }

   void SetFormat(int aFormat) { mFormat = aFormat; }
   int  GetFormat() const { return mFormat; }

   // 'format' controls what the stream insertion operator '>>' writes.
   // To include fractions, add in the number of decimal places:
   //
   //   Fmt??  + nDecimals
   //
   // For example, 'FmtM + 2' will produce output that looks like:
   //
   //   xxxxx.ff m

   enum
   {
      FmtMPS       = UtReal::FmtUnitBase * 1, // meters/sec
      FmtKPH       = UtReal::FmtUnitBase * 2, // kilometers/hour
      FmtKMH       = UtReal::FmtUnitBase * 2, // kilometers/hour (alias)
      FmtFPS       = UtReal::FmtUnitBase * 3, // feet/sec
      FmtMPH       = UtReal::FmtUnitBase * 4, // miles/hour
      FmtKTS       = UtReal::FmtUnitBase * 5, // knots
      FmtFPM       = UtReal::FmtUnitBase * 6, // feet/min
      FmtMACH      = UtReal::FmtUnitBase * 7, // mach speed
      FmtShowPoint = UtReal::FmtShowPoint,
      FmtNoSuffix  = UtReal::FmtFlagBase * 1,
      FmtParens    = UtReal::FmtFlagBase * 2 // (*)
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static int SetDefaultFormat(int aFormat)
   {
      int oldFormat  = mDefaultFormat;
      mDefaultFormat = aFormat;
      return oldFormat;
   }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtSpeed& aSpeed);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtSpeed& aSpeed);

private:
   static int mDefaultFormat;

   double mSpeed; // meters per second
   int    mFormat;
   double mAlt; // altitude used to calculate sonic velocity (for mach speed)
};

#endif
