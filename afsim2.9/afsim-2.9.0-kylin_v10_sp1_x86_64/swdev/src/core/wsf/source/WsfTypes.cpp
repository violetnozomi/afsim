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

#include "WsfTypes.hpp"

namespace WsfTypes
{
//! Convert a string representing a spatial domain to its equivalent enumeration.
//! @param aString        [input]  The string to converted.
//! @param aSpatialDomain [output] The resulting enumeration.
//! @returns 'true' if successful or 'false' if the input string did not represent a valid spatial domain.
bool StringToEnum(const std::string& aString, WsfSpatialDomain& aSpatialDomain)
{
   bool ok = true;
   if (aString == "land")
   {
      aSpatialDomain = WSF_SPATIAL_DOMAIN_LAND;
   }
   else if (aString == "air")
   {
      aSpatialDomain = WSF_SPATIAL_DOMAIN_AIR;
   }
   else if (aString == "surface")
   {
      aSpatialDomain = WSF_SPATIAL_DOMAIN_SURFACE;
   }
   else if (aString == "subsurface")
   {
      aSpatialDomain = WSF_SPATIAL_DOMAIN_SUBSURFACE;
   }
   else if (aString == "space")
   {
      aSpatialDomain = WSF_SPATIAL_DOMAIN_SPACE;
   }
   else
   {
      ok = false;
   }
   return ok;
}

//! Convert an spatial domain enumeration to an equivalent string.
//! @param aSpatialDomain [input] The spatial domain whose string value is desired.
//! @returns The string value of the enumeration.
std::string EnumToString(WsfSpatialDomain aSpatialDomain)
{
   std::string enumString("unknown");
   switch (aSpatialDomain)
   {
   case WSF_SPATIAL_DOMAIN_LAND:
      enumString = "land";
      break;
   case WSF_SPATIAL_DOMAIN_AIR:
      enumString = "air";
      break;
   case WSF_SPATIAL_DOMAIN_SURFACE:
      enumString = "surface";
      break;
   case WSF_SPATIAL_DOMAIN_SUBSURFACE:
      enumString = "subsurface";
      break;
   case WSF_SPATIAL_DOMAIN_SPACE:
      enumString = "space";
      break;
   default:
      break;
   }
   return enumString;
}
} // namespace WsfTypes
