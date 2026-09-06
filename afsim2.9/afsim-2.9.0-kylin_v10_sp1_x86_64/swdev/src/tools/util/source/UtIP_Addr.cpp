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
//  This class supports IP input using GuiTextInput in the utilg library.
// ****************************************************************************

#include "UtIP_Addr.hpp"

#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>

#include "UtStringUtil.hpp"

// static
bool UtIP_Addr::Parsable(const std::string& aIP)
{
   bool        lReturn = false;
   std::string ip      = aIP;
   int         i;
   int         count = 0;
   for (i = 0; i < static_cast<int>(ip.length()); ++i)
   {
      if (ip[i] == '.')
      {
         ip[i] = ' ';
         ++count;
      }
   }
   if (count == 3)
   {
      int a;
      int b;
      int c;
      int d;
      if (4 == sscanf(ip.c_str(), "%i %i %i %i", &a, &b, &c, &d))
      {
         if ((a >= 0) && (a < 256) && (b >= 0) && (b < 256) && (c >= 0) && (c < 256) && (d >= 0) && (d < 256))
         {
            lReturn = true;
         }
      }
   }
   return lReturn;
}

// extraction operator
std::istream& operator>>(std::istream& aIn, UtIP_Addr& aIP_Addr)
{
   std::string value;
   aIn >> value;
   if (!aIn)
      return aIn;

   if (!UtIP_Addr::Parsable(value))
   {
      aIn.clear(std::ios_base::failbit);
   }
   aIP_Addr.mIP_Addr = value;
   if (!aIn)
   {
      if (!aIn.eof())
         return aIn;
      // eof is allowed and cleared because we'll get eof when trying to read the null character on a strstream.
      aIn.clear(std::ios_base::goodbit);
   }

   return aIn;
}

// insertion operator
std::ostream& operator<<(std::ostream& aOut, const UtIP_Addr& aIP_Addr)
{
   if (!aOut)
      return aOut; // error set on stream
   aOut << aIP_Addr.mIP_Addr;
   return aOut;
}
