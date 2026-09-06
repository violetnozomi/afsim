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

#ifndef WSFWEAPONLAR_HPP
#define WSFWEAPONLAR_HPP

#include "wsf_mil_export.h"

class UtInput;
#include "WsfObject.hpp"
class WsfSimulation;

/**
   An abstract class for specifying Weapon Launch Acceptable Regions (LARs).
   The class must be extended further to provide whatever information is
   needed to launch a weapon against a target of interest.  The information
   required will vary depending upon the type of weapon that is to be employed.
*/

class WSF_MIL_EXPORT WsfWeaponLAR : public WsfObject
{
public:
   WsfWeaponLAR();
   ~WsfWeaponLAR() override             = default;
   WsfWeaponLAR* Clone() const override = 0;

   static std::string GetTypeString() { return "WSF_WEAPON_LAR"; }

   /** Returns 'true' if debugging is enabled for this instance */
   bool DebugEnabled() const { return mDebugEnabled; }
   void SetDebugEnabled(bool aDebugEnabled) { mDebugEnabled = aDebugEnabled; }

   bool         ProcessInput(UtInput& aInput) override;
   virtual bool Initialize(WsfSimulation& aSimulation);

   static bool LoadInstance(UtInput& aInput, WsfWeaponLAR*& aLARPtr);

   WsfSimulation* GetSimulation() const { return mSimulationPtr; }

protected:
   // Copy constructor (for Clone())
   WsfWeaponLAR(const WsfWeaponLAR& aSrc) = default;

   // Prevent use of operator= by declaring, but not defining.
   WsfWeaponLAR& operator=(const WsfWeaponLAR& aRhs) = delete;

private:
   WsfSimulation* mSimulationPtr;
   bool           mDebugEnabled;
};

#endif
