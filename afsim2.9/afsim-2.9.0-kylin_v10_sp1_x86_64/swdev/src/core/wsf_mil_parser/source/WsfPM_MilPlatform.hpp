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

#ifndef WSFPM_MILPLATFORM_HPP
#define WSFPM_MILPLATFORM_HPP

#include "wsf_mil_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_Platform.hpp"
#include "WsfPM_Weapon.hpp"

class WSF_MIL_PARSER_EXPORT WsfPM_MilPlatform : public WsfPM_Platform
{
public:
   WsfPM_MilPlatform();
   WsfPM_MilPlatform(const WsfPProxyNode& aNode);

   WsfPM_WeaponMap Weapons() const;
   WsfPProxyNode   FindPart(const std::string& aName) const override;

private:
   WsfPM_ObjectMap GetTypeObjectMap(const WsfPProxyStructType& aProxyStruct) override;
};

typedef WsfPM_ObjectMapT<WsfPM_MilPlatform> WsfPM_MilPlatformMap;

#endif
