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

#ifndef WSFDISDIRECTEDENERGYFIRE_HPP
#define WSFDISDIRECTEDENERGYFIRE_HPP

#include "wsf_mil_export.h"

#include <map>

#include "DisDirectedEnergyFire.hpp"
#include "DisEventId.hpp"

class WsfDirectedEnergyWeapon;
class WsfDisInterface;
class WsfMilDisInterface;
class WsfMilExtInterface;
class WsfWeaponEngagement;

class WSF_MIL_EXPORT WsfDisDirectedEnergyFire : public DisDirectedEnergyFire
{
public:
   WsfDisDirectedEnergyFire(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   WsfDisDirectedEnergyFire(WsfDisInterface* aInterfacePtr);

   ~WsfDisDirectedEnergyFire() override = default;

   int Process() override;

   bool Exists(const DisEventId& aEventId) const;

   static void Reset();

private:
   WsfDisInterface*    mInterfacePtr;
   WsfMilDisInterface* mMilInterfacePtr;
   WsfMilExtInterface* mExtMilPtr;
};

#endif
