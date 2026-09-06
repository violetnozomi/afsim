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

#ifndef UTTIME_HPP
#define UTTIME_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

class UT_EXPORT UtTime
{
public:
   UtTime()
      : mTime(0.0)
      , mFormat(0)
   {
   }

   UtTime(double aTime, int aFormat = 0)
      : mTime(aTime)
      , mFormat(aFormat)
   {
   }

   UtTime(int aHours, int aMinutes = 0, int aSeconds = 0, double aFraction = 0.0)
      : mFormat(0)
   {
      mTime = (aHours * 3600.0) + (aMinutes * 60.0) + aSeconds + aFraction;
   }

   //! Set time with time value and a string describing the units.
   UtTime(double aTime, const std::string& aUnits);

   UtTime& operator=(const UtTime& aTime) = default;

   UtTime& operator=(double aTime)
   {
      mTime = aTime;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator double() const { return mTime; }

   void SetFormat(int aFormat) { mFormat = aFormat; }

   int GetFormat() const { return mFormat; }

   //! Converts an enumerated time units format into a string.
   static std::string FormatToString(const int aFormat);

   //! Converts a string into an enumerated time units format.
   //! Returns UtTime::FmtNoSuffix if this string does not exactly match
   //! a recognized time unit.
   //! NOTE: this function is case sensitive and correct units
   //!       designations are expected to be lower-cased.
   static int StringToFormat(const std::string& aFormat);

   static std::string ToString(double aTime_sec, int aFormat);

   // 'format' controls how much the stream insertion operator '>>' writes.
   // To include output for fractions of seconds, use:
   //
   //   FmtHMS + nDecimals
   //
   // For example, 'FmtHMS + 2' will produce output that looks like:
   //
   //   hh:mm:ss.ff

   enum Fmt
   {
      FmtS         = 1 * 0x100,  // s[.n]
      FmtMS        = 2 * 0x100,  // mm:ss[.n]
      FmtHMS       = 3 * 0x100,  // hh:mm:ss[.n]
      FmtM         = 4 * 0x100,  // m[.n]
      FmtH         = 5 * 0x100,  // h[.n]
      FmtMilliSec  = 6 * 0x100,  // millisec[.n]
      FmtMicroSec  = 7 * 0x100,  // microsec[.n]
      FmtNanoSec   = 8 * 0x100,  // nano[.n]
      FmtDays      = 9 * 0x100,  // days[.n]
      FmtShowPoint = 0x1000 * 1, // Always show a decimal point
      FmtNoSuffix  = 0x1000 * 2
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static int SetDefaultFormat(int aFormat)
   {
      int oldFormat  = mDefaultFormat;
      mDefaultFormat = aFormat;
      return (oldFormat);
   }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtTime& aTime);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtTime& aTime);

private:
   static int mDefaultFormat;

   double mTime; // seconds
   int    mFormat;
};

#endif
