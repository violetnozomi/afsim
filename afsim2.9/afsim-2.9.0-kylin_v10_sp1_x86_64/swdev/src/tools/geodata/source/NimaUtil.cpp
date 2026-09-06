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

#include "NimaUtil.hpp"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>

#include "NimaMaps.hpp"

// Given a Data Code from MIL-STD-2411-1, section 5.1.4, return our
// internal Data Series ID as defined in NimaMaps.hpp.

// static
int NimaUtil::GetDataSeriesId(const char* aCode)
{
   char code[3];

   code[0] = toupper(aCode[0]);
   code[1] = toupper(aCode[1]);
   code[2] = '\0';

   int seriesId = NimaMaps::DsUnknown;
   if (::strcmp(code, "GN") == 0)
   {
      seriesId = NimaMaps::DsGNC;
   }
   else if (::strcmp(code, "JN") == 0)
   {
      seriesId = NimaMaps::DsJNC;
   }
   else if (::strcmp(code, "ON") == 0)
   {
      seriesId = NimaMaps::DsONC;
   }
   else if (::strcmp(code, "TP") == 0)
   {
      seriesId = NimaMaps::DsTPC;
   }
   else if (::strcmp(code, "JG") == 0)
   {
      seriesId = NimaMaps::DsJOG;
   }
   else if (::strcmp(code, "JA") == 0)
   {
      seriesId = NimaMaps::DsJOG_A;
   }
   else if (::strcmp(code, "JR") == 0)
   {
      seriesId = NimaMaps::DsJOG_R;
   }
   else if (::strcmp(code, "TF") == 0)
   {
      seriesId = NimaMaps::DsTFC;
   }
   else if (::strcmp(code, "AT") == 0)
   {
      seriesId = NimaMaps::DsATC;
   }
   else if (::strcmp(code, "TC") == 0)
   {
      seriesId = NimaMaps::DsTLM_100;
   }
   else if (::strcmp(code, "TL") == 0)
   {
      seriesId = NimaMaps::DsTLM_50;
   }
   else if (::strcmp(code, "I1") == 0)
   {
      seriesId = NimaMaps::DsCIB_10;
   }
   else if (::strcmp(code, "I2") == 0)
   {
      seriesId = NimaMaps::DsCIB_5;
   }
   else if (::strcmp(code, "I4") == 0)
   {
      seriesId = NimaMaps::DsCIB_1;
   }
   return seriesId;
}

// Given a zone code '0'-'9' (Northern Hemisphere) or 'A'-'I' (Southern
// Hemisphere), return the zone number 0-9 (Northern Hemisphere) or
// 10-18 (Southern Hemisphere).

// static
int NimaUtil::GetZoneNumber(const char aCode)
{
   int zoneId = 0;
   switch (toupper(aCode))
   {
   case '1':
      zoneId = 1;
      break;
   case '2':
      zoneId = 2;
      break;
   case '3':
      zoneId = 3;
      break;
   case '4':
      zoneId = 4;
      break;
   case '5':
      zoneId = 5;
      break;
   case '6':
      zoneId = 6;
      break;
   case '7':
      zoneId = 7;
      break;
   case '8':
      zoneId = 8;
      break;
   case '9':
      zoneId = 9;
      break;
   case 'A':
      zoneId = 10;
      break;
   case 'B':
      zoneId = 11;
      break;
   case 'C':
      zoneId = 12;
      break;
   case 'D':
      zoneId = 13;
      break;
   case 'E':
      zoneId = 14;
      break;
   case 'F':
      zoneId = 15;
      break;
   case 'G':
      zoneId = 16;
      break;
   case 'H':
      zoneId = 17;
      break;
   case 'I':
      zoneId = 18;
      break;
   default:
      zoneId = 0;
      break;
   }
   return zoneId;
}

// Convert a string to lower case (in place).  The pointer to the
// string to be converted is returned.

// static
char* NimaUtil::LowerCase(char* aStr)
{
   char* chPtr = aStr;
   while (*chPtr != '\0')
   {
      *chPtr = ::tolower(*chPtr);
      ++chPtr;
   }
   return aStr;
}

// Convert a potentially unterminated character string to a numeric
// value.  The character string 'aSize' characters will be copied to an
// internal buffer, a terminator will be appended and the conversion
// performed using the standard routine 'atoi'.  The function return
// is the converted value.

// static
int NimaUtil::StrToInt(const char* aStr, unsigned int aSize)
{
   char tempStr[12];

   assert(aSize < sizeof(tempStr));
   ::memcpy(tempStr, aStr, aSize);
   tempStr[aSize] = '\0';
   return ::atoi(tempStr);
}
