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

#ifndef WSFEW_EFFECTMANAGER_HPP
#define WSFEW_EFFECTMANAGER_HPP

#include <list>
#include <map>
#include <memory>
#include <string>

class WsfFalseTarget;
class WsfFalseTargetScreener;
#include "wsf_mil_export.h"
class WsfSimulation;

//! Manages simulation-wide variables used by EW Effects
class WsfEW_EffectManager
{
public:
   static WSF_MIL_EXPORT WsfEW_EffectManager* Find(WsfSimulation& aSimulation);

   WsfEW_EffectManager() = default;
   ~WsfEW_EffectManager();

   void            AddFalseTargetType(WsfFalseTarget* aFalseTarget);
   WsfFalseTarget* FindFalseTargetType(const std::string& aTypeName) const;

   void AddFalseTargetScreener(WsfFalseTarget* aFalseTarget, WsfFalseTargetScreener* aFT_Screener);
   void RemoveFalseTargetScreener(WsfFalseTargetScreener* aFT_Screener);
   void RemoveFalseTargetScreener(WsfFalseTarget* aFalseTargetPtr, WsfFalseTargetScreener* aFT_Screener);

private:
   using FT_Screeners = std::list<std::pair<WsfFalseTargetScreener*, WsfFalseTarget*>>;
   using FT_Types     = std::map<std::string, std::unique_ptr<WsfFalseTarget>>;

   FT_Types     mFalseTargetTypes; //! Managed
   FT_Screeners mFT_Screeners;
};

#endif
