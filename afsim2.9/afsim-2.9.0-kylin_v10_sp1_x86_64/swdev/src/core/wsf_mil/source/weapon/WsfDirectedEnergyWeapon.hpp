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

#ifndef WSFDIRECTEDENERGYWEAPON_HPP
#define WSFDIRECTEDENERGYWEAPON_HPP

#include "wsf_mil_export.h"

#include "WsfImplicitWeapon.hpp"
#include "WsfMilComponentRoles.hpp"

//! An interface for directed energy weapons.
//! Currently directed-energy weapons are  modeled as an
//! implicit weapon with an optional "beam" object.
//! This object is suitable for HEL modeling.  For
//! HPM modeling another structure should be added
//! and used instead of the beam object.
class WSF_MIL_EXPORT WsfDirectedEnergyWeapon : public WsfImplicitWeapon
{
public:
   WsfDirectedEnergyWeapon(const WsfScenario& aScenario);
   WsfDirectedEnergyWeapon& operator=(const WsfDirectedEnergyWeapon&) = delete;
   ~WsfDirectedEnergyWeapon() override;

   WsfWeapon* Clone() const override { return new WsfDirectedEnergyWeapon(*this); }

   class Beam;

   //! Return the Spot object associated with this directed energy weapon.
   //! If this weapon is not a spot-based weapon, this method returns zero.
   Beam* GetBeam() const { return mBeamPtr; }

   void SetBeam(Beam* aBeamPtr);

   FireResult Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings) override;

   void CeaseFire(double aSimTime) override;

   //! Return whether the weapon is firing for the last updated time.
   virtual bool IsFiring() const;

   void EngagementComplete(double aSimTime, WsfWeaponEngagement* aEngagementPtr) override;

   //! @name Component infrastructure methods not provided by WsfWeapon.
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   //@}

   //! A representation of a directed energy "beam," suitable for
   //! HEL modeling.
   //! @note Directed Energy Weapons may be modeled either as single-beam
   //! or multiple beam.  This beam class assumes a single beam
   //! (such as for HEL).  Beam spots are assumed Gaussian in shape.
   class WSF_MIL_EXPORT Beam : public WsfObject
   {
   public:
      Beam();
      Beam(const Beam& aSrc);
      Beam& operator=(const Beam&) = delete;
      ~Beam() override             = default;

      bool ProcessInput(UtInput& aInput) override;

      void Initialize();

      //! Propagate the beam to the target.
      //!@note This method's signature may be changed in the future.
      virtual void Propagate(double /*aSimTime*/){}; // Should define in child classes

      //! Clone this object.
      Beam* Clone() const override { return new Beam(*this); }

      //! Return the peak fluence (i.e., the peak irradiance on target integrated over time).
      double GetPeakFluence() const { return mPeakFluence; }

      //! Set the peak fluence, in W/m^2.
      void SetPeakFluence(double aPeakFluence) { mPeakFluence = aPeakFluence; }

      //! Return the edge fluence (i.e., the irradiance at the damage radius)
      //! whichever is smaller.
      double GetEdgeFluence() const { return mEdgeFluence; }

      //! Set the edge fluence, in W/m^2.
      void SetEdgeFluence(double aEdgeFluence) { mEdgeFluence = aEdgeFluence; }

      //! Return the edge radius (i.e., the radius at which the edge fluence is calculated).
      double GetEdgeRadius() const { return mEdgeRadius; }

      //! Set the edge radius (i.e., the radius at which the edge fluence is calculated).
      void SetEdgeRadius(double aEdgeRadius) { mEdgeRadius = aEdgeRadius; }

      //! Same as GetEdgeRadius, above.
      double GetSemiMinorAxis() const { return mSpotSize; }

      //! Set the spot radius, which for an elliptical spot is the same as the semi-minor axis.
      void SetSpotRadius(double aRadius) { mSpotSize = aRadius; }

      //! Return the spot radius, which for an elliptical spot is the same as the semi-minor axis.
      double GetSpotRadius() const { return mSpotSize; }

      double GetSemiMajorAxis() const;

      //! Return the spot orientation angle
      virtual double GetIncidenceAngle() const { return mIncidenceAngle; }

      virtual void SetIncidenceAngle(double aIncidenceAngle);

      //! Return the characteristic wavelength of the beam.
      virtual double GetWavelength() const { return mWavelength; }

      //! Return the characteristic wavelength of the beam as a string in nm.
      unsigned GetWavelength_nm() const { return mWavelength_nm; }

      //! Set the characteristic wavelength.
      virtual void SetWavelength(double aWavelength);

      //! Return the diameter of the exit aperture of the beam producing the spot.
      virtual double GetApertureDiameter() const { return mApertureDiameter; } // todo deconflict with diameter

      //! Set the diameter of the aperture through which the beam exits (m^2).
      virtual void SetApertureDiameter(double aApertureDiameter) { mApertureDiameter = aApertureDiameter; }

      //! Return the initial power of the beam before any losses.
      virtual double GetInitialPower() const { return mInitialPower; }

      //! Set initial power of the beam before any losses.
      virtual void SetInitialPower(double aInitialPower) { mInitialPower = aInitialPower; }

      //! Return the average irradiance.
      double GetPeakIrradiance() const { return mPeakIrradiance; }

      //! Set the peak irradiance (W/m^2)
      void SetPeakIrradiance(double aPeakIrradiance) { mPeakIrradiance = aPeakIrradiance; }

      //! Return the average irradiance over the beam spot (W/m2).
      double GetAverageIrradiance() const { return mAverageIrradiance; }

      //! Set the average irradiance over the beam spot.
      void SetAverageIrradiance(double aAverageIrradiance) { mAverageIrradiance = aAverageIrradiance; }

      //! Return the average irradiance over the beam spot (W/m2).
      double GetEdgeIrradiance() const { return mEdgeIrradiance; }

      //! Set the average irradiance over the beam spot.
      void SetEdgeIrradiance(double aEdgeIrradiance) { mEdgeIrradiance = aEdgeIrradiance; }

      //! Return the Pulse Repetition Frequency
      virtual double GetPulseRepetitionFrequency() const { return mPulseRepetitionFrequency; }

      //! Set the pulse repetition frequency.
      virtual void SetPulseRepetitionFrequency(double aPRF) { mPulseRepetitionFrequency = aPRF; }

      //! Return the Pulse Width
      virtual double GetPulseWidth() const { return mPulseWidth; }

      //! Set the pulse width (0 == CW).
      virtual void SetPulseWidth(double aPulseWidth) { mPulseWidth = aPulseWidth; }

      virtual double GetEnergyDensity() const;

      //! Return the total energy (i.e., the average irradiance on target integrated over time and area; J).
      double GetEnergy() const { return mEnergyInSpot; }

      //! Set the energy deposited during the engagement in the beam spot (J).
      void SetEnergy(double aEnergy) { mEnergyInSpot = aEnergy; }

      //! Return the average power on target, integrated over the beam spot.
      double GetAveragePower() const { return mAveragePower; }

      //! Set the average power on target, integrated over the beam spot.
      void SetAveragePower(double aAveragePower) { mAveragePower = aAveragePower; }

      void GetOffsetLocationECS(double aLocationECS[3]) const;

      void SetOffsetLocationECS(const double aLocationECS[3]);

   private:
      double       mAveragePower;
      double       mInitialPower;
      double       mEnergyInSpot;
      double       mPeakFluence;
      double       mEdgeFluence;
      double       mPeakIrradiance;
      double       mAverageIrradiance;
      double       mEdgeIrradiance;
      double       mPulseRepetitionFrequency;
      double       mPulseWidth;
      double       mEdgeRadius;
      double       mSpotSize;
      double       mIncidenceAngle;
      double       mCosIncidenceAngle;
      double       mWavelength;
      unsigned int mWavelength_nm;
      double       mApertureDiameter;
      double       mOffsetLocationECS[3];
   };

protected:
   WsfDirectedEnergyWeapon(const WsfDirectedEnergyWeapon& aSrc);

   virtual bool BeginEngagement(double aSimTime, EngagementData* aEngagementDataPtr);

private:
   Beam* mBeamPtr;
   bool  mIsFiring;
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfDirectedEnergyWeapon, cWSF_COMPONENT_DIRECTED_ENERGY_WEAPON)

#endif
