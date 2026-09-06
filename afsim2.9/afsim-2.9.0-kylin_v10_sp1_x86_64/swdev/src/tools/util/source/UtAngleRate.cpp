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

#include "UtAngleRate.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtMath.hpp"
#include "UtStringUtil.hpp"

int UtAngleRate::mDefaultFormatAngle = 0;
int UtAngleRate::mDefaultFormatRate  = 0;

const double UtAngleRate::cDEG_PER_MIL = 0.05625;
const double UtAngleRate::cMIL_PER_DEG = 17.777777778;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtAngleRate& aAngleRate)
{
   UtReal value;
   aIn >> value;
   if (!aIn)
      return aIn;

   double angle            = value;
   aAngleRate.mFormatAngle = value.GetFormat();

   char c;
   c = aIn.peek();

   while ((isalnum(c) == 0) && (c != '#'))
   {
      aIn.get(c);
      if (!aIn)
         break;
      c = aIn.peek();
   }

   if (c != 'r' && c != 'R' && c != 'd' && c != 'D' && c != 'm' && c != 'M' && c != 'u')
   {
      aAngleRate.mAngleRate = angle;
      aAngleRate.mFormatAngle |= UtAngleRate::FmtAngNoSuffix;
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

   if (suffix == "rpm")
   {
      aAngleRate.mAngleRate   = angle * 360.0 / 60.0;
      aAngleRate.mFormatAngle = UtAngleRate::FmtRevolutions;
      aAngleRate.mFormatRate  = UtAngleRate::FmtM;
   }
   else
   {
      std::string suffixAng;  // is everything before the slash
      std::string suffixRate; // is everything after the slash
      size_t      slashLoc = suffix.find_first_of("/");
      if (slashLoc != std::string::npos)
      {
         suffixAng  = suffix.substr(0, slashLoc);
         suffixRate = suffix.substr(slashLoc + 1);

         if (suffixAng == "radians")
         {
            angle = angle * UtMath::cDEG_PER_RAD;
            aAngleRate.mFormatAngle |= UtAngleRate::FmtRadians;
         }
         else if (suffixAng == "radian")
         {
            angle = angle * UtMath::cDEG_PER_RAD;
            aAngleRate.mFormatAngle |= UtAngleRate::FmtRadian;
         }
         else if (suffixAng == "rad")
         {
            angle = angle * UtMath::cDEG_PER_RAD;
            aAngleRate.mFormatAngle |= UtAngleRate::FmtRad;
         }
         else if (suffixAng == "degrees")
         {
            aAngleRate.mFormatAngle |= UtAngleRate::FmtDegrees;
         }
         else if (suffixAng == "degree")
         {
            aAngleRate.mFormatAngle |= UtAngleRate::FmtDegree;
         }
         else if (suffixAng == "deg")
         {
            aAngleRate.mFormatAngle |= UtAngleRate::FmtDeg;
         }
         else if (suffixAng == "mils")
         {
            angle = angle * UtAngleRate::cDEG_PER_MIL;
            aAngleRate.mFormatAngle |= UtAngleRate::FmtMils;
         }
         else if (suffixAng == "mil")
         {
            angle = angle * UtAngleRate::cDEG_PER_MIL;
            aAngleRate.mFormatAngle |= UtAngleRate::FmtMil;
         }
         else
         {
            for (int i = (static_cast<int>(suffix.size()) - 1); i >= 0; --i)
            {
               aIn.putback(suffix[i]);
            }
            aAngleRate.mFormatAngle |= UtAngleRate::FmtAngNoSuffix;
            return aIn;
         }

         if ((suffixRate == "seconds") || (suffixRate == "second") || (suffixRate == "secs") || (suffixRate == "sec") ||
             (suffixRate == "s"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtS;
            aAngleRate.mAngleRate  = angle;
         }
         else if ((suffixRate == "minutes") || (suffixRate == "minute") || (suffixRate == "mins") ||
                  (suffixRate == "min") || (suffixRate == "m"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtM;
            aAngleRate.mAngleRate  = angle / 60.0;
         }
         else if ((suffixRate == "hours") || (suffixRate == "hour") || (suffixRate == "hrs") || (suffixRate == "hr") ||
                  (suffixRate == "h"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtM;
            aAngleRate.mAngleRate  = angle / 3600.0;
         }
         else if ((suffixRate == "milliseconds") || (suffixRate == "millisecond") || (suffixRate == "msecs") ||
                  (suffixRate == "msec") || (suffixRate == "ms"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtMilliSec;
            aAngleRate.mAngleRate  = angle / 1.0E-3;
         }
         else if ((suffixRate == "microseconds") || (suffixRate == "microsecond") || (suffixRate == "usecs") ||
                  (suffixRate == "usec") || (suffixRate == "us"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtMicroSec;
            aAngleRate.mAngleRate  = angle / 1.0E-6;
         }
         else if ((suffixRate == "nanoseconds") || (suffixRate == "nanosecond") || (suffixRate == "nsecs") ||
                  (suffixRate == "nsec") || (suffixRate == "ns"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtNanoSec;
            aAngleRate.mAngleRate  = angle / 1.0E-9;
         }
         else if ((suffixRate == "days") || (suffixRate == "day"))
         {
            aAngleRate.mFormatRate = UtAngleRate::FmtDays;
            aAngleRate.mAngleRate  = angle / 86400.0;
         }
         else
         {
            for (int i = (static_cast<int>(suffix.size()) - 1); i >= 0; --i)
            {
               aIn.putback(suffix[i]);
            }
            aAngleRate.mFormatAngle |= UtAngleRate::FmtAngNoSuffix;
            return aIn;
         }
      }
      else
      {
         for (int i = (static_cast<int>(suffix.size()) - 1); i >= 0; --i)
         {
            aIn.putback(suffix[i]);
         }
         aAngleRate.mFormatAngle |= UtAngleRate::FmtAngNoSuffix;
      }
   }

   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtAngleRate& aAngleRate)
{
   if (!aOut)
      return aOut; // error set on stream
   double      angle;
   const char* suffix1;
   const char* suffix2;
   int         formatAngle = aAngleRate.mFormatAngle;
   int         formatRate  = aAngleRate.mFormatRate;
   if (formatAngle == 0)
      formatAngle = UtAngleRate::FmtDeg;
   if (formatRate == 0)
      formatRate = UtAngleRate::FmtS;

   if ((formatAngle == UtAngleRate::FmtRevolutions) && (formatRate == UtAngleRate::FmtM))
   {
      angle = aAngleRate.mAngleRate / 360.0 * 60.0;
      aOut << UtReal(angle, formatAngle);
      if ((formatAngle & UtAngleRate::FmtAngNoSuffix) == 0)
      {
         aOut << ' ' << "rpm";
      }
      return aOut;
   }

   switch (formatAngle & UtReal::FmtUnitMask)
   {
   case UtAngleRate::FmtDeg:
      angle   = aAngleRate.mAngleRate;
      suffix1 = "deg";
      break;

   case UtAngleRate::FmtDegree:
      angle   = aAngleRate.mAngleRate;
      suffix1 = "degree";
      break;

   case UtAngleRate::FmtDegrees:
      angle   = aAngleRate.mAngleRate;
      suffix1 = "degrees";
      break;

   case UtAngleRate::FmtRad:
      angle   = aAngleRate.mAngleRate * UtMath::cRAD_PER_DEG;
      suffix1 = "rad";
      break;

   case UtAngleRate::FmtRadian:
      angle   = aAngleRate.mAngleRate * UtMath::cRAD_PER_DEG;
      suffix1 = "radian";
      break;

   case UtAngleRate::FmtRadians:
      angle   = aAngleRate.mAngleRate * UtMath::cRAD_PER_DEG;
      suffix1 = "radians";
      break;

   case UtAngleRate::FmtMil:
      angle   = aAngleRate.mAngleRate * UtAngleRate::cMIL_PER_DEG;
      suffix1 = "mil";
      break;

   case UtAngleRate::FmtMils:
      angle   = aAngleRate.mAngleRate * UtAngleRate::cMIL_PER_DEG;
      suffix1 = "mils";
      break;

   default:
      angle   = aAngleRate.mAngleRate;
      suffix1 = "deg";
      break;
   }
   switch (formatRate)
   {
   case UtAngleRate::FmtS:
      suffix2 = "sec";
      break;
   case UtAngleRate::FmtM:
      suffix2 = "min";
      angle *= 60.0;
      break;
   case UtAngleRate::FmtH:
      suffix2 = "hour";
      angle *= 3600.0;
      break;
   case UtAngleRate::FmtDays:
      suffix2 = "day";
      angle *= 86400.0;
      break;
   case UtAngleRate::FmtMilliSec:
      suffix2 = "msec";
      angle *= 1.0E-3;
      break;
   case UtAngleRate::FmtMicroSec:
      suffix2 = "usec";
      angle *= 1.0E-6;
      break;
   case UtAngleRate::FmtNanoSec:
      suffix2 = "nsec";
      angle *= 1.0E-9;
      break;
   default:
      suffix2 = "sec";
      break;
   }

   aOut << UtReal(angle, formatAngle);
   if ((formatAngle & UtAngleRate::FmtAngNoSuffix) == 0)
   {
      aOut << ' ' << suffix1 << '/' << suffix2;
   }
   return aOut;
}
