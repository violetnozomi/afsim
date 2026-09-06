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

#include "VaResourceManager.hpp"

#include "UtoResourceDB.hpp"

namespace vespa
{

// static
VaResourceManager& VaResourceManager::Instance()
{
   static VaResourceManager instance;
   return instance;
}

bool VaResourceManager::LoadResources(const std::string& aResourceFile)
{
   return UtoResourceDB::Instance().LoadResources(aResourceFile);
}

} // namespace vespa
