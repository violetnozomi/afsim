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

#ifndef WSFATG_WEAPONLAR_HPP
#define WSFATG_WEAPONLAR_HPP

#include "wsf_mil_export.h"

class UtInputBlock;
#include "WsfStringId.hpp"
#include "WsfWeaponLAR.hpp"
class WsfZone;

/**
   A class for specifying Weapon Launch Acceptable Regions (LARs)
   for Air-to-Ground weapons.
*/

class WSF_MIL_EXPORT WsfATG_WeaponLAR : public WsfWeaponLAR
{
public:
   WsfATG_WeaponLAR();
   WsfATG_WeaponLAR(const WsfATG_WeaponLAR& aSrc);

   static WsfStringId GetTypeId();

   ~WsfATG_WeaponLAR() override;
   WsfWeaponLAR* Clone() const override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          Initialize(WsfSimulation& aSimulation) override;

   void ProcessInputBlock(UtInputBlock& aInputBlock);

   double LaunchSpeed() const { return mLaunchSpd; }

   // This function indicates whether the target is inside
   // the LAR for this weapon.  If the launch is off design
   // conditions, an offset will be applied to the input
   // target position to account for the LAR shift.  This
   // process is an approximation only, and cannot be relied
   // upon to be correct for far off nominal conditions.
   virtual bool IsInside(const double aTargetPointWCS[3],
                         const double aLaunchPointWCS[3],
                         const double aLaunchHeading,
                         const double aLaunchSpeed,
                         const double aDeltaAltitude,
                         const double aLaunchGamma) const;

   /** Delta Altitude is defined as Launcher Altitude minus Target Altitude */
   double         DeltaAltitude() const { return mDeltaAlt; }
   const WsfZone* GetZone() const { return mZonePtr; }

   // Needed for sorting LARs by altitude...
   virtual bool operator<(const WsfATG_WeaponLAR& aRhs) const { return mDeltaAlt < aRhs.mDeltaAlt; }

protected:
   WsfATG_WeaponLAR& operator=(const WsfATG_WeaponLAR& aRhs);

private:
   WsfZone* mZonePtr;
   double   mDeltaAlt;
   double   mLaunchSpd;
   double   mSens10mAlt;
   double   mSens10msVel;
   double   mSensGradient;
};

#endif
