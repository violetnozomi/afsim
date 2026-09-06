// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtAzElUtil.hpp"

#include <string>

#include "UtStringUtil.hpp"

namespace ut
{
namespace azel
{

// =================================================================================================
//! Convert a string representing an interpolation type (linear or logarithmic) to its equivalent enumeration.
//! @param aInterpolationType   [output] The resulting enumeration.
//! @param aString              [input]  The string to be converted.
//! @returns 'true' if successful or 'false' if the input string did not represent a valid interpolation type.
bool util::StringToEnum(InterpolationType& aInterpolationType, const std::string& aString)
{
   bool        ok       = true;
   std::string myString = aString;
   UtStringUtil::ToLower(myString);
   if ((myString == "linear") || myString.empty())
   {
      aInterpolationType = InterpolationType::cLinear;
   }
   else if (myString == "logarithmic")
   {
      aInterpolationType = InterpolationType::cLogarithmic;
   }
   else
   {
      ok = false;
   }
   return ok;
}

// =================================================================================================
//! Convert an interpolation type enumeration to an equivalent string.
//! @param aInterpolationType [input] The interpolation type whose string value is desired.
//! @returns The string value of the enumeration.
std::string util::EnumToString(InterpolationType aInterpolationType)
{
   switch (aInterpolationType)
   {
   case InterpolationType::cLinear:
      return "linear";

   case InterpolationType::cLogarithmic:
      return "logarithmic";

   default:
      return "unknown";
   }
}

} // namespace azel
} // namespace ut
