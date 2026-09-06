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

#ifndef WSFPM_RF_JAMMER_HPP
#define WSFPM_RF_JAMMER_HPP

#include "wsf_mil_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_Transmitter.hpp"
#include "WsfPM_Weapon.hpp"
#include "WsfPProxyNode.hpp"

class WSF_MIL_PARSER_EXPORT WsfPM_RF_Jammer : public WsfPM_Weapon
{
public:
   WsfPM_RF_Jammer() {}
   WsfPM_RF_Jammer(const WsfPProxyNode& aNode)
      : WsfPM_Weapon(aNode)
   {
   }
   WsfPM_RF_Jammer(const WsfPM_Weapon& aWpn)
      : WsfPM_Weapon(aWpn)
   {
   }

   class Mode : public WsfPProxyNode
   {
   public:
      Mode() {}
      Mode(const WsfPProxyNode& aNode)
         : WsfPProxyNode(aNode)
      {
      }

      // todo, should we check xmtrAntenna first
      WsfPM_Transmitter transmitter() const;
   };

   typedef WsfPM_ObjectMapT<Mode> ModeMap;
   ModeMap                        modes() const { return ModeMap(*this + "mode"); }
   Mode                           getTemplate() const { return Mode(*this + "template"); }
};

#endif
