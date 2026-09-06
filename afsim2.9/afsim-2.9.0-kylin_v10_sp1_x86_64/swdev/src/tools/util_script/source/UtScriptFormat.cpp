// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptFormat.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "UtAngle.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptVec3.hpp"
#include "UtStringUtil.hpp"
#include "UtTime.hpp"
#include "UtVec3.hpp"

namespace
{
int LatLonFormat(const std::string& aFormatStr, int aPrecision)
{
   std::string formatStr(aFormatStr);
   UtStringUtil::ToLower(formatStr);
   int format = UtAngle::FmtDMS;
   if (formatStr == "dms")
   {
      format = UtAngle::FmtDMS;
   }
   else if (formatStr == "dm")
   {
      format = UtAngle::FmtDM;
   }
   else if (formatStr == "d")
   {
      format = UtAngle::FmtD;
   }
   return (format += std::min(std::max(aPrecision, 0), 15));
}

int TimeFormat(const std::string& aFormatStr, int aPrecision)
{
   std::string formatStr(aFormatStr);
   UtStringUtil::ToLower(formatStr);
   int format = UtAngle::FmtDMS;
   if (formatStr == "hms")
   {
      format = UtTime::FmtHMS;
   }
   else if (formatStr == "h")
   {
      format = UtTime::FmtH;
   }
   else if (formatStr == "ms")
   {
      format = UtTime::FmtMS;
   }
   else if (formatStr == "m")
   {
      format = UtTime::FmtM;
   }
   else if (formatStr == "s")
   {
      format = UtTime::FmtS;
   }
   format |= UtTime::FmtNoSuffix;
   return (format += std::min(std::max(aPrecision, 0), 15));
}

std::string FormatTime(double aTime, int aFormat)
{
   // UtTime does not handle negative time values...
   std::ostringstream oss;
   if (aTime >= 0.0)
   {
      oss << UtTime(aTime, aFormat);
   }
   else
   {
      oss << '-' << UtTime(-aTime, aFormat);
   }
   return oss.str();
}
} // namespace

UtScriptFormat::UtScriptFormat(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Format", aScriptTypesPtr)
{
   UT_SCRIPT_NONWRAPPING_CLASS(Format);

   mConstructible = true;

   // The vector forms must appear before the double forms or else overloading will not work.
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Fixed, (const UtScriptData& aVecObj, std::streamsize aPrecision))
   {
      std::ostringstream oss;
      UtVec3d            vec(UtScriptVec3::ConvertToVec3(aVecObj));
      oss << std::fixed << std::setprecision(aPrecision) << '(' << vec[0] << ", " << vec[1] << ", " << vec[2] << ')';
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Fixed, (double value, std::streamsize precision))
   {
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(precision) << value;
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Scientific, (const UtScriptData& aVecObj, std::streamsize aPrecision))
   {
      std::ostringstream oss;
      UtVec3d            vec(UtScriptVec3::ConvertToVec3(aVecObj));
      oss << std::scientific << std::setprecision(aPrecision) << '(' << vec[0] << ", " << vec[1] << ", " << vec[2] << ')';
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Scientific, (double value, std::streamsize precision))
   {
      std::ostringstream oss;
      oss << std::scientific << std::setprecision(precision) << value;
      return oss.str();
   };
   UT_SCRIPT_STATIC_METHOD(std::string, General, (const UtScriptData& aVecObj, std::streamsize aPrecision))
   {
      std::ostringstream oss;
      UtVec3d            vec(UtScriptVec3::ConvertToVec3(aVecObj));
      oss << std::setprecision(aPrecision) << '(' << vec[0] << ", " << vec[1] << ", " << vec[2] << ')';
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, General, (double value, std::streamsize precision))
   {
      std::ostringstream oss;
      oss << std::setprecision(precision) << value;
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Latitude, (double lat))
   {
      std::ostringstream oss;
      int                format = UtAngle::FmtDMS + 2;
      UtAngle::PutAngle(oss, lat, format, 'n', 's');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Latitude, (double lat, int precision))
   {
      std::ostringstream oss;
      int                format = UtAngle::FmtDMS + precision;
      UtAngle::PutAngle(oss, lat, format, 'n', 's');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Latitude, (double lat, const std::string& formatStr, int precision))
   {
      std::ostringstream oss;
      int                format = LatLonFormat(formatStr, precision);
      UtAngle::PutAngle(oss, lat, format, 'n', 's');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Longitude, (double lon))
   {
      std::ostringstream oss;
      int                format = UtAngle::FmtDMS + 2;
      UtAngle::PutAngle(oss, lon, format, 'e', 'w');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Longitude, (double lon, int precision))
   {
      std::ostringstream oss;
      int                format = UtAngle::FmtDMS + precision;
      UtAngle::PutAngle(oss, lon, format, 'e', 'w');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Longitude, (double lon, const std::string& formatStr, int precision))
   {
      std::ostringstream oss;
      int                format = LatLonFormat(formatStr, precision);
      UtAngle::PutAngle(oss, lon, format, 'e', 'w');
      return oss.str();
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Time, (double aTime))
   {
      int format = UtTime::FmtHMS + 2;
      return FormatTime(aTime, format);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Time, (double aTime, int aPrecision))
   {
      int format = UtTime::FmtHMS + aPrecision;
      return FormatTime(aTime, format);
   };
   UT_SCRIPT_CONSTEXPR
   UT_SCRIPT_STATIC_METHOD(std::string, Time, (double aTime, const std::string& aFormatStr, int aPrecision))
   {
      int format = TimeFormat(aFormatStr, aPrecision);
      return FormatTime(aTime, format);
   };
}

void* UtScriptFormat::Create(const UtScriptContext& /*aContext*/)
{
   // Since this acts as a static class, we just return the current instance.
   return this;
}

void UtScriptFormat::Destroy(void* aObjectPtr)
{
   // Do nothing.  This is a static class.
}
