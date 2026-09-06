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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFP6DOF_OBJECTMANAGER_HPP
#define WSFP6DOF_OBJECTMANAGER_HPP

#include "UtCallbackHolder.hpp"
#include "WsfSimulationExtension.hpp"

class WsfPlatform;
class WsfP6DOF_TypeManager;
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;

class WsfP6DOF_ObjectManager : public WsfSimulationExtension
{
public:
   explicit WsfP6DOF_ObjectManager(WsfP6DOF_TypeManager* aTypeManager);
   ~WsfP6DOF_ObjectManager() override = default;

   //! Called by the simulation when this extension has been added.
   void AddedToSimulation() override;

   //! Called by the simulation when it is being initialized.
   bool Initialize() override;

   //! Called by the simulation when it is about to be started.
   void Start() override;

   //! Called when the simulation is complete
   void Complete(double aSimTime) override;

   // Called when the simulation is initializing.
   // This is guaranteed to be called once even if the simulation
   // initialized prior to adding the extension.
   bool PrepareExtension() override;

   // PlatformAdded handler
   void PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr);

   // PlatformDeleted handler
   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

   // WeaponFired handler
   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   // WeaponReloadEnded handler
   void WeaponReloadEnded(double aSimTime, WsfWeapon* aWeaponPtr);

private:
   UtCallbackHolder      mCallbacks;
   WsfP6DOF_TypeManager* mWsfTypeManagerPtr;
};

#endif
