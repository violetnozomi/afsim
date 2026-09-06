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

// UtAngle is a helper class for UtLatPos and UtLonPos.  It defines the
// common functions for stream input and output.

#ifndef UTANGLE_HPP
#define UTANGLE_HPP

#include "ut_export.h"

#include <iosfwd>

class UT_EXPORT UtAngle
{
public:
   // 'format' controls how much the stream insertion operator '>>' writes.
   // To include output for fractions of units, use:
   //
   //   FmtDMS + nDecimals
   //
   // For example, 'FmtDMS + 2' will produce output that looks like:
   //
   //   dd:mm:ss.ff

   enum Fmt
   {
      FmtFracMask  = 0x0000000F, // Mask for number of fraction digits
      FmtFracIncr  = 0x00000001,
      FmtFracShift = 0,
      FmtSecMask   = 0x000000F0,
      FmtSecIncr   = 0x00000010,
      FmtSecShift  = 4,
      FmtMinMask   = 0x00000F00,
      FmtMinIncr   = 0x00000100,
      FmtMinShift  = 8,
      FmtDegMask   = 0x0000F000,
      FmtDegIncr   = 0x00001000,
      FmtDegShift  = 12,

      FmtTypeBase = 0x01000000,
      FmtTypeMask = 0x3F000000,

      FmtD   = FmtTypeBase * 1, // dd[.f][dir]
      FmtDM  = FmtTypeBase * 2, // dd[:]mm[.f][dir]
      FmtDMS = FmtTypeBase * 3, // dd[:]mm[:]ss[.f][dir]

      // The following indicates the format was determined from a value
      // that was read by the stream extractor.
      FmtFromRead = 0x00010000,

      // Show the decimal point even if there is no fraction portion.
      FmtShowPoint = 0x00020000,

      FmtSuffixDir = 0x00000000, // Suffix is implied
      FmtPrefixDir = 0x00040000,
      FmtNoDir     = 0x00080000,
      FmtUpper     = 0x00100000, // Make pre/suffix upper case on
                                 // output
      FmtNoSep = 0x10000000      // No separator (default is colon)
                                 // on output
   };

   // returns the current format for '<<'
   static int GetDefaultFormat() { return mDefaultFormat; }

   // Sets a new format for '<<' and returns the previous format.  The
   // new format applies until changed by another call to 'format'.
   static int SetDefaultFormat(int aFormat);

   static void GetAngle(std::istream& aIn, double& aAngle, int& aFormat, char aPosChar, char aNegChar, double aMaxAngle);

   static void PutAngle(std::ostream& aOut, double aAngle, int aFormat, char aPosChar, char aNegChar);

private:
   static int mDefaultFormat;
};

#endif
