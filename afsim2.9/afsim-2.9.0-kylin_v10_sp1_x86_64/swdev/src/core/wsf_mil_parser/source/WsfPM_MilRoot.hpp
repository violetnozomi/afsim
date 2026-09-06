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

#ifndef WSFPM_MILROOT_HPP
#define WSFPM_MILROOT_HPP

#include "wsf_mil_parser_export.h"

#include "WsfPM_MilPlatform.hpp"
#include "WsfPM_Root.hpp"
#include "WsfPM_Weapon.hpp"
#include "WsfPProxyNode.hpp"

class WsfPProxy;

class WSF_MIL_PARSER_EXPORT WsfPM_MilRoot : public WsfPM_Root
{
public:
   WsfPM_MilRoot();
   WsfPM_MilRoot(WsfPProxyNode aRootNode);
   WsfPM_MilRoot(WsfPProxy* aProxy);

   WsfPM_WeaponMap      weaponTypes() const { return WsfPM_WeaponMap(*this + "weaponType"); }
   WsfPM_MilPlatformMap platforms() const { return WsfPM_PlatformMap(*this + "platform"); }
};

#endif
