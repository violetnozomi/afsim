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

#include "UtDataSize.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtStringUtil.hpp"

int UtDataSize::mDefaultFormat = 0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtDataSize& aSize)
{
   UtReal value;
   aIn >> value;
   if (!aIn)
      return aIn;

   double sizev  = value;
   aSize.mFormat = value.GetFormat();

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

   if ((suffix == "bit") || (suffix == "bits"))
   {
      aSize.mSize = sizev;
      aSize.mFormat |= UtDataSize::FmtBits;
   }
   else if ((suffix == "bytes") || (suffix == "byte"))
   {
      aSize.mSize = sizev * 8.0;
      aSize.mFormat |= UtDataSize::FmtBytes;
   }
   else if ((suffix == "kbit") || (suffix == "kbits"))
   {
      aSize.mSize = sizev * 1000.0;
      aSize.mFormat |= UtDataSize::FmtKBits;
   }
   else if ((suffix == "kbyte") || (suffix == "kbytes"))
   {
      aSize.mSize = sizev * 8000.0;
      aSize.mFormat |= UtDataSize::FmtKBytes;
   }
   else if ((suffix == "mbit") || (suffix == "mbits"))
   {
      aSize.mSize = sizev * 1000000.0;
      aSize.mFormat |= UtDataSize::FmtMBits;
   }
   else if ((suffix == "mbyte") || (suffix == "mbytes"))
   {
      aSize.mSize = sizev * 8000000.0;
      aSize.mFormat |= UtDataSize::FmtMBytes;
   }
   else if ((suffix == "gbit") || (suffix == "gbits"))
   {
      aSize.mSize = sizev * 1000000000.0;
      aSize.mFormat |= UtDataSize::FmtGBits;
   }
   else if ((suffix == "gbyte") || (suffix == "gbytes"))
   {
      aSize.mSize = sizev * 8000000000.0;
      aSize.mFormat |= UtDataSize::FmtGBytes;
   }
   else
   {
      aIn.clear(std::ios_base::failbit);
   }
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtDataSize& aSize)
{
   if (!aOut)
      return aOut; // error set on stream
   double      sizev;
   const char* suffix;
   int         format = aSize.mFormat;
   if (format == 0)
      format = UtDataSize::mDefaultFormat;
   if (format == 0)
      format = UtDataSize::FmtBits;

   switch (format & UtReal::FmtUnitMask)
   {
   case UtDataSize::FmtBits:
      sizev  = aSize.mSize;
      suffix = "bits";
      break;

   case UtDataSize::FmtBytes:
      sizev  = aSize.mSize / 8.0;
      suffix = "bytes";
      break;

   case UtDataSize::FmtKBits:
      sizev  = aSize.mSize / 1000.0;
      suffix = "kbits";
      break;

   case UtDataSize::FmtKBytes:
      sizev  = aSize.mSize / 8000.0;
      suffix = "kbytes";
      break;

   case UtDataSize::FmtMBits:

      sizev  = aSize.mSize / 1000000.0;
      suffix = "mbits";
      break;

   case UtDataSize::FmtMBytes:
      sizev  = aSize.mSize / 8000000.0;
      suffix = "mbytes";
      break;

   case UtDataSize::FmtGBits:
      sizev  = aSize.mSize / 1000000000.0;
      suffix = "gbits";
      break;

   case UtDataSize::FmtGBytes:
      sizev  = aSize.mSize / 8000000000.0;
      suffix = "gbytes";
      break;

   default:
      sizev  = aSize.mSize;
      suffix = "bits";
      break;
   }
   aOut << UtReal(sizev, format);
   if ((format & UtDataSize::FmtNoSuffix) == 0)
   {
      aOut << ' ' << suffix;
   }
   return aOut;
}
