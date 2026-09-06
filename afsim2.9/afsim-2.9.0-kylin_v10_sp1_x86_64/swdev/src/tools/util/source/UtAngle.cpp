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

#include "UtAngle.hpp"

#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>

int UtAngle::mDefaultFormat = 0;

int UtAngle::SetDefaultFormat(int aFormat)
{
   int oldFormat  = mDefaultFormat;
   mDefaultFormat = aFormat;
   return oldFormat;
}

// get an angle from an input stream
// static
void UtAngle::GetAngle(std::istream& aIn, double& aAngle, int& aFormat, char aPosChar, char aNegChar, double aMaxAngle)
{
   if (!aIn)
   {
      return; // uncleared error is set...
   }
   aIn >> std::ws; // Skip to first non-whitespace character

   aFormat          = UtAngle::FmtFromRead;
   int    dmsFormat = UtAngle::FmtD;
   int    ch;
   int    numParts  = 0;
   double part      = 0.0;
   double partUnits = 1.0;
   double angle     = 0.0;
   double sign      = 1.0;
   int    digitInc  = UtAngle::FmtDegIncr;

   // Check for a direction prefix...

   ch = aIn.peek();
   if (ch == EOF)
   {
      aIn.clear(std::ios_base::eofbit | std::ios_base::failbit);
   }
   else if (toupper(aPosChar) == toupper(ch))
   {
      ch   = aIn.get();
      sign = 1.0;
      aFormat |= UtAngle::FmtPrefixDir;
      if (isupper(ch) != 0)
      {
         aFormat |= UtAngle::FmtUpper;
      }
   }
   else if (toupper(aNegChar) == toupper(ch))
   {
      ch   = aIn.get();
      sign = -1.0;
      aFormat |= UtAngle::FmtPrefixDir;
      if (isupper(ch) != 0)
      {
         aFormat |= UtAngle::FmtUpper;
      }
   }

   while (aIn)
   {
      ch = aIn.get();
      if ((ch >= '0') && (ch <= '9'))
      {
         aFormat += digitInc;
         part = (part * 10.0) + (ch - '0');
      }
      else if (ch == ':')
      {
         ++numParts;
         if (numParts == 1)
         {
            angle     = part;
            part      = 0.0;
            partUnits = 1.0 / 60.0;
            dmsFormat = UtAngle::FmtDM;
            digitInc  = UtAngle::FmtMinIncr;
         }
         else if (numParts == 2)
         {
            if (part > 59.0)
            {
               aIn.clear(std::ios_base::failbit);
               break;
            }
            angle += (part * partUnits);
            part      = 0.0;
            partUnits = 1.0 / 3600.0;
            dmsFormat = UtAngle::FmtDMS;
            digitInc  = UtAngle::FmtSecIncr;
         }
         else
         {
            aIn.clear(std::ios_base::failbit); // Can't have 3 ':'
            break;
         }
      }
      else
      {
         break;
      }
   }
   angle += (part * partUnits);
   aFormat |= dmsFormat;

   // Make the last minutes/seconds is valid...
   if (numParts > 0)
   {
      if (part > 59.0)
      {
         aIn.clear(std::ios_base::failbit);
      }
   }

   // Make sure supplied components are not null...

   if ((aFormat & UtAngle::FmtDegMask) == 0)
   {
      aIn.clear(std::ios_base::failbit); // Null degrees specification
   }
   if (((dmsFormat == UtAngle::FmtDM) || (dmsFormat == UtAngle::FmtDMS)) && ((aFormat & UtAngle::FmtMinMask) == 0))
   {
      aIn.clear(std::ios_base::failbit); // Null minutes specification
   }
   if ((dmsFormat == UtAngle::FmtDMS) && ((aFormat & UtAngle::FmtSecMask) == 0))
   {
      aIn.clear(std::ios_base::failbit); // Null seconds specification
   }

   // assemble fraction if present...

   if (ch == '.')
   {
      aFormat |= UtAngle::FmtShowPoint;
      double fraction = 0.0;
      double scale    = 1.0;
      while (isdigit(ch = aIn.get()) != 0)
      {
         if ((ch >= '0') && (ch <= '9'))
         {
            fraction = (fraction * 10.0) + (ch - '0');
            scale *= 10.0;
            aFormat += UtAngle::FmtFracIncr;
         }
      }
      angle += (fraction * partUnits / scale);
   }

   // Check for and extract the suffix direction indicator..

   if ((aFormat & UtAngle::FmtPrefixDir) == 0)
   {
      if (toupper(ch) == toupper(aPosChar))
      {
         sign = 1.0;
         if (isupper(ch) != 0)
         {
            aFormat |= UtAngle::FmtUpper;
         }
      }
      else if (toupper(ch) == toupper(aNegChar))
      {
         sign = -1.0;
         if (isupper(ch) != 0)
         {
            aFormat |= UtAngle::FmtUpper;
         }
      }
      else
      {
         aIn.clear(std::ios_base::failbit); // must have a direction
      }
   }
   else if (ch != EOF)
   {
      aIn.putback(ch);
   }

   if (angle > aMaxAngle)
   {
      aIn.clear(std::ios_base::failbit);
   }
   aAngle = sign * angle;
}

// Put an angle to an output stream...
// static
void UtAngle::PutAngle(std::ostream& aOut, double aAngle, int aFormat, char aPosChar, char aNegChar)
{
   if (aOut.fail())
   {
      return; // error set on stream
   }
   int format = aFormat;
   if (format == 0)
      format = UtAngle::mDefaultFormat;
   if (format == 0)
      format = UtAngle::FmtDMS + 1;

   double angle = aAngle;
   char   dir   = aPosChar;
   if (angle < 0.0)
   {
      dir   = aNegChar;
      angle = -angle;
   }

   // Output the prefix if requested...

   if (((format & UtAngle::FmtNoDir) == 0) && ((format & UtAngle::FmtPrefixDir) != 0))
   {
      if ((format & UtAngle::FmtUpper) != 0)
      {
         aOut << char(toupper(dir));
      }
      else
      {
         aOut << char(tolower(dir));
      }
   }

   // Determine the rounding factor...
   int numFracDigits = (format & UtAngle::FmtFracMask) >> UtAngle::FmtFracShift;
   if (numFracDigits > 8)
      numFracDigits = 8;
   double scale = 1.0;
   for (int i = 0; i < numFracDigits; ++i)
   {
      scale *= 10.0;
   }

   int numDegDigits = (format & UtAngle::FmtDegMask) >> UtAngle::FmtDegShift;
   int numMinDigits = (format & UtAngle::FmtMinMask) >> UtAngle::FmtMinShift;
   int numSecDigits = (format & UtAngle::FmtSecMask) >> UtAngle::FmtSecShift;

   if ((format & UtAngle::FmtFromRead) == 0)
   {
      if (numDegDigits == 0)
      {
         numDegDigits = 2;
      }
      if (numMinDigits == 0)
      {
         numMinDigits = 2;
      }
      if (numSecDigits == 0)
      {
         numSecDigits = 2;
      }
   }

   long int dd;
   long int mm;
   long int ss;
   long int ddmm;
   long int mmss;
   long int ddmmss;
   char     saveFill = aOut.fill();
   aOut.fill('0');
   bool useSeparator = ((format & UtAngle::FmtNoSep) == 0);

   switch (format & UtAngle::FmtTypeMask)
   {
   case UtAngle::FmtD:
      angle += (0.5 / scale);
      dd = static_cast<long int>(angle);
      if ((dd != 0) || (numDegDigits != 0))
      {
         aOut << std::setw(numDegDigits) << dd;
      }
      break;

   case UtAngle::FmtDM:
      angle *= 60.0; // minute and fractions of minutes
      angle += (0.5 / scale);
      ddmm = static_cast<long int>(angle);
      dd   = ddmm / 60;
      mm   = ddmm - (dd * 60);
      if ((dd != 0) || (numDegDigits != 0))
      {
         aOut << std::setw(numDegDigits) << dd;
      }
      if (useSeparator)
      {
         aOut << ':';
      }
      if ((mm != 0) || (numMinDigits != 0))
      {
         aOut << std::setw(numMinDigits) << mm;
      }
      break;

   case UtAngle::FmtDMS:
   default:
      angle *= 3600.0; // seconds and fractions of seconds
      angle += (0.5 / scale);
      ddmmss = static_cast<long int>(angle);
      dd     = ddmmss / 3600;
      mmss   = ddmmss - (dd * 3600);
      mm     = mmss / 60;
      ss     = mmss - (mm * 60);
      if ((dd != 0) || (numDegDigits != 0))
      {
         aOut << std::setw(numDegDigits) << dd;
      }
      if (useSeparator)
      {
         aOut << ':';
      }
      if ((mm != 0) || (numMinDigits != 0))
      {
         aOut << std::setw(numMinDigits) << mm;
      }
      if (useSeparator)
      {
         aOut << ':';
      }
      if ((ss != 0) || (numSecDigits != 0))
      {
         aOut << std::setw(numSecDigits) << ss;
      }
      break;
   }

   // Output fraction if requested...

   if (numFracDigits > 0)
   {
      long int intValue  = static_cast<long int>(angle);
      long int fracValue = static_cast<long int>((angle - intValue) * scale);
      aOut << '.' << std::setfill('0') << std::setw(numFracDigits) << fracValue;
   }
   else if ((format & UtAngle::FmtShowPoint) != 0)
   {
      aOut << '.';
   }

   // Output the direction suffix if requested

   if (((format & UtAngle::FmtNoDir) == 0) && ((format & UtAngle::FmtPrefixDir) == 0))
   {
      if ((format & UtAngle::FmtUpper) != 0)
      {
         aOut << char(toupper(dir));
      }
      else
      {
         aOut << char(tolower(dir));
      }
   }
   aOut.fill(saveFill);
}
