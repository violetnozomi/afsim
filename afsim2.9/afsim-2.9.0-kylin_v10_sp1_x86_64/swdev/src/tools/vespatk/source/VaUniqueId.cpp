// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaUniqueId.hpp"

#include <sstream>

namespace vespa
{
unsigned int VaUniqueId::sLastUniqueId = 1;

VaUniqueId::VaUniqueId()
   : mUniqueId(++sLastUniqueId)
   , mUniqueIdString()
{
   std::ostringstream oss;
   oss << mUniqueId;
   mUniqueIdString = oss.str();
}

VaUniqueId::VaUniqueId(const VaUniqueId& aSrc, bool aCreateNewUniqueId)
   : mUniqueId(aSrc.mUniqueId)
   , mUniqueIdString(aSrc.mUniqueIdString)
{
   if (aCreateNewUniqueId)
   {
      mUniqueId = ++sLastUniqueId;

      std::ostringstream oss;
      oss << mUniqueId;
      mUniqueIdString = oss.str();
   }
}

// static
void VaUniqueId::ResetUniqueIds()
{
   sLastUniqueId = 1;
}
} // end namespace vespa
