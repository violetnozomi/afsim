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

#ifndef UTANGLER_HPP
#define UTANGLER_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtMath.hpp"
#include "UtReal.hpp"

class UT_EXPORT UtAngleR
{
public:
   UtAngleR()
      : mAngle(0)
      , mFormat(0)
   {
   }
   UtAngleR(double aAngle, int aFormat = 0)
      : mAngle(aAngle)
      , mFormat(aFormat)
   {
   }

   UtAngleR& operator=(const UtAngleR& aAngle) = default;

   UtAngleR& operator=(double aAngle)
   {
      mAngle = aAngle;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
#ifdef UTANGLER_IN_RADIANS
   operator double() const { return mAngle * UtMath::cRAD_PER_DEG; }
#else
   operator double() const { return mAngle; }
#endif

   void Set(double aAngle, int aFormat)
   {
      mAngle  = aAngle;
      mFormat = aFormat;
   }

   void SetFormat(int aFormat) { mFormat = aFormat; }
   int  GetFormat() const { return mFormat; }

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
      FmtDeg        = UtReal::FmtUnitBase * 1,  // deg
      FmtDegree     = UtReal::FmtUnitBase * 2,  // degree
      FmtDegrees    = UtReal::FmtUnitBase * 3,  // degrees
      FmtRad        = UtReal::FmtUnitBase * 4,  // rad
      FmtRadian     = UtReal::FmtUnitBase * 5,  // radian
      FmtRadians    = UtReal::FmtUnitBase * 6,  // radians
      FmtMicroRad   = UtReal::FmtUnitBase * 7,  // microradians
      FmtMil        = UtReal::FmtUnitBase * 8,  // mil A 'mil' is 1/6400 of a circle
      FmtMils       = UtReal::FmtUnitBase * 9,  // mils
      FmtArcsecond  = UtReal::FmtUnitBase * 10, // arcsecond A 'arcsecond' is 1/3600 of a degree
      FmtArcseconds = UtReal::FmtUnitBase * 11, // arcseconds
      FmtShowPoint  = UtReal::FmtShowPoint,
      FmtNoSuffix   = UtReal::FmtFlagBase * 1,
      FmtParens     = UtReal::FmtFlagBase * 2 // (*)
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormat(int aFormat) { mDefaultFormat = aFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtAngleR& aAngle);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtAngleR& aAngle);

private:
   static int mDefaultFormat;

   static const double cMIL_PER_DEG;
   static const double cDEG_PER_MIL;
   static const double cDEG_PER_SEC;
   static const double cSEC_PER_DEG;

   double mAngle; // degrees
   int    mFormat;
};

#endif
