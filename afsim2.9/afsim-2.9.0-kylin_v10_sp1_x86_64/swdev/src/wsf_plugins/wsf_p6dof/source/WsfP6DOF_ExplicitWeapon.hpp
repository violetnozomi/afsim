// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFP6DOF_EXPLICITWEAPON_HPP
#define WSFP6DOF_EXPLICITWEAPON_HPP

#include "wsf_p6dof_export.h"

#include <list>

#include "WsfExplicitWeapon.hpp"

// WsfP6DOF_ExplicitWeapon extends WsfExplicitWeapon to allow P6DOF-based subobjects
// to be fired as explicit weapons.

class WSF_P6DOF_EXPORT WsfP6DOF_ExplicitWeapon : public WsfExplicitWeapon
{
public:
   explicit WsfP6DOF_ExplicitWeapon(WsfScenario& aScenario);

   WsfWeapon* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

protected:
   std::list<std::string> mSubobjectList;
};

#endif
