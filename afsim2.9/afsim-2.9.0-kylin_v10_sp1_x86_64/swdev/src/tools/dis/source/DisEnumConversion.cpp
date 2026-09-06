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

#include "DisEnumConversion.hpp"

#include <locale>
#include <sstream>
#include <string>

std::string DisEnum::SimplifyString(const std::string& str)
{
   std::stringstream ss;
   std::locale       loc;
   for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
   {
      if (std::isalpha(*i, loc) || std::isdigit(*i, loc))
      {
         ss << std::tolower(*i, loc);
      }
   }
   ss << std::ends;
   return ss.str();
}
