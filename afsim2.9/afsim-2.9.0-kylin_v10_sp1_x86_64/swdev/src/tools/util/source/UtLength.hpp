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

#ifndef UTLENGTH_HPP
#define UTLENGTH_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtReal.hpp"

class UT_EXPORT UtLength
{
public:
   UtLength()
      : mLength(0)
      , mFormat(0)
   {
   }
   UtLength(double aLength, int aFormat = 0)
      : mLength(aLength)
      , mFormat(aFormat)
   {
   }

   UtLength& operator=(const UtLength& aLength) = default;

   UtLength& operator=(double aLength)
   {
      mLength = aLength;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mLength; }

   void Set(double aLength, int aFormat)
   {
      mLength = aLength;
      mFormat = aFormat;
   }

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
      FmtM         = UtReal::FmtUnitBase * 1,  // meters
      FmtKM        = UtReal::FmtUnitBase * 2,  // kilometers
      FmtFT        = UtReal::FmtUnitBase * 3,  // feet
      FmtMI        = UtReal::FmtUnitBase * 4,  // miles
      FmtNM        = UtReal::FmtUnitBase * 5,  // nautical miles
      FmtCentiM    = UtReal::FmtUnitBase * 6,  // centimeters
      FmtMicroM    = UtReal::FmtUnitBase * 7,  // micrometers
      FmtNanoM     = UtReal::FmtUnitBase * 8,  // nanometers
      FmtAngsM     = UtReal::FmtUnitBase * 9,  // angstroms
      FmtInch      = UtReal::FmtUnitBase * 10, // inches
      FmtKFT       = UtReal::FmtUnitBase * 11, // kilofeet
      FmtMegaM     = UtReal::FmtUnitBase * 12, // megameter
      FmtShowPoint = UtReal::FmtShowPoint,
      FmtNoSuffix  = UtReal::FmtFlagBase * 1,
      FmtParens    = UtReal::FmtFlagBase * 2 // (*)
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormat(int aFormat) { mDefaultFormat = aFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtLength& aLength);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtLength& aLength);

private:
   static int mDefaultFormat;

   static const double cFT_PER_M;
   static const double cM_PER_FT;
   static const double cM_PER_MI;
   static const double cM_PER_NM;

   double mLength; // meters
   int    mFormat;
};

#endif
