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

#include "WsfExplicitWeaponEffects.hpp"

#include <iostream>

#include "UtAzElLookup.hpp"
#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfPkTable.hpp"
#include "WsfPlatform.hpp"
#include "WsfWeaponEngagement.hpp"


// =================================================================================================
WsfExplicitWeaponEffects::WsfExplicitWeaponEffects(WsfScenario& aScenario)
   : WsfWeaponEffects(aScenario)
   , mMaxRadius(0.0)
   , mRadiusSquared(0.0)
{
   SetUseInterceptPk();
}

// =================================================================================================
// virtual
WsfWeaponEffects* WsfExplicitWeaponEffects::Clone() const
{
   return new WsfExplicitWeaponEffects(*this);
}

// =================================================================================================
//! Get the radius squared beyond which the weapon has no lethal effect.
// virtual
double WsfExplicitWeaponEffects::MaximumEffectiveRadiusSquared() const
{
   return mMaxRadius * mMaxRadius;
}

// =================================================================================================
//! Get the radius beyond which the weapon has no lethal effect.
// virtual
double WsfExplicitWeaponEffects::MaximumEffectiveRadius() const
{
   return mMaxRadius;
}

// =================================================================================================
//! Method will read a data stream, and initialize any data required by the object
//! prior to first use.  The type loader must call this method on
//! each object after the data stream input is read.
// virtual
bool WsfExplicitWeaponEffects::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "maximum_radius")
   {
      myCommand = true;
      aInput.ReadValueOfType(mMaxRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mMaxRadius, 0.0);
   }
   else
   {
      myCommand = WsfWeaponEffects::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// This overrides the base class implementation (which does nothing).
// IsVulnerable should always be called prior to calling this method.
// If it passes that then we know the target is within the maximum
// radius, and thus the Pk is 1.
// virtual
void WsfExplicitWeaponEffects::CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr)
{
   SetInterceptPk(1.0);

   if (GetPkTable() == nullptr)
   {
      if (DebugEnabled())
      {
         ut::log::debug() << "WsfExplicitWeaponEffects::CalcInterceptPk() did not look up engagement conditions, as "
                             "the pk_table pointer was null.";
      }
   }
   else
   {
      UtAzElLookup lookup;
      double       az;
      double       el;
      double       mslSpd;
      double       tgtSpeed;
      CalcPkParameters(aTargetPtr, az, el, mslSpd, tgtSpeed);
      // Here we assume weapon effect is symmetrical with azimuth.
      // string tgtTypeName = aTargetPtr->GetTypeId();
      WsfPk::Table* tPtr                 = GetPkTable();
      double        interceptPkThreshold = tPtr->LookUp(aTargetPtr->GetTypeId(), fabs(az), el, mslSpd, tgtSpeed);
      SetInterceptPk(interceptPkThreshold);
      if (DebugEnabled())
      {
         auto out =
            ut::log::debug() << "WsfExplicitWeaponEffects::CalcInterceptPk() computed engagement conditions of:";
         out.AddNote() << "Target Type: " << aTargetPtr->GetTypeId();
         out.AddNote() << "fabs() Azimuth: " << fabs(az) * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "fabs() Elevation: " << el * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "Weapon Speed: " << mslSpd * UtMath::cFT_PER_M << " ft/s";
         out.AddNote() << "Target Speed: " << tgtSpeed * UtMath::cFT_PER_M << " ft/s";
         out.AddNote() << "RESULTING PK THRESHOLD: " << interceptPkThreshold;
      }
   }
}

// =================================================================================================
//! In addition to the base class checks, this also excludes
//! platforms that are outside of the lethal radius.
// virtual
bool WsfExplicitWeaponEffects::IsVulnerable(WsfPlatform* aTargetPtr)
{
   // Preliminary quick-and-dirty test:
   bool vulnerable = WsfWeaponEffects::IsVulnerable(aTargetPtr);

   if ((vulnerable) && (mMaxRadius > 0.0))
   {
      // A secondary test to see if the weapon and target are within lethal range of each other:
      WsfPlatform* weaponPtr = GetEngagement()->GetWeaponPlatform();
      if (weaponPtr != nullptr)
      {
         double missDistance = GetEngagement()->GetMissDistance(aTargetPtr);
         mRadiusSquared      = missDistance * missDistance;
         if (mRadiusSquared > (mMaxRadius * mMaxRadius))
         {
            vulnerable = false;
         }
      }
   }
   else
   {
      mRadiusSquared = 0.0;
   }

   return vulnerable;
}

// =================================================================================================
//! Utility method for use in derived classes.
//! Calculates opening or closing speed and approach angle from true platform velocities.
//! Does not calculate a miss distance, as the weapon_fuse does this.  Since the
//! method does not have actual locations, it cannot give them signs (approach - recede).
//! @param aTargetPtr Platform to consider as the target for this engagement.
//! @param aRangeRate Calculated relative speed for a hit-to-kill determination.
//! @param aApproachAngle Calculated impact angle.  0.0 = Grazing bump, 180 = head-on.

void WsfExplicitWeaponEffects::CalcInterceptGeometry(WsfPlatform* aTargetPtr, double& aRangeRate, double& aApproachAngle)
{
   double targetVel[3];
   aTargetPtr->GetVelocityWCS(targetVel);

   double interceptorVel[3];
   GetEngagement()->GetWeaponPlatform()->GetVelocityWCS(interceptorVel);

   CalcInterceptGeometry(interceptorVel, targetVel, aRangeRate, aApproachAngle);
}

// =================================================================================================
// private
void WsfExplicitWeaponEffects::CalcInterceptGeometry(const double aInterceptorVel[],
                                                     const double aTargetVel[],
                                                     double&      aRangeRate,
                                                     double&      aApproachAngle)
{
   double Va[3];
   double Vb[3];

   UtVec3d::Set(Va, aInterceptorVel);
   UtVec3d::Set(Vb, aTargetVel);

   double difference[3];
   UtVec3d::Subtract(difference, Va, Vb);
   aRangeRate = UtVec3d::Magnitude(difference);

   UtVec3d::Normalize(Va);
   UtVec3d::Normalize(Vb);

   double cosine = UtVec3d::DotProduct(Va, Vb);
   if (cosine > 1.0)
      cosine = 1.0;
   if (cosine < -1.0)
      cosine = -1.0;

   aApproachAngle = acos(cosine);
}

// =================================================================================================
// virtual protected
//! Capture the geometry values needed to do a Pk table lookup.
//! @param aTargetPtr Pointer to the target to be evaluated for geometry.
//! @param aAzimuth Missile-body-relative azimuth to the target.
//! @param aElevation Missile-body-relative elevation to the target.
//! @param aMissileSpeed Missile speed.
//! @param aTargetSpeed Target speed.
void WsfExplicitWeaponEffects::CalcPkParameters(WsfPlatform* aTargetPtr,
                                                double&      aAzimuth,
                                                double&      aElevation,
                                                double&      aMissileSpeed,
                                                double&      aTargetSpeed)
{
   // In this derived class, the assumption is that this is an explicit rather
   // than implicit engagement, and so the Pk determination is done at end-game.

   UtVec3dX mslLocWCS;
   UtVec3dX tgtLocWCS;

   // IMPORTANT:  The fuse PCA for target and weapon is stuffed directly into the
   // weapon engagement struct.   Do NOT get the location from the target Platform
   // or the weapon Platform, as they currently contain their flew-past location.

   GetEngagement()->GetTargetLocationWCS(aTargetPtr, tgtLocWCS.GetData());
   WsfPlatform* wpnPlatPtr = GetEngagement()->GetWeaponPlatform();
   GetEngagement()->GetWeaponLocationWCS(wpnPlatPtr, mslLocWCS.GetData());
   UtDCM    mslWCStoECS = GetEngagement()->GetWeaponPlatform()->GetDcmWCStoECS();
   UtVec3dX tgtLOS_WCS  = tgtLocWCS - mslLocWCS;
   UtVec3dX tgtLOS_ECS  = tgtLOS_WCS * mslWCStoECS;
   aAzimuth             = tgtLOS_ECS.Azimuth();
   aElevation           = tgtLOS_ECS.Elevation();
   aMissileSpeed        = GetEngagement()->GetWeaponPlatform()->GetSpeed();
   aTargetSpeed         = aTargetPtr->GetSpeed();
}
