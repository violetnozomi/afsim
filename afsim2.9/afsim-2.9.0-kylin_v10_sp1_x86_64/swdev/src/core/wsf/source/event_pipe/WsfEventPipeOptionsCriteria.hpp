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

#ifndef WSFEVENTPIPEOPTIONSCRITERIA_HPP
#define WSFEVENTPIPEOPTIONSCRITERIA_HPP

class WsfPlatform;
#include "WsfStringId.hpp"

// A criteria to use when setting detail level for platforms
struct WsfEventPipeOptionsCriteria
{
   enum Kind
   {
      cPLATFORM_NAME,
      cPLATFORM_TYPE,
      cCATEGORY_NAME,
      cSIDE_NAME
   };
   bool        Matches(const WsfPlatform& aPlatform);
   Kind        mKind;
   WsfStringId mText;
};

#endif
