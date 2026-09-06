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

#ifndef WSFEM_EARCE_ATTENUATION_HPP
#define WSFEM_EARCE_ATTENUATION_HPP

#include <string>

#include "WsfEM_Attenuation.hpp"

//! The RF atmospheric attenuation model from the ESAMS/ALARM/RADGUNS Common Environment.
//! This is a C++ conversion of the FORTRAN model as found in SUPPRESSOR.
//! @note This algorithm is only valid for ground-based radars.
class WsfEM_EARCE_Attenuation : public WsfEM_Attenuation
{
public:
   WsfEM_EARCE_Attenuation() = default;
   WsfEM_EARCE_Attenuation(const WsfEM_EARCE_Attenuation& aSrc);

   static WsfEM_Attenuation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Attenuation* Clone() const override;

protected:
   double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency) override;

private:
   static double ATMABS(double aALT, double aFRQGHZ);

   static double ATNEAR(double aRNGIN, double aANGIN, double aFREQIN, double aEFFRAD);

   static double EARCAL(double aDIST3D, double aELEV, double aFREQ, double aEFRAD);

   static int EARPOS(int aCOUNT, const double aARRAY[], double aVALUE);
};

#endif
