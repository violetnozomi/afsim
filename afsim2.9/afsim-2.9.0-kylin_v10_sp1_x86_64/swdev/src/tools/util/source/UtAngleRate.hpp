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

#ifndef UTANGLERATE_HPP
#define UTANGLERATE_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtMath.hpp"
#include "UtReal.hpp"

class UT_EXPORT UtAngleRate
{
public:
   UtAngleRate()
      : mAngleRate(0)
      , mFormatAngle(0)
      , mFormatRate(0)
   {
   }
   UtAngleRate(double aAngle, int aFormatAngle = 0, int aFormatRate = 0)
      : mAngleRate(aAngle)
      , mFormatAngle(aFormatAngle)
      , mFormatRate(aFormatRate)
   {
   }

   UtAngleRate& operator=(const UtAngleRate& aAngleRate) = default;

   UtAngleRate& operator=(double aAngleRate)
   {
      mAngleRate = aAngleRate;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
#ifdef UTANGLER_IN_RADIANS
   operator double() const { return mAngleRate * UtMath::cRAD_PER_DEG; }
#else
   operator double() const { return mAngleRate; }
#endif

   void Set(double aAngleRate, int aFormatAngle, int aFormatRate)
   {
      mAngleRate   = aAngleRate;
      mFormatAngle = aFormatAngle;
      mFormatRate  = aFormatRate;
   }

   void SetFormatAngle(int aFormatAngle) { mFormatAngle = aFormatAngle; }
   void SetFormatRate(int aFormatRate) { mFormatRate = aFormatRate; }
   int  GetFormatAngle() const { return mFormatAngle; }
   int  GetFormatRate() const { return mFormatRate; }

   // 'format' controls what the stream insertion operator '>>' writes.
   // To include fractions, add in the number of decimal places:
   //
   //   Fmt??  + nDecimals
   //
   // For example, 'FmtDeg + 2' will produce output that looks like:
   //
   //   xxxxx.ff deg

   enum
   {
      FmtDeg          = UtReal::FmtUnitBase * 1,  // deg
      FmtDegree       = UtReal::FmtUnitBase * 2,  // degree
      FmtDegrees      = UtReal::FmtUnitBase * 3,  // degrees
      FmtRad          = UtReal::FmtUnitBase * 4,  // rad
      FmtRadian       = UtReal::FmtUnitBase * 5,  // radian
      FmtRadians      = UtReal::FmtUnitBase * 6,  // radians
      FmtMil          = UtReal::FmtUnitBase * 8,  // mil A 'mil' is 1/6400 of a circle
      FmtMils         = UtReal::FmtUnitBase * 9,  // mils
      FmtRevolutions  = UtReal::FmtUnitBase * 10, // revolutions
      FmtAngShowPoint = UtReal::FmtShowPoint,
      FmtAngNoSuffix  = UtReal::FmtFlagBase * 1,
      FmtParens       = UtReal::FmtFlagBase * 2 // (*)
   };
   enum
   {
      FmtS        = 1 * 0x100, // s[.n]
      FmtM        = 4 * 0x100, // m[.n]
      FmtH        = 5 * 0x100, // h[.n]
      FmtMilliSec = 6 * 0x100, // millisec[.n]
      FmtMicroSec = 7 * 0x100, // microsec[.n]
      FmtNanoSec  = 8 * 0x100, // nano[.n]
      FmtDays     = 9 * 0x100  // days[.n]
   };

   // returns the current format for '<<'
   static int GetDefaultFormatAngle() { return mDefaultFormatAngle; }

   // returns the current format for '<<'
   static int GetDefaultFormatRate() { return mDefaultFormatRate; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormatAngle(int aFormat) { mDefaultFormatAngle = aFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormatRate(int aFormat) { mDefaultFormatRate = aFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtAngleRate& aAngleRate);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtAngleRate& aAngleRate);

private:
   static int mDefaultFormatAngle;
   static int mDefaultFormatRate;

   static const double cMIL_PER_DEG;
   static const double cDEG_PER_MIL;

   double mAngleRate; // degrees per second
   int    mFormatAngle;
   int    mFormatRate;
};

#endif
