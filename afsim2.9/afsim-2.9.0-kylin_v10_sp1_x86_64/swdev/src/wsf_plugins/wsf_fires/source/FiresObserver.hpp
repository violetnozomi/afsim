// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef FIRES_OBSERVER_HPP
#define FIRES_OBSERVER_HPP

#include "wsf_fires_export.h"

#include <map>

#include "UtCallbackHolder.hpp"
class UtInput;
class WsfDisFire;
class WsfDisSignal;
#include "WsfDraw.hpp"
class WsfPlatform;
class WsfSimulation;
class WsfTrack;
class WsfWeaponEngagement;

namespace Fires
{
//! Provide callbacks to perform custom actions in response to events in the simulation.
//! Currently custom drawing is performed in response to WeaponFired and WeaponTerminated
//! events.
class WSF_FIRES_EXPORT Observer
{
public:
   explicit Observer(WsfSimulation* aSimPtr);
   virtual ~Observer();

   void WeaponFired(double aSimTime, const WsfWeaponEngagement* aEngagementPtr, const WsfTrack* aTargetTrackPtr);

   void WeaponTerminated(double aSimTime, const WsfWeaponEngagement* aEngagementPtr);

private:
   WsfSimulation*                 mSimPtr;
   WsfDraw                        mDraw;
   std::map<unsigned int, size_t> mEngagementMap;
   UtCallbackHolder               mCallbacks;
};
}; // namespace Fires
#endif
