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

#ifndef WSFSIXDOFOBJECTMANAGER_HPP
#define WSFSIXDOFOBJECTMANAGER_HPP

#include "UtCallbackHolder.hpp"
class WsfPlatform;
#include "WsfSimulationExtension.hpp"
class WsfTrack;
class WsfWeapon;
class WsfWeaponEngagement;

namespace wsf
{
namespace six_dof
{
class TypeManager;

class ObjectManager : public WsfSimulationExtension
{
public:
   explicit ObjectManager(TypeManager* aTypeManager);
   ~ObjectManager() = default;

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
   UtCallbackHolder mCallbacks;
   TypeManager*     mWsfTypeManagerPtr;
};
} // namespace six_dof
} // namespace wsf

#endif
