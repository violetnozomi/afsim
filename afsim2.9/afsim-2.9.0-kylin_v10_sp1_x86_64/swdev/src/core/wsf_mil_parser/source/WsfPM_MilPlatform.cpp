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

#include "WsfPM_MilPlatform.hpp"

#include "WsfPProxyStructType.hpp"

WsfPM_MilPlatform::WsfPM_MilPlatform() {}

WsfPM_MilPlatform::WsfPM_MilPlatform(const WsfPProxyNode& aNode)
   : WsfPM_Platform(aNode)
{
}

WsfPM_WeaponMap WsfPM_MilPlatform::Weapons() const
{
   return WsfPM_WeaponMap(*this + "weapons");
}

WsfPProxyNode WsfPM_MilPlatform::FindPart(const std::string& aName) const
{
   WsfPM_ObjectMap partMaps[6] = {WsfPM_ObjectMap(*this + "comms"),
                                  WsfPM_ObjectMap(*this + "movers"),
                                  WsfPM_ObjectMap(*this + "weapons"),
                                  WsfPM_ObjectMap(*this + "sensors"),
                                  WsfPM_ObjectMap(*this + "processors"),
                                  WsfPM_ObjectMap(*this + "fuels")};
   for (int i = 0; i < 6; ++i)
   {
      WsfPProxyNode node = partMaps[i].Find(aName);
      if (node)
         return node;
   }
   return WsfPProxyNode();
}

WsfPM_ObjectMap WsfPM_MilPlatform::GetTypeObjectMap(const WsfPProxyStructType& aProxyStruct)
{
   WsfPM_ObjectMap typeObjMap = WsfPM_Platform::GetTypeObjectMap(aProxyStruct);

   if (!typeObjMap.IsValid())
   {
      if (aProxyStruct.IsOfBasicType("Weapon"))
      {
         typeObjMap = WsfPM_ObjectMap(Weapons());
      }
   }

   return typeObjMap;
}
