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

#include "VaScenarioKey.hpp"

namespace vespa
{
int VaScenarioKey::sUniqueId = -1;

VaScenarioKey::VaScenarioKey()
   : mId(-1)
{
}

bool VaScenarioKey::IsValid()
{
   return (mId >= 0);
}

// private
void VaScenarioKey::Initialize()
{
   mId = ++sUniqueId;
}

bool VaScenarioKey::operator<(const VaScenarioKey& aKey) const
{
   return (mId < aKey.mId);
}

bool VaScenarioKey::operator<(int aKey) const
{
   return (mId < aKey);
}

bool VaScenarioKey::operator==(const VaScenarioKey& aKey) const
{
   return (mId == aKey.mId);
}

bool VaScenarioKey::operator==(int aKey) const
{
   return (mId == aKey);
}

bool VaScenarioKey::operator!=(const VaScenarioKey& aKey) const
{
   return (mId != aKey.mId);
}

bool VaScenarioKey::operator!=(int aKey) const
{
   return (mId != aKey);
}
} // namespace vespa
