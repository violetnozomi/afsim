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

#ifndef UTREAL_HPP
#define UTREAL_HPP

#include "ut_export.h"

#include <iosfwd>

class UT_EXPORT UtReal
{
public:
   UtReal()
      : mReal(0)
      , mFormat(0)
   {
   }

   UtReal(double aReal, int aFormat = 0)
      : mReal(aReal)
      , mFormat(aFormat)
   {
   }

   // Using default copy constructor
   // UtReal(const UtReal& aReal);

   // Using default destructor
   //~UtReal();

   UtReal& operator=(const UtReal& aReal) = default;
   UtReal& operator                       =(double aReal)
   {
      mReal = aReal;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mReal; }

   void SetFormat(int aFormat) { mFormat = aFormat; }
   int  GetFormat() const { return mFormat; }

   // 'format' controls what the stream insertion operator '>>' writes.
   // To include fractions, add in the number of decimal places:
   //
   //   Fmt??  + nDecimals

   enum
   {
      // 4 bits each for the fraction and integer digit counts.

      FmtFracMask  = 0x0000000F, // Mask for number of fraction digits
      FmtFracIncr  = 0x00000001,
      FmtFracShift = 0,
      FmtIntMask   = 0x000000F0, // Mask for number of integer digits
      FmtIntIncr   = 0x00000010,
      FmtIntShift  = 4,

      FmtReserved1 = 0x00000F00, // Reserved for future use

      // The following indicates the format was determiend from a value
      // that was read by the stream extractor.
      FmtFromRead = 0x00001000,

      // Show a '+' for positive values
      FmtShowPlus = 0x00002000,

      // Show the decimal point even if there is no fraction portion.
      FmtShowPoint = 0x00004000,

      FmtReserved2 = 0x00008000, // Reserved for future use

      // The following fields are reserved for derived classes

      FmtFlagBase = 0x00010000,
      FmtFlagMask = 0x00FF0000,

      // The units area is reserved for derived classes
      FmtUnitBase = 0x01000000,
      FmtUnitMask = 0x3F000000
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormat(int aFormat) { mDefaultFormat = aFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtReal& aReal);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtReal& aReal);

private:
   static int mDefaultFormat;

   double mReal;
   int    mFormat;
};

#endif
