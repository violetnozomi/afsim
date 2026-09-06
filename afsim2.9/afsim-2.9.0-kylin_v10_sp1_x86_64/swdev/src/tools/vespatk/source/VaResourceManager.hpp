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

#ifndef VARESOURCEMANAGER_HPP
#define VARESOURCEMANAGER_HPP

#include "vespatk_export.h"

#include <string>

namespace vespa
{

class VESPATK_EXPORT VaResourceManager
{
public:
   static VaResourceManager& Instance();

   bool LoadResources(const std::string& aResourceFileName);

private:
   VaResourceManager() {}
};

} // namespace vespa
#endif
