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

#ifndef NimaUtil_hpp
#define NimaUtil_hpp

#include "geodata_export.h"

// ----------------------------------------------------------------------------
//
// This is a collection of utilities used by the various classes that process
// NIMA data.  They are not intended for use by other classes.
//
// ----------------------------------------------------------------------------

class GEODATA_EXPORT NimaUtil
{
public:
   // Given a Data Code from MIL-STD-2411-1, section 5.1.4, return our
   // internal Data Series ID as defined in NimaMaps.hpp.

   static int GetDataSeriesId(const char* aCode);

   // Given a zone code '0'-'9' (Northern Hemisphere) or 'A'-'I' (Southern
   // Hemisphere), return the zone number 0-9 (Northern Hemisphere) or
   // 10-18 (Southern Hemisphere).

   static int GetZoneNumber(const char aCode);

   // Convert a string to lower case (in place).  The pointer to the
   // string to be converted is returned.

   static char* LowerCase(char* aStr);

   // Convert a potentially unterminated character string to a numeric
   // value.  The character string 'aSize' characters will be copied to an
   // internal buffer, a terminator will be appended and the conversion
   // performed using the standard routine 'atoi'.  The function return
   // is the converted value.

   static int StrToInt(const char* aStr, unsigned int aSize);
};

#endif
