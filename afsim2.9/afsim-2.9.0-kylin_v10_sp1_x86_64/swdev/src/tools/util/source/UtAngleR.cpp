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

#include "UtAngleR.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtReal.hpp"
#include "UtStringUtil.hpp"

int UtAngleR::mDefaultFormat = 0;

const double UtAngleR::cDEG_PER_MIL = 0.05625;
const double UtAngleR::cMIL_PER_DEG = 17.777777778;
const double UtAngleR::cDEG_PER_SEC = 2.777777778e-4;
const double UtAngleR::cSEC_PER_DEG = 3600.0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtAngleR& aAngle)
{
   UtReal value;
   aIn >> value;
   if (!aIn)
      return aIn;

   double angle   = value;
   aAngle.mFormat = value.GetFormat();

   char c;
   c = aIn.peek();

   while ((isalnum(c) == 0) && (c != '#'))
   {
      aIn.get(c);
      if (!aIn)
         break;
      c = aIn.peek();
   }

   // check to see what the first character is
   if (c != 'r' && c != 'R' && c != 'd' && c != 'D' && c != 'm' && c != 'M' && c != 'a' && c != 'A' && c != 'u')
   {
      aAngle.mAngle = angle;
      aAngle.mFormat |= UtAngleR::FmtNoSuffix;
      if (!aIn)
      {
         if (!aIn.eof())
            return aIn;
         // eof is allowed and cleared because we'll get eof when trying to read the null character on a strstream.
         aIn.clear(std::ios_base::goodbit);
      }
      return aIn;
   }

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

   if (suffix == "radians")
   {
      aAngle.mAngle = angle * UtMath::cDEG_PER_RAD;
      aAngle.mFormat |= UtAngleR::FmtRadians;
   }
   else if (suffix == "radian")
   {
      aAngle.mAngle = angle * UtMath::cDEG_PER_RAD;
      aAngle.mFormat |= UtAngleR::FmtRadian;
   }
   else if (suffix == "rad")
   {
      aAngle.mAngle = angle * UtMath::cDEG_PER_RAD;
      aAngle.mFormat |= UtAngleR::FmtRad;
   }
   else if ((suffix == "microradians") || (suffix == "microradian") || (suffix == "urads") || (suffix == "urad"))
   {
      aAngle.mAngle = angle * 1.0E-6 * UtMath::cDEG_PER_RAD;
      aAngle.mFormat |= UtAngleR::FmtMicroRad;
   }
   else if (suffix == "degrees")
   {
      aAngle.mAngle = angle;
      aAngle.mFormat |= UtAngleR::FmtDegrees;
   }
   else if (suffix == "degree")
   {
      aAngle.mAngle = angle;
      aAngle.mFormat |= UtAngleR::FmtDegree;
   }
   else if (suffix == "deg")
   {
      aAngle.mAngle = angle;
      aAngle.mFormat |= UtAngleR::FmtDeg;
   }
   else if (suffix == "mils")
   {
      aAngle.mAngle = angle * UtAngleR::cDEG_PER_MIL;
      aAngle.mFormat |= UtAngleR::FmtMils;
   }
   else if (suffix == "mil")
   {
      aAngle.mAngle = angle * UtAngleR::cDEG_PER_MIL;
      aAngle.mFormat |= UtAngleR::FmtMil;
   }
   else if (suffix == "arcsecond")
   {
      aAngle.mAngle = angle * UtAngleR::cDEG_PER_SEC;
      aAngle.mFormat |= UtAngleR::FmtArcsecond;
   }
   else if (suffix == "arcseconds")
   {
      aAngle.mAngle = angle * UtAngleR::cDEG_PER_SEC;
      aAngle.mFormat |= UtAngleR::FmtArcseconds;
   }
   else
   {
      for (int i = (static_cast<int>(suffix.size()) - 1); i >= 0; --i)
      {
         aIn.putback(suffix[i]);
      }
      aAngle.mAngle = angle;
      aAngle.mFormat |= UtAngleR::FmtNoSuffix;
   }
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtAngleR& aAngle)
{
   if (!aOut)
      return aOut; // error set on stream
   double      angle;
   const char* suffix;
   int         format = aAngle.mFormat;
   if (format == 0)
      format = UtAngleR::mDefaultFormat;
   if (format == 0)
      format = UtAngleR::FmtDeg;

   switch (format & UtReal::FmtUnitMask)
   {
   case UtAngleR::FmtDeg:
      angle  = aAngle.mAngle;
      suffix = "deg";
      break;

   case UtAngleR::FmtDegree:
      angle  = aAngle.mAngle;
      suffix = "degree";
      break;

   case UtAngleR::FmtDegrees:
      angle  = aAngle.mAngle;
      suffix = "degrees";
      break;

   case UtAngleR::FmtRad:
      angle  = aAngle.mAngle * UtMath::cRAD_PER_DEG;
      suffix = "rad";
      break;

   case UtAngleR::FmtRadian:

      angle  = aAngle.mAngle * UtMath::cRAD_PER_DEG;
      suffix = "radian";
      break;

   case UtAngleR::FmtRadians:
      angle  = aAngle.mAngle * UtMath::cRAD_PER_DEG;
      suffix = "radians";
      break;

   case UtAngleR::FmtMicroRad:
      angle  = aAngle.mAngle * UtMath::cRAD_PER_DEG / 1.0E-6;
      suffix = "urad";
      break;

   case UtAngleR::FmtMil:
      angle  = aAngle.mAngle * UtAngleR::cMIL_PER_DEG;
      suffix = "mil";
      break;

   case UtAngleR::FmtMils:
      angle  = aAngle.mAngle * UtAngleR::cMIL_PER_DEG;
      suffix = "mils";
      break;

   case UtAngleR::FmtArcsecond:
      angle  = aAngle.mAngle * UtAngleR::cSEC_PER_DEG;
      suffix = "arcsecond";
      break;

   case UtAngleR::FmtArcseconds:
      angle  = aAngle.mAngle * UtAngleR::cSEC_PER_DEG;
      suffix = "arcseconds";
      break;

   default:
      angle  = aAngle.mAngle;
      suffix = "deg";
      break;
   }
   aOut << UtReal(angle, format);
   if ((format & UtAngleR::FmtNoSuffix) == 0)
   {
      aOut << ' ' << suffix;
   }
   return aOut;
}
