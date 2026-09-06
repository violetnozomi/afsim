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

#ifndef WSFSOSM_INTERACTION_HPP
#define WSFSOSM_INTERACTION_HPP

#include "wsf_sosm_export.h"

#include "SOSM_Interaction.hpp"
class WsfEM_Interaction;

//! The provider of interaction data to the Spectral Optical Detection Model (SOSM).
//!
//! SOSM requires the implementation of this interface which provides information about
//! the sensor and target states (location, orientation, etc.) to the detection model.
class WSF_SOSM_EXPORT WsfSOSM_Interaction : public SOSM_Interaction
{
public:
   WsfSOSM_Interaction(WsfEM_Interaction& aSource);

   float         GetSlantRange() override;     // abstract in base class
   float         GetSensorAltitude() override; // abstract in base class
   virtual float GetSensorSpeed();
   float         GetTargetAltitude() override; // abstract in base class
   float         GetTargetSpeed() override;    // abstract in base class
   float         GetTargetThrottle() override;
   float         GetAbsoluteTargetElevation() override; // abstract in base class
   void          GetSensorToTargetAspect(float& aAzimuth,
                                         float& aElevation) override; // abstract in base class
   void          GetTargetToSensorAspect(float& aAzimuth,
                                         float& aElevation) override; // abstract in base class

private:
   WsfEM_Interaction& mSource;
};

#endif
