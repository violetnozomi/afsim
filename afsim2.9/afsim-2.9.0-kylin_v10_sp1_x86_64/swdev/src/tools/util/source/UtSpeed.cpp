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

#include "UtSpeed.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtAtmosphere.hpp"
#include "UtStringUtil.hpp"

namespace
{
UtAtmosphere& GetGlobalAtmosphere()
{
   static UtAtmosphere atmosphere(UtAtmosphere::CreateNewAtmosphereTable());
   return atmosphere;
}
} // namespace

int UtSpeed::mDefaultFormat = 0;

// extraction operator
std::istream& operator>>(std::istream& aIn, UtSpeed& aSpeed)
{
   UtReal value;
   aIn >> value;
   if (!aIn)
      return aIn;

   double speed   = value;
   aSpeed.mFormat = value.GetFormat();

   // extract and process suffix
   std::string suffix;
   aIn >> suffix;
   if (!aIn)
   {
      if (!aIn.eof())
         return (aIn);
      // eof is allowed and cleared because we'll get eof when trying to read the null character on a strstream.
      aIn.clear(std::ios_base::goodbit);
   }
   UtStringUtil::ToLower(suffix);

   if ((suffix == "m/s") || (suffix == "ms") || (suffix == "m/sec"))
   {
      aSpeed.mSpeed = speed;
      aSpeed.mFormat |= UtSpeed::FmtMPS;
   }
   else if ((suffix == "km/h") || (suffix == "kmh"))
   {
      aSpeed.mSpeed = (speed * 1000.0) / 3600.0;
      aSpeed.mFormat |= UtSpeed::FmtKMH;
   }
   else if ((suffix == "ft/s") || (suffix == "fps"))
   {
      aSpeed.mSpeed = speed / 3.28084;
      aSpeed.mFormat |= UtSpeed::FmtFPS;
   }
   else if ((suffix == "ft/m") || (suffix == "fpm"))
   {
      aSpeed.mSpeed = (speed / 3.28084) / 60.0;
      aSpeed.mFormat |= UtSpeed::FmtFPM;
   }
   else if ((suffix == "mi/h") || (suffix == "mph"))
   {
      aSpeed.mSpeed = ((speed / 3.28084) * 5280.0) / 3600.0;
      aSpeed.mFormat |= UtSpeed::FmtMPH;
   }
   else if ((suffix == "kt") || (suffix == "kts") || (suffix == "knots"))
   {
      aSpeed.mSpeed = (speed * 1852.0) / 3600.0;
      aSpeed.mFormat |= UtSpeed::FmtKTS;
   }
   else if ((suffix == "mach") || (suffix == "ma"))
   {
      aSpeed.mSpeed = (speed * GetGlobalAtmosphere().SonicVelocity(aSpeed.mAlt));
      aSpeed.mFormat |= UtSpeed::FmtMACH;
   }
   else
   {
      aIn.clear(std::ios_base::failbit);
   }
   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtSpeed& aSpeed)
{
   if (!aOut)
      return (aOut); // error set on stream
   double      speed;
   const char* suffix;
   int         format = aSpeed.mFormat;
   if (format == 0)
      format = UtSpeed::mDefaultFormat;
   if (format == 0)
      format = UtSpeed::FmtMPS;

   switch (format & UtReal::FmtUnitMask)
   {
   case UtSpeed::FmtMPS:
      speed  = aSpeed.mSpeed;
      suffix = "m/s";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(M/SEC)";
      break;

   case UtSpeed::FmtKPH:
      speed  = (aSpeed.mSpeed / 1000.0) * 3600.0;
      suffix = "km/h";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(KM/HR)";
      break;

   case UtSpeed::FmtFPS:
      speed  = aSpeed.mSpeed * 3.28084;
      suffix = "ft/s";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(FT/SEC)";
      break;

   case UtSpeed::FmtFPM:
      if ((format & UtSpeed::FmtParens) != 0)
      {
         speed  = aSpeed.mSpeed * 3.28084;
         suffix = "(FT/SEC)";
      }
      else
      {
         speed  = aSpeed.mSpeed * 3.28084 * 60.0;
         suffix = "ft/m";
      }
      break;

   case UtSpeed::FmtMPH:
      speed  = ((aSpeed.mSpeed * 3.28084) / 5280.0) * 3600.0;
      suffix = "mph";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(MPH)";
      break;

   case UtSpeed::FmtKTS:
      speed  = (aSpeed.mSpeed / 1852.0) * 3600.0;
      suffix = "kts";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(KTS)";
      break;

   case UtSpeed::FmtMACH:
      speed  = (aSpeed.mSpeed / GetGlobalAtmosphere().SonicVelocity(aSpeed.mAlt));
      suffix = "mach";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(MACH)";
      break;

   default:
      speed  = aSpeed.mSpeed;
      suffix = "m/s";
      if ((format & UtSpeed::FmtParens) != 0)
         suffix = "(M/SEC)";
      break;
   }
   aOut << UtReal(speed, format);
   if ((format & UtSpeed::FmtNoSuffix) == 0)
   {
      aOut << ' ' << suffix;
   }
   return aOut;
}
