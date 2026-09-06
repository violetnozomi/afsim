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

#include "UtLength.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtStringUtil.hpp"

int UtLength::mDefaultFormat = 0;

// Feet per meter
const double UtLength::cFT_PER_M = 3.28083989501312335958; // 1 / 0.3048
// Meters per feet
const double UtLength::cM_PER_FT = 0.3048;
// Meters per mile
const double UtLength::cM_PER_MI = 1609.344;
// Meters per nautical mile
const double UtLength::cM_PER_NM = 1852.0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtLength& aLength)
{
   UtReal value;
   aIn >> value;
   if (!aIn)
      return aIn;

   double length   = value;
   aLength.mFormat = value.GetFormat();

   // extract and process suffix
   std::string suffix;
   aIn >> suffix;
   if (!aIn)
   {
      if (!aIn.eof())
         return aIn;
      // eof is allowed and cleared because we'll get eof when trying to read the null character on a strstream.
      aIn.clear(std::ios_base::goodbit);
   }
   UtStringUtil::ToLower(suffix);

   if ((suffix == "meters") || (suffix == "meter") || (suffix == "m"))
   {
      aLength.mLength = length;
      aLength.mFormat |= UtLength::FmtM;
   }
   else if ((suffix == "kilometers") || (suffix == "kilometer") || (suffix == "km"))
   {
      aLength.mLength = length * 1000.0;
      aLength.mFormat |= UtLength::FmtKM;
   }
   else if ((suffix == "megameters") || (suffix == "megameter"))
   {
      aLength.mLength = length * 1000000.0;
      aLength.mFormat |= UtLength::FmtMegaM;
   }
   else if ((suffix == "feet") || (suffix == "foot") || (suffix == "ft"))
   {
      aLength.mLength = length * UtLength::cM_PER_FT;
      aLength.mFormat |= UtLength::FmtFT;
   }
   else if ((suffix == "kfeet") || (suffix == "kft"))
   {
      aLength.mLength = (length * 1000.0) * UtLength::cM_PER_FT;
      aLength.mFormat |= UtLength::FmtKFT;
   }
   else if ((suffix == "miles") || (suffix == "mile") || (suffix == "mi"))
   {
      aLength.mLength = length * UtLength::cM_PER_MI;
      aLength.mFormat |= UtLength::FmtMI;
   }
   else if ((suffix == "nm") || (suffix == "nmi"))
   {
      aLength.mLength = length * UtLength::cM_PER_NM;
      aLength.mFormat |= UtLength::FmtNM;
   }
   else if ((suffix == "centimeters") || (suffix == "centimeter") || (suffix == "cm"))
   {
      aLength.mLength = length * 1.0E-2;
      aLength.mFormat |= UtLength::FmtCentiM;
   }
   else if ((suffix == "micrometers") || (suffix == "micrometer") || (suffix == "um") || (suffix == "microns") ||
            (suffix == "micron"))
   {
      aLength.mLength = length * 1.0E-6;
      aLength.mFormat |= UtLength::FmtMicroM;
   }
   else if ((suffix == "nanometers") || (suffix == "nanometer"))
   {
      aLength.mLength = length * 1.0E-9;
      aLength.mFormat |= UtLength::FmtNanoM;
   }
   else if ((suffix == "angstroms") || (suffix == "angstrom"))
   {
      aLength.mLength = length * 1.0E-10;
      aLength.mFormat |= UtLength::FmtAngsM;
   }
   else if ((suffix == "inches") || (suffix == "inch") || (suffix == "in"))
   {
      aLength.mLength = (length / 12.0) * UtLength::cM_PER_FT;
      aLength.mFormat |= UtLength::FmtInch;
   }
   else
   {
      aIn.clear(std::ios_base::failbit);
   }
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtLength& aLength)
{
   if (!aOut)
      return aOut; // error set on stream
   double      length;
   const char* suffix;
   int         format = aLength.mFormat;
   if (format == 0)
      format = UtLength::mDefaultFormat;
   if (format == 0)
      format = UtLength::FmtM;

   switch (format & UtReal::FmtUnitMask)
   {
   case UtLength::FmtM:
      length = aLength.mLength;
      suffix = "m";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(M)";
      break;

   case UtLength::FmtKM:
      length = aLength.mLength / 1000.0;
      suffix = "km";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(KM)";
      break;

   case UtLength::FmtMegaM:
      length = aLength.mLength / 1000000.0;
      suffix = "megameters";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(MEGAMETERS)";
      break;

   case UtLength::FmtFT:
      length = aLength.mLength * UtLength::cFT_PER_M;
      suffix = "ft";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(FT)";
      break;

   case UtLength::FmtKFT:
      length = aLength * UtLength::cFT_PER_M / 1000.0;
      suffix = "kft";
      break;

   case UtLength::FmtMI:

      length = aLength.mLength / UtLength::cM_PER_MI;
      suffix = "mi";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(MILES)";
      break;

   case UtLength::FmtNM:
      length = aLength.mLength / UtLength::cM_PER_NM;
      suffix = "nm";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(NM)";
      break;

   case UtLength::FmtCentiM:
      length = aLength.mLength / 1.0E-2;
      suffix = "cm";
      break;

   case UtLength::FmtMicroM:
      length = aLength.mLength / 1.0E-6;
      suffix = "um";
      break;

   case UtLength::FmtNanoM:
      length = aLength.mLength / 1.0E-9;
      suffix = "nanometers";
      break;

   case UtLength::FmtAngsM:
      length = aLength.mLength / 1.0E-10;
      suffix = "angstroms";
      break;

   case UtLength::FmtInch:
      length = aLength.mLength * UtLength::cFT_PER_M * 12.0;
      suffix = "in";
      break;

   default:
      length = aLength.mLength;
      suffix = "m";
      if ((format & UtLength::FmtParens) != 0)
         suffix = "(M)";
      break;
   }
   aOut << UtReal(length, format);
   if ((format & UtLength::FmtNoSuffix) == 0)
   {
      aOut << ' ' << suffix;
   }
   return aOut;
}
