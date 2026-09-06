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

// ****************************************************************************
//  This class supports IP input using GuiTextInput in the utilg library.
// ****************************************************************************

#ifndef UTIP_ADDR_HPP
#define UTIP_ADDR_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

class UT_EXPORT UtIP_Addr
{
public:
   UtIP_Addr() = default;
   UtIP_Addr(const std::string& aIP_Addr)
      : mIP_Addr(aIP_Addr)
   {
   }

   // Using default copy constructor
   // UtIP_Addr(const UtIP_Addr& aIP_Addr);

   // Using default destructor
   //~UtIP_Addr();

   UtIP_Addr& operator=(const UtIP_Addr& aIP_Addr) = default;

   UtIP_Addr& operator=(const std::string& aIP_Addr)
   {
      mIP_Addr = aIP_Addr;
      return *this;
   }

   // Allow the value to be used anywhere a double would be used
   operator std::string() const { return mIP_Addr; }

   void Set(const std::string& aIP_Addr) { mIP_Addr = aIP_Addr; }

   const char* GetCharPtr() const { return mIP_Addr.c_str(); }

   friend UT_EXPORT std::istream& operator>>(std::istream& aIn, UtIP_Addr& aIP_Addr);
   friend UT_EXPORT std::ostream& operator<<(std::ostream& aOut, const UtIP_Addr& aIP_Addr);

   static bool Parsable(const std::string& aIP_Addr);

private:
   std::string mIP_Addr{"0.0.0.0"};
};

#endif
