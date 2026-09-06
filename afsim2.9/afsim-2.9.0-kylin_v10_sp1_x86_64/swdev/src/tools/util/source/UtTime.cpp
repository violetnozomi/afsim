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

#include "UtTime.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

int UtTime::mDefaultFormat = 0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtTime& aTime)
{
   if (!aIn)
      return (aIn); // uncleared error is set...
   aIn >> std::ws;  // gobble leading whitespace
   // we must not attempt to skip white space as we extract single characters
   std::ios_base::fmtflags saveFlags = aIn.flags();
   aIn.unsetf(std::ios_base::skipws);

   aTime.mFormat = UtTime::FmtS;
   aTime.mFormat |= UtTime::FmtNoSuffix;
   char   ch;
   int    numParts = 0;
   int    part     = 0;
   double time     = 0.0;
   while (aIn)
   {
      aIn >> ch;
      if (!aIn)
         break;
      if ((ch >= '0') && (ch <= '9'))
      {
         part = static_cast<int>((part * 10.0) + (ch - '0'));
      }
      else if (ch == ':')
      {
         ++numParts;
         if (numParts == 1)
         {
            time          = part * 60.0;
            part          = 0;
            aTime.mFormat = UtTime::FmtMS;
         }
         else if (numParts == 2)
         {
            if (part > 59)
            {
               aIn.clear(std::ios_base::failbit);
               break;
            }
            time          = (time + part) * 60.0;
            part          = 0;
            aTime.mFormat = UtTime::FmtHMS;
         }
         else
         {
            break;
         }
      }
      else
      {
         break;
      }
   }
   if ((numParts > 0) && (part > 59))
      aIn.clear(std::ios_base::failbit);
   time += part;

   // assemble fraction if present...

   if (aIn && (ch == '.'))
   {
      aTime.mFormat |= UtTime::FmtShowPoint;
      double fraction = 0.0;
      double scale    = 1.0;
      while (aIn)
      {
         aIn >> ch;
         if (!aIn)
            break;
         if ((ch >= '0') && (ch <= '9'))
         {
            fraction = (fraction * 10.0) + (ch - '0');
            scale *= 10.0;
            ++aTime.mFormat;
         }
         else
         {
            break;
         }
      }
      time += (fraction / scale);
   }

   if (aIn)
   {
      // Read in the suffix.
      if ((aTime.mFormat & 0xf00) == UtTime::FmtS)
      {
         std::string suffix;

         aIn.setf(std::ios_base::skipws);

         aIn >> suffix;

         if ((suffix == "seconds") || (suffix == "secs") || (suffix == "sec") || (suffix == "s"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtS;
         }
         else if ((suffix == "minutes") || (suffix == "mins") || (suffix == "min") || (suffix == "m"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtM;
            time *= 60.0;
         }
         else if ((suffix == "hours") || (suffix == "hrs") || (suffix == "hr") || (suffix == "h"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtH;
            time *= 3600.0;
         }
         else if ((suffix == "milliseconds") || (suffix == "millisecond") || (suffix == "msecs") ||
                  (suffix == "msec") || (suffix == "ms"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtMilliSec;
            time *= 1.0E-3;
         }
         else if ((suffix == "microseconds") || (suffix == "microsecond") || (suffix == "usecs") ||
                  (suffix == "usec") || (suffix == "us"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtMicroSec;
            time *= 1.0E-6;
         }
         else if ((suffix == "nanoseconds") || (suffix == "nanosecond") || (suffix == "nsecs") || (suffix == "nsec") ||
                  (suffix == "ns"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtNanoSec;
            time *= 1.0E-9;
         }
         else if ((suffix == "days") || (suffix == "day"))
         {
            aTime.mFormat &= ~UtTime::FmtS;
            aTime.mFormat &= ~UtTime::FmtNoSuffix;
            aTime.mFormat |= UtTime::FmtDays;
            time *= 86400.0;
         }
         else
         {
            for (int i = 0; i < (int)suffix.length(); ++i)
            {
               aIn.unget();
            }
            aIn.unget();
            aTime.mFormat |= UtTime::FmtS;
         }
      }
      else
      {
         aIn.putback(ch);
      }
   }
   else if (aIn.eof())
   {
      // Allow eof only to allow reading from strstream
      aIn.clear(std::ios_base::goodbit);
   }
   aIn.setf(saveFlags);
   aTime.mTime = time;
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtTime& aTime)
{
   if (!aOut)
      return (aOut); // error set on stream

   double time = aTime.mTime;
   int    hh;
   int    mm;
   int    ss;
   int    mmss;
   int    hhmmss;
   char   saveFill = aOut.fill();
   int    format   = aTime.mFormat;
   if (format == 0)
      format = UtTime::mDefaultFormat;
   if (format == 0)
      format = UtTime::FmtHMS;

   // Determine the rounding factor...
   int numDecimals = format & 0xff;
   if (numDecimals > 8)
      numDecimals = 8;
   double scale = 1.0;
   for (int i = 0; i < numDecimals; ++i)
   {
      scale *= 10.0;
   }


   std::string suffix;
   switch (format & 0xf00)
   {
   case UtTime::FmtS:
      time += (0.5 / scale);
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "sec";
      break;

   case UtTime::FmtMS:
      time += (0.5 / scale);
      mmss = static_cast<int>(time);
      mm   = mmss / 60;
      ss   = mmss - (mm * 60);
      aOut << std::setfill('0') << std::setw(2) << mm << ':' << std::setw(2) << ss;
      break;

   case UtTime::FmtM:
      time /= 60.0;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "min";
      break;

   case UtTime::FmtH:
      time /= 3600.0;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "hr";
      break;

   case UtTime::FmtMilliSec:
      time /= 1.0E-3;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "ms";
      break;

   case UtTime::FmtMicroSec:
      time /= 1.0E-6;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "us";
      break;

   case UtTime::FmtNanoSec:
      time /= 1.0E-9;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "ns";
      break;

   case UtTime::FmtDays:
      time /= 86400.0;
      ss = static_cast<int>(time);
      aOut << ss;
      suffix = "days";
      break;

   default: // UtTime::FmtHMS
      time += (0.5 / scale);
      hhmmss = static_cast<int>(time);
      hh     = hhmmss / 3600;
      mmss   = hhmmss - (hh * 3600);
      mm     = mmss / 60;
      ss     = mmss - (mm * 60);
      aOut << std::setfill('0') << std::setw(2) << hh << ':' << std::setw(2) << mm << ':' << std::setw(2) << ss;
      break;
   }

   // Output fractional seconds if requested...

   if (numDecimals > 0)
   {
      int intTime  = static_cast<int>(time);
      int fraction = static_cast<int>((time - intTime) * scale);
      aOut << '.' << std::setfill('0') << std::setw(numDecimals) << fraction;
   }
   else if ((format & UtTime::FmtShowPoint) != 0)
   {
      aOut << '.';
   }

   if (((format & UtTime::FmtNoSuffix) == 0) && (!suffix.empty()))
   {
      aOut << ' ' << suffix;
   }
   aOut.fill(saveFill);
   return aOut;
}

//! Converts a string representation of units to an enumerated format.
UtTime::UtTime(double aTime, const std::string& aUnits)
{
   int    format    = UtTime::FmtS;
   double timeInSec = aTime;

   // find the best fit for the string description of the units.
   if (aUnits.empty() || (aUnits == "seconds") || (aUnits == "secs") || (aUnits == "sec") || (aUnits == "s"))
   {
      format = UtTime::FmtS;
   }
   else if ((aUnits == "minutes") || (aUnits == "mins") || (aUnits == "min") || (aUnits == "m"))
   {
      format = UtTime::FmtM;
      timeInSec *= 60.0;
   }
   else if ((aUnits == "hours") || (aUnits == "hrs") || (aUnits == "hr") || (aUnits == "h"))
   {
      format = UtTime::FmtH;
      timeInSec *= 3600;
   }
   else if ((aUnits == "milliseconds") || (aUnits == "millisecond") || (aUnits == "msecs") || (aUnits == "msec") ||
            (aUnits == "ms"))
   {
      format = UtTime::FmtMilliSec;
      timeInSec *= 1.0E-3;
   }
   else if ((aUnits == "microseconds") || (aUnits == "microsecond") || (aUnits == "usecs") || (aUnits == "usec") ||
            (aUnits == "us"))
   {
      format = UtTime::FmtMicroSec;
      timeInSec *= 1.0E-6;
   }
   else if ((aUnits == "nanoseconds") || (aUnits == "nanosecond") || (aUnits == "nsecs") || (aUnits == "nsec") ||
            (aUnits == "ns"))
   {
      format = UtTime::FmtNanoSec;
      timeInSec *= 1.0E-9;
   }
   else if ((aUnits == "days") || (aUnits == "day"))
   {
      format = UtTime::FmtDays;
      timeInSec *= 86400.0;
   }
   else
   {
   }

   mFormat = format;
   mTime   = timeInSec;
}

//! Converts an enumerated time units format into a string.
std::string UtTime::FormatToString(const int aFormat)
{
   std::string suffix = "";
   switch (aFormat & 0xf00)
   {
   case UtTime::FmtS:
      suffix = "sec";
      break;
   case UtTime::FmtMS:
      suffix = "mm:ss";
      break;
   case UtTime::FmtM:
      suffix = "min";
      break;
   case UtTime::FmtH:
      suffix = "hr";
      break;
   case UtTime::FmtMilliSec:
      suffix = "ms";
      break;
   case UtTime::FmtMicroSec:
      suffix = "us";
      break;
   case UtTime::FmtNanoSec:
      suffix = "ns";
      break;
   case UtTime::FmtDays:
      suffix = "days";
      break;
   default:
      suffix = "hh:mm:ss";
      break;
   };
   return suffix;
}

int UtTime::StringToFormat(const std::string& aUnits)
{
   int format = FmtNoSuffix;
   if (!aUnits.empty())
   {
      const char firstChar = aUnits.at(0);
      switch (firstChar)
      {
      case 's':
         if ((aUnits == "seconds") || (aUnits == "secs") || (aUnits == "sec") || (aUnits == "s"))
         {
            format = UtTime::FmtS;
         }
         break;
      case 'm':
         if ((aUnits == "minutes") || (aUnits == "mins") || (aUnits == "min") || (aUnits == "m"))
         {
            format = UtTime::FmtM;
         }
         else if ((aUnits == "milliseconds") || (aUnits == "millisecond") || (aUnits == "msecs") ||
                  (aUnits == "msec") || (aUnits == "ms"))
         {
            format = UtTime::FmtMilliSec;
         }
         else if ((aUnits == "microseconds") || (aUnits == "microsecond"))
         {
            format = UtTime::FmtMicroSec;
         }
         break;
      case 'u':
         if ((aUnits == "usecs") || (aUnits == "usec") || (aUnits == "us"))
         {
            format = UtTime::FmtMicroSec;
         }
         break;
      case 'h':
         if ((aUnits == "hours") || (aUnits == "hrs") || (aUnits == "hr") || (aUnits == "h"))
         {
            format = UtTime::FmtH;
         }
         break;
      case 'n':
         if ((aUnits == "nanoseconds") || (aUnits == "nanosecond") || (aUnits == "nsecs") || (aUnits == "nsec") ||
             (aUnits == "ns"))
         {
            format = UtTime::FmtNanoSec;
         }
         break;
      case 'd':
         if ((aUnits == "days") || (aUnits == "day"))
         {
            format = UtTime::FmtDays;
         }
         break;
      default:
         format = UtTime::FmtNoSuffix;
      };
   }
   return format;
}

std::string UtTime::ToString(double aTime_sec, int aFormat)
{
   std::ostringstream oss;
   oss << UtTime(aTime_sec, aFormat);
   return oss.str();
}
