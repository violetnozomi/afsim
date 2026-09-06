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

#ifndef UTDATASIZE_HPP
#define UTDATASIZE_HPP

#include "ut_export.h"

#include <iosfwd>

#include "UtReal.hpp"

class UT_EXPORT UtDataSize
{
public:
   UtDataSize()
      : mSize(0)
      , mFormat(0)
   {
   }
   UtDataSize(double aSize, int aFormat = 0)
      : mSize(aSize)
      , mFormat(aFormat)
   {
   }

   UtDataSize& operator=(const UtDataSize& aSize) = default;

   UtDataSize& operator=(double aSize)
   {
      mSize = aSize;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mSize; }

   void Set(double aSize, int aFormat)
   {
      mSize   = aSize;
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
      FmtBits      = UtReal::FmtUnitBase * 1, // bits
      FmtBytes     = UtReal::FmtUnitBase * 2, // bytes
      FmtKBytes    = UtReal::FmtUnitBase * 3, // kilobytes
      FmtMBytes    = UtReal::FmtUnitBase * 4, // megabytes
      FmtGBytes    = UtReal::FmtUnitBase * 5, // gigabytes
      FmtKBits     = UtReal::FmtUnitBase * 6, // kilobits
      FmtMBits     = UtReal::FmtUnitBase * 7, // megabits
      FmtGBits     = UtReal::FmtUnitBase * 8, // gigabits
      FmtShowPoint = UtReal::FmtShowPoint,
      FmtNoSuffix  = UtReal::FmtFlagBase * 1
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static void SetDefaultFormat(int aFormat) { mDefaultFormat = aFormat; }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtDataSize& aSize);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtDataSize& aSize);

private:
   static int mDefaultFormat;

   double mSize; // bytes
   int    mFormat;
};

#endif
