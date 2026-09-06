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

#ifndef WSFAIRMOVER_HPP
#define WSFAIRMOVER_HPP

#include "wsf_export.h"

class UtInput;
#include "WsfWaypointMover.hpp"

//! WsfAirMover is a specialization of WsfWaypointMover for aircraft.
//! It calculates a Height above Terrain (HAT), and uses this information in
//! ground collision calculations.  It implements a damage assessment based
//! on normal impact velocity with terrain and a maximum impact velocity,
//! provided by the user input.  It allows for take-offs, and for landings,
//! if the normal of the landing velocity is less than the allowed impact
//! velocity.  It does not implement terrain following or terrain avoidance.

class WSF_EXPORT WsfAirMover : public WsfWaypointMover
{
public:
   WsfAirMover() = delete;
   WsfAirMover(const WsfScenario& aScenario);
   ~WsfAirMover() override;

   WsfMover* Clone() const override;
   bool      Initialize(double aSimTime) override;
   bool      ProcessInput(UtInput& aInput) override;

   WsfSpatialDomain GetSpatialDomain() const override { return WSF_SPATIAL_DOMAIN_AIR; }

   double GetThrottlePosition() override;

   bool DestIsInAir();

protected:
   void         PostMoveUpdate(double aSimTime) override;
   virtual void TakeOff(double aSimTime);
   virtual void Land(double aSimTime);
   virtual void CrashIntoGround(double aSimTime);
   virtual void AssessDamage(double aSimTime);

private:
   double mMaxImpactSpeed;
   bool   mCrashedIntoGround;
   bool   mIsOnRunWay;
};

#endif
