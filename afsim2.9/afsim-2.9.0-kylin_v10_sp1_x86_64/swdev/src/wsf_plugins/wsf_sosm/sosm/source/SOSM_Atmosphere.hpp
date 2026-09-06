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

#ifndef SOSM_ATMOSPHERE_HPP
#define SOSM_ATMOSPHERE_HPP

#include <vector>

#include "SOSM_CoreBase.hpp"
class SOSM_Interaction;
class SOSM_Selector;

//! An abstract base class for representing atmospherics.
class SOSM_Atmosphere : public SOSM_CoreBase
{
public:
   SOSM_Atmosphere();
   SOSM_Atmosphere(const SOSM_Atmosphere& aSrc);
   // Using default destructor

   //! A virtual copy constructor.
   virtual SOSM_Atmosphere* Clone() const = 0;

   //! Compute the atmospheric data
   //!
   //! The background is considered space or ground. The foreground is considered the target.
   //!
   //! @param aSelector                [input]  The selector which defines the spectral overlap between this and the sensor.
   //! @param aInteraction             [input]  The interaction object representing the sensor and target state.
   //! @param aBackgroundRadiance      [output] The background radiance (W/cm^2/sr/um)
   //! @param aForegroundRadiance      [output] The foreground radiance (W/cm^2/sr/um)
   //! @param aForegroundTransmittance [output] The foreground transmittance (unitless - [0..1])
   virtual void ComputeAtmosphereData(const SOSM_Selector& aSelector,
                                      SOSM_Interaction&    aInteraction,
                                      std::vector<float>&  aBackgroundRadiance,
                                      std::vector<float>&  aForegroundRadiance,
                                      std::vector<float>&  aForegroundTransmittance) = 0;

   //! Get the sample points to be used for fast detection mode table generation.
   //! @param aAltValues   Altitude sample points (meters)
   //! @param aElValues    Elevation sample points (radians)
   //! @param aRangeValues Range sample points (meters)
   virtual void GetSamplePoints(std::vector<float>& aAltValues,
                                std::vector<float>& aElValues,
                                std::vector<float>& aRangeValues) = 0;
};

#endif
