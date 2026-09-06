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

#ifndef WSFHIGHENERGYLASER_HPP
#define WSFHIGHENERGYLASER_HPP

#include "wsf_mil_export.h"

#include <mutex>

class UtInput;
#include "WsfAtmosphericTurbulence.hpp"
#include "WsfDirectedEnergyWeapon.hpp"
#include "WsfException.hpp"
class WsfGeoPoint;
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"

/**
   This implementation is decoupled from
   the weapon class to handle only laser-related effects, including propagation and
   engagement.
*/
class WSF_MIL_EXPORT WsfHighEnergyLaser : public WsfDirectedEnergyWeapon::Beam
{
public:
   WsfHighEnergyLaser(WsfScenario& aScenario);
   WsfHighEnergyLaser(const WsfHighEnergyLaser& aSrc);
   WsfHighEnergyLaser& operator=(const WsfHighEnergyLaser&) = delete;
   ~WsfHighEnergyLaser() override                           = default;

   WsfDirectedEnergyWeapon::Beam* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   virtual bool Initialize(WsfDirectedEnergyWeapon* aWeaponPtr);

   virtual void OutputConfiguration() const {};

   WsfDirectedEnergyWeapon* GetWeapon() const { return mWeaponPtr; }

   // Controlling methods for the laser.
   virtual void BeginLasing(double aSimTime, size_t aTargetIndex, const std::string& aTargetOffsetName = "");

   void UpdateLasing(double aSimTime); // same track as before

   virtual void EndLasing(double aSimTime);

   //! Return whether the weapon is currently lasing.
   bool IsLasing() const { return mIsLasing; }

   //! Return the incidence angle of the laser on the target.
   double GetIncidenceAngle() const override { return mIncidenceAngle; }

   //! Return whether the HEL-target interaction results in a target occlusion.
   //! @note For this result to be valid, either there must be an active engagement
   //! (you've called Fire(...), or you first call UpdateTargetGeometry(...).
   virtual bool TargetOccluded() const { return mTargetOccluded; }

   //! Test the laser against a specific target without actually engaging.
   //! @param aDuration A specific duration to lase.
   //! @param aTargetIndex A target index against which to laser.
   //! @param aTargetOffset The intended target offset to
   //! @return Whether the test was successful.
   virtual bool TestLasing(double aDuration, size_t aTargetIndex, const std::string& aTargetOffset = "")
   {
      return false;
   }

   //! Test the laser against a perceived target in a particular location
   //! without actually engaging.
   //! @param aDuration A specific duration to lase.
   //! @param aSourceLocation The laser's location.
   //! @param aTargetLocation The perceived target location.
   //! @return Whether the test was successful.
   virtual bool TestLasing(double aDuration, const WsfGeoPoint& aSourceLocation, const WsfGeoPoint& aTargetLocation)
   {
      return false;
   }

   //! Determine from the given geometry and desired total energy the required duration.
   //! @param aEnergy The total energy on target.
   //! @param aSourceLocation The laser's location.
   //! @param aTargetLocation The perceived target location.
   //! @return The required duration for lasing, or 0.0 if unsuccessful.
   virtual double RequiredDurationEnergy(double aEnergy, const WsfGeoPoint& aSourceLocation, const WsfGeoPoint& aTargetLocation)
   {
      return 0.0;
   }

   //! Determine from the given geometry and desired energy density the required duration.
   //! @param aEnergyDensity The energy density on target.
   //! @param aSourceLocation The laser's location.
   //! @param aTargetLocation The perceived target location.
   //! @return The required duration for lasing, or 0.0 if unsuccessful.
   virtual double RequiredDurationFluence(double             aEnergyDensity,
                                          const WsfGeoPoint& aSourceLocation,
                                          const WsfGeoPoint& aTargetLocation)
   {
      return 0.0;
   }

   //! Determine from the given geometry and desired total energy the required duration.
   //! @param aEnergy The total energy on target.
   //! @param aTargetIndex A target index against which to laser.
   //! @param aTargetOffset The intended target offset to
   //! @return The required duration for lasing, or 0.0 if unsuccessful.
   virtual double RequiredDurationEnergy(double aEnergy, int aTargetIndex, const std::string& aTargetOffset = "")
   {
      return 0.0;
   }

   //! Determine from the given geometry and desired energy density the required duration.
   //! @param aEnergyDensity The energy density on target.
   //! @param aTargetIndex A target index against which to laser.
   //! @param aTargetOffset The intended target offset to
   //! @return The required duration for lasing, or 0.0 if unsuccessful.
   virtual double RequiredDurationFluence(double aEnergyDensity, size_t aTargetIndex, const std::string& aTargetOffset = "")
   {
      return 0.0;
   }

   //! Set up all engagement geometry in preparation for the laser propagation calculation.
   //! This includes weapon and target positioning and optional raycasting to a target offset with specified jitter.
   //! @param aSimTime the current simulation time.
   //! @param aTargetIndex The target index against which the laser will fire.
   //! @param aTargetOffset The desired target offset if any (used for raycasting).
   //! @param aAddJitter The jitter offset calculated instantaneously (used in raycasting).
   //! @note This is also used to perform an independent raycasting calculation to determine target occlusion.
   virtual bool UpdateEngagementGeometry(double             aSimTime,
                                         size_t             aTargetIndex,
                                         const std::string& aTargetOffset = "",
                                         bool               aAddJitter    = true);

   //! Get the max. radius about which the laser is effective.
   //! @note If the value is zero, the damage radius is ignored, and the spot edge is used instead.
   double GetDamageRadius() const { return mDamageRadius; }

   //! Get the max. radius about which the laser is effective (default value).
   //! @note If the value is zero, the damage radius is ignored, and the spot edge is used instead.
   //! @note This accessor is meant to support legacy operation, where only one damage radius is specified.
   double GetDefaultDamageRadius() const { return mDefaultDamageRadius; }

   //! Set the max. radius about which the laser is effective.
   //! @note This value is usually set in the framework based on values defined in WsfHEL_Lethality.
   virtual void SetDamageRadius(double aDamageRadius) { mDamageRadius = aDamageRadius; }

   virtual void SetAtmosphericStructure(const std::string& aAtmosphericStructure)
   {
      mTurbulenceModel.SetCN2Form(aAtmosphericStructure);
   }

   //! Get the expected value for the jitter of the HEL on target.
   double GetJitter() const { return mJitterExpectedValue; }

   //! Set the expected value for the jitter of the HEL on target.
   void SetJitter(double aJitter) { mJitterExpectedValue = aJitter; }

   //! Get the current target index (valid only while IsLasing is true).
   size_t GetTargetIndex() const { return mTargetIndex; }

   //! Get the current range to target (only valid while IsLasing is true).
   double GetTargetRange() const { return mTargetRange; }

   //! Set the beam quality wrt a perfect Gaussian beam.
   void SetBeamQuality(double aBeamQuality) { mBeamQuality = aBeamQuality; }

   //! Get the strehl due to window effects (default is 1.0 [no effect]).
   virtual double GetWindowStrehl() const { return 1.0; }

   //! Set the window strehl (should be between 0 and 1).
   virtual void SetWindowStrehl(double aWindowStrehl) {}

   //! Get the aero strehl (strehl from aerooptics; should be between 0 and 1).
   virtual double GetAeroStrehl() const { return 1.0; }

   //! Set the areo strehl (strehl from aerooptics; should be between 0 and 1).
   virtual void SetAeroStrehl(double aAeroStrehl) {}

   //! Get the total rms strehl from all sources.
   virtual double GetSystemStrehl() const { return 1.0; }

   //! Set the total rms system strehl.
   virtual void SetSystemStrehl(double aSystemStrehl) {}

   //! Get the system transmission, if available (default is 1.0).
   virtual double GetSystemTransmission() const { return 1.0; }

   WsfSimulation* GetSimulation() const;

protected:
   void Propagate(double aSimTime) override = 0; // Pure virtual; define in child classes

   WsfDirectedEnergyWeapon* mWeaponPtr; //! The weapon object associated with the HEL.
   bool                     mSysInit;

   bool   mCalculateIncidence;
   double mIncidenceAngle;

   size_t      mTargetIndex;
   WsfStringId mTargetOffsetName;

   bool   mTargetOccluded;
   double mTargetRange;

   bool   mIsLasing;
   bool   mDebug;
   double mLastUpdateTime;
   double mDamageRadius;
   double mDefaultDamageRadius;
   double mJitterExpectedValue;
   double mBeamQuality;

   wsf::AtmosphericTurbulence mTurbulenceModel;
};

class WsfHighEnergyLaserTypes : public WsfObjectTypeList<WsfHighEnergyLaser>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfHighEnergyLaserTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfHighEnergyLaserTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfHighEnergyLaserTypes(WsfScenario& aScenario);
   ~WsfHighEnergyLaserTypes() override = default;
   WsfHighEnergyLaser* Create(const std::string& aName) const;
};

#endif
