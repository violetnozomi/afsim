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

#include "UtReal.hpp"

#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>

int UtReal::mDefaultFormat = 0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtReal& aReal)
{
   if (!aIn)
   {
      return aIn; // uncleared error is set
   }

   int    ch    = 0;
   double sign  = 1.0;
   double value = 0.0;

   aReal.mFormat = UtReal::FmtFromRead;

   aIn >> std::ws; // Skip to first non-whitespace character
   ch = aIn.peek();
   if (ch == EOF)
   {
      aIn.clear(std::ios_base::eofbit | std::ios_base::failbit);
      return aIn;
   }
   else if (ch == '-')
   {
      ch   = aIn.get();
      sign = -1.0;
   }
   else if (ch == '+')
   {
      aReal.mFormat |= UtReal::FmtShowPlus;
      ch = aIn.get();
   }

   // Assemble the integer part

   while (isdigit(ch = aIn.get()) != 0)
   {
      value = (value * 10.0) + (ch - '0');
      aReal.mFormat += UtReal::FmtIntIncr;
   }

   // assemble fraction if present...

   if (ch == '.')
   {
      aReal.mFormat |= UtReal::FmtShowPoint;
      double fraction = 0.0;
      double scale    = 1.0;
      while (isdigit(ch = aIn.get()) != 0)
      {
         fraction = (fraction * 10.0) + (ch - '0');
         scale *= 10.0;
         aReal.mFormat += UtReal::FmtFracIncr;
      }
      value += (fraction / scale);
   }
   if (ch != EOF)
   {
      aIn.putback(ch);
   }

   // Make sure at least one digit was supplied

   if ((aReal.mFormat & (UtReal::FmtIntMask | UtReal::FmtFracMask)) == 0)
   {
      aIn.clear(std::ios_base::failbit);
   }
   else if (aIn.eof())
   {
      // Don't return an EOF when an eof terminates the string.  EOF must get
      // picked up the next time around
      aIn.clear(std::ios_base::goodbit);
   }
   aReal.mReal = sign * value;
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtReal& aReal)
{
   if (!aOut)
   {
      return aOut; // error set on stream
   }

   int format = aReal.mFormat;
   if (format == 0)
   {
      format = UtReal::mDefaultFormat;
   }

   double value = aReal.mReal;
   if (value < 0.0)
   {
      value = -value;
      aOut << '-';
   }
   else if ((format & UtReal::FmtShowPlus) != 0)
   {
      aOut << '+';
   }

   int numFracDigits = (format & UtReal::FmtFracMask) >> UtReal::FmtFracShift;
   if (numFracDigits > 8)
   {
      numFracDigits = 8;
   }
   double scale = 1.0;
   for (int i = 0; i < numFracDigits; ++i)
   {
      scale *= 10.0;
   }
   value += (0.5 / scale);

   // Output the integer portion...

   long int intValue     = static_cast<long int>(value);
   int      numIntDigits = (format & UtReal::FmtIntMask) >> UtReal::FmtIntShift;
   if (numIntDigits > 0)
   {
      // Number of integer digits was specified so use that for the width.
      // The actual width will be automatically increased if the number of
      // digits that need to be output is greater that what is specified.

      char saveFill = aOut.fill('0');
      aOut << std::setw(numIntDigits) << intValue;
      aOut.fill(saveFill);
   }
   else if (intValue != 0)
   {
      // No integer portion field width was specified but we have a non-zero
      // value.  Use the default which utilizes as many digits as needed.

      aOut << intValue;
   }
   else if ((format & UtReal::FmtFromRead) == 0)
   {
      // A zero value always puts out a zero UNLESS it is from a value that
      // was read.  If value was read as '.23', we don't want to output the
      // integer portion.

      aOut << intValue;
   }

   // Output the fraction portion...

   if (numFracDigits > 0)
   {
      long int fracValue = static_cast<long int>((value - intValue) * scale);
      char     saveFill  = aOut.fill('0');
      aOut << '.' << std::setw(numFracDigits) << fracValue;
      aOut.fill(saveFill);
   }
   else if ((format & UtReal::FmtShowPoint) != 0)
   {
      aOut << '.';
   }
   return (aOut);
}
