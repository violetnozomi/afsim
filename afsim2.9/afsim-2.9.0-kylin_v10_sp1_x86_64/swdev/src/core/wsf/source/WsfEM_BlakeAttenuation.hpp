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

#ifndef WSFEM_BLAKEATTENUATION_HPP
#define WSFEM_BLAKEATTENUATION_HPP

#include "wsf_export.h"

#include "WsfEM_Attenuation.hpp"

//! Class definition for the Atmospheric Attenuation (Absorption) Model of Lamont Blake.
//!
//! Translated to C++ from FORTRAN.
//!
//! Computes a percentage of signal NOT lost due to absorption as a function
//! of signal frequency, elevation between origin and destination, and range
//! between origin and destination.

class WSF_EXPORT WsfEM_BlakeAttenuation : public WsfEM_Attenuation
{
public:
   WsfEM_BlakeAttenuation();
   WsfEM_BlakeAttenuation(const WsfEM_BlakeAttenuation& aSrc);
   WsfEM_BlakeAttenuation& operator=(const WsfEM_BlakeAttenuation& aRhs) = delete;
   ~WsfEM_BlakeAttenuation() override;

   static WsfEM_Attenuation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Attenuation* Clone() const override;

protected:
   double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency) override;

private:
   double Calculate2WayAttenuation(double aRange, double aElevation, double aFrequency);

   void SearchForValue(double aVal, double aTable[], int aLength, int& aLowIndex, double& aFrac);

   static double sElevations[6];
   static double sFrequencies[7];
   static double sACoefTable[7][6];
   static double sBCoefTable[7][6];
};

#endif
