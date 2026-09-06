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

#ifndef WSFNORAD_ORBITALPROPAGATOR_HPP
#define WSFNORAD_ORBITALPROPAGATOR_HPP

#include "wsf_space_export.h"

class UtCalendar;
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtOrbitalState.hpp"
#include "WsfNORAD_Util.hpp"
#include "WsfNonClassicalOrbitalPropagator.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
class WsfScenario;

//! WsfNORAD_OrbitalPropagator is a specialization of WsfMover for earth-orbiting satellites.
//!
//! This mover incorporates the SG* and SD* propagators for "near-earth" (orbits > 6.4 revs / day)
//! and "deep-space" (orbits < 6.4 revs / day) satellites.  These are published in the "SpaceTrack 3" report:
//! http://celestrak.com/NORAD/documentation/spacetrk.pdf.  They take into account
//! perturbation effects from the sun and moon.
//! This code is based upon a public domain c-based adaptation of the algorithms.  This adaptation can
//! be found at http://www.projectpluto.com/sat_code.htm .
class WSF_SPACE_EXPORT WsfNORAD_OrbitalPropagator : public WsfNonClassicalOrbitalPropagator
{
public:
   WsfNORAD_OrbitalPropagator();
   WsfNORAD_OrbitalPropagator(const WsfNORAD_OrbitalPropagator& aSrc);
   WsfNORAD_OrbitalPropagator& operator=(const WsfNORAD_OrbitalPropagator&) = delete;
   ~WsfNORAD_OrbitalPropagator() override                                   = default;

   WsfNORAD_OrbitalPropagator* Clone() const override { return new WsfNORAD_OrbitalPropagator(*this); }

   static WsfOrbitalPropagatorTypes::FactoryPtr ObjectFactory(WsfScenario& aScenario);

   //! @name Common framework methods.
   //@{
   bool Initialize(const UtCalendar& aInitialTime) override;
   using WsfNonClassicalOrbitalPropagator::Initialize;
   //@}

   enum EphemerisType
   {
      cSGP  = 0,
      cSGP4 = 1,
      cSGP8 = 2,
      cSDP4 = 3,
      cSDP8 = 4
   }; // Ephemeris type

   //! Get the ephemeris type.  The return value corresponds with the
   //! above enumeration for ephemeris type.
   int GetEphemerisType() const { return mEphem; }

   double GetTimeToNorthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const override;
   double GetTimeToSouthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const override;

   bool SetInitialOrbitalState(const ut::OrbitalState& aState) override;

protected:
   void Propagate(const UtCalendar& aTime) override;

   void UpdateOrbitalState() override;

   bool HyperbolicPropagationAllowed() const override { return false; }

   void PostPropagate() override;

private:
   int SelectEphemeris();

   void SGP_init();
   void SGP(double);

   void SGP4_init();
   void SGP4(double tsince);

   void SGP8_init();
   void SGP8(double tsince);

   void SDP4_init();
   void SDP4(double tsince);

   void SDP8_init();
   void SDP8(double tsince);

   static const int DEEP_ARG_T_PARAMS = 87;
   static const int N_SAT_PARAMS      = (11 + DEEP_ARG_T_PARAMS);

   ut::OrbitalState mPropagatedOrbitalState;

   // data elements
   std::unique_ptr<WsfNORAD_Util::tle_t> tle{nullptr}; // Kept the original implementation's name (Project Pluto).

   bool   mSimpleFlag{false}; // Flag to determine whether to perform simplified calculations (false).
   int    mEphem{1};
   double params[N_SAT_PARAMS];   // for library propagators
   double mPos[3]{0.0, 0.0, 0.0}; // ECI coordinates, km
   double mVel[3]{0.0, 0.0, 0.0}; // ECI coordinates, km/min
};
#endif
