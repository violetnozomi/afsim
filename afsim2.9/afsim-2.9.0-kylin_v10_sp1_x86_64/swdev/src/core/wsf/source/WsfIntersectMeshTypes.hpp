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

#ifndef WSFINTERSECTMESHTYPES_HPP
#define WSFINTERSECTMESHTYPES_HPP

#include "wsf_export.h"

#include "WsfIntersectMesh.hpp"
#include "WsfObjectTypeList.hpp"

class WsfIntersectMeshTypes : public WsfObjectTypeList<WsfIntersectMesh>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfIntersectMeshTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfIntersectMeshTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfIntersectMeshTypes(WsfScenario& aScenario);
};

#endif
