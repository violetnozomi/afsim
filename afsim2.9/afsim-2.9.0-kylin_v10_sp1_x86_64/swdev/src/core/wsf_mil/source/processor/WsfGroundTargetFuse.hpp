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

#ifndef WSFGROUNDTARGETFUSE_HPP
#define WSFGROUNDTARGETFUSE_HPP

#include "wsf_mil_export.h"

#include "WsfWeaponFuse.hpp"

//! A specialization of WsfWeaponFuse that sets defaults appropriate for
//! efficient Ground-Target use.  All functionality is in the base class,
//! so see WsfWeaponFuse for details.
class WSF_MIL_EXPORT WsfGroundTargetFuse : public WsfWeaponFuse
{
public:
   WsfGroundTargetFuse(WsfScenario& aScenario);
   WsfProcessor* Clone() const override;
   bool          ProcessInput(UtInput& aInput) override;
};

#endif
