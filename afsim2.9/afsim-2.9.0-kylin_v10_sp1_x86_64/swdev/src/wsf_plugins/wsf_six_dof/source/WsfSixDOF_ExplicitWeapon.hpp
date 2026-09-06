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

#ifndef WSFSIXDOFEXPLICITWEAPON_HPP
#define WSFSIXDOFEXPLICITWEAPON_HPP

#include "wsf_six_dof_export.h"

#include <list>

#include "WsfExplicitWeapon.hpp"

namespace wsf
{
namespace six_dof
{
// ExplicitWeapon extends WsfExplicitWeapon to allow SixDOF subobjects
// to be fired as explicit weapons.
class WSF_SIX_DOF_EXPORT ExplicitWeapon : public WsfExplicitWeapon
{
public:
   explicit ExplicitWeapon(WsfScenario& aScenario);

   WsfWeapon* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

protected:
   std::list<std::string> mSubobjectList;
};
} // namespace six_dof
} // namespace wsf

#endif
