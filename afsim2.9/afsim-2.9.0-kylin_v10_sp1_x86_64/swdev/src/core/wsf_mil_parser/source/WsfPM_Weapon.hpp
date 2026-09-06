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

#ifndef WSFPM_WEAPON_HPP
#define WSFPM_WEAPON_HPP

#include "wsf_mil_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"

class WSF_MIL_PARSER_EXPORT WsfPM_Weapon : public WsfPM_PlatformPart
{
public:
   WsfPM_Weapon() = default;
   WsfPM_Weapon(const WsfPProxyNode& aNode)
      : WsfPM_PlatformPart(aNode)
   {
   }

   std::string LaunchedPlatformType() const { return GetValue().GetAttr("launchedPlatformType").ValueToString(); }
};

typedef WsfPM_ObjectMapT<WsfPM_Weapon> WsfPM_WeaponMap;

#endif
