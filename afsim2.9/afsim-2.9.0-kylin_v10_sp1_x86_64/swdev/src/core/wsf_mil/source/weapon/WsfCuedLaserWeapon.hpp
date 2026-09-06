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

#ifndef WSFCUEDLASERWEAPON_HPP
#define WSFCUEDLASERWEAPON_HPP

#include "wsf_mil_export.h"

#include "UtInputString.hpp"
#include "UtOptional.hpp"
#include "WsfBeamDirector.hpp"
#include "WsfEvent.hpp"
#include "WsfHighEnergyLaser.hpp"
#include "WsfLaserWeapon.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
class WsfWeaponEngagement;

//! The WsfCuedLaserWeapon is a subclass of WsfLaserWeapon that incorporates an integral beam director.
//! When fired at a track, the WsfCuedLaserWeapon will automatically acquire the associated target with the beam director.
//! Then, if the beam director is able to cue to the target's region of interest, it will fire.
//! If the beam director breaks lock, the laser will automatically stop firing.
class WSF_MIL_EXPORT WsfCuedLaserWeapon : public WsfLaserWeapon
{
public:
   WsfCuedLaserWeapon(WsfScenario& aScenario);
   WsfCuedLaserWeapon& operator=(const WsfCuedLaserWeapon&) = delete;
   ~WsfCuedLaserWeapon() override                           = default;

   WsfWeapon* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(double aSimTime) override;
   bool Initialize2(double aSimTime) override;

   void TurnOff(double aSimTime) override;

   WsfBeamDirector* GetBeamDirector() const { return mBeamDirectorPtr; }

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   // Todo; pause/resume are not necessary; just call fire again after CeaseFire call.
   void CeaseFire(double aSimTime) override;

   virtual void PauseFire(double aSimTime);

   virtual void ResumeFire(double aSimTime);

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

protected:
   WsfCuedLaserWeapon(const WsfCuedLaserWeapon& aSrc);

   bool EndShot(double aSimTime) override;

   bool HasCurrentEngagement() const;

   // The laser system's aggregated objects:
   WsfBeamDirector* mBeamDirectorPtr;

private:
   void UpdateEngagement(double aSimTime);

   WsfStringId mBeamDirectorName;
   WsfTrack mBeamDirectorTrack; // The track from the beam director; this should not be confused with queued engagement tracks.
   WsfTrack                  mCurrentTargetTrack;
   std::string               mBeamDirectorInputs;
   ut::optional<std::string> mBeamDirector;
};

#endif
