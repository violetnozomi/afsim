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

#ifndef WSFDEFAULTHEL_HPP
#define WSFDEFAULTHEL_HPP

#include "wsf_mil_export.h"

#include <string>

#include "UtIntegrand.hpp"
#include "WsfAtmosphericTurbulence.hpp"
#include "WsfHighEnergyLaser.hpp"
class WsfOpticalPath;

//! Deafult (core) HEL model.  This model is based based the papaer by Gephardt (1976),
//! "High Energy Laser Propagation".  It calculates the spot on target based on the following
//! phenomena:
//! <ul>
//! <li> Diffraction of the beam around the exit aperture.
//! <li> Extinction losses from atmospheric propagation.
//! <li> Beam Spread due to atmospheric turbulence.
//! <li> Spot jitter.
//! </ul>
//! This model does not yet account for thermal blooming but may eventually use the approximations
//! in Gepahrdt to model it.
class WSF_MIL_EXPORT WsfDefaultHEL : public WsfHighEnergyLaser
{
public:
   WsfDefaultHEL(WsfScenario& aScenario);
   ~WsfDefaultHEL() override = default;

   WsfDirectedEnergyWeapon::Beam* Clone() const override { return new WsfDefaultHEL(*this); }

   bool ProcessInput(UtInput& aInput) override;

   void Propagate(double aSimTime) override;

private:
   void PropagateP(double aSimTime);

   int mAtmosphereModel;
   int mHazeModel;
};

#endif
