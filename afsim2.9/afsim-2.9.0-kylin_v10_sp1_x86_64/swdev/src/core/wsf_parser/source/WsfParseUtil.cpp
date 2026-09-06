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

#include "WsfParseUtil.hpp"

#include <ctype.h>

#include "UtCast.hpp"

namespace WsfParseUtil
{
bool                   cOPTIMIZE_PARSE = true;
bool                   cIS_WHITESPACE[256];
bool                   cIS_ALPHANUMERIC[256];
WSF_PARSER_EXPORT void InitializeParseUtil()
{
   static bool initialized = false;
   if (initialized)
   {
      return;
   }
   for (int i = 0; i < 256; ++i)
   {
      cIS_WHITESPACE[i]   = false;
      cIS_ALPHANUMERIC[i] = isalpha(i) || IsDigit(i) || (i == '_');
   }
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\0')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\f')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\n')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\r')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\t')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>('\v')] = true;
   cIS_WHITESPACE[ut::safe_cast<unsigned int>(' ')]  = true;
}

} // namespace WsfParseUtil
