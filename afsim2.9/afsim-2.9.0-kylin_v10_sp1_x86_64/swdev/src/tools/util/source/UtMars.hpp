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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTMARS_HPP
#define UTMARS_HPP

#include "ut_export.h"

//! A standard set of constants for Mars.
//! See also UtEarth.hpp.
class UT_EXPORT UtMars
{
public:
   static const double cACCEL_OF_GRAVITY; // = 3.71;

   // Semi-major axis (a) in meters
   static const double cA; // = 3393940;

   // Reciprocal of ellipsoid flattening (1/f)
   static const double cRF; // = 197.7820;

   // Semi-minor axis (b) in meters
   static const double cB; // =  3376780;

   // (First eccentricity squared (e ^ 2)
   static const double cE_2; // = 0.0; (no data)

   // Second eccentricity squared (e' ^2)
   static const double cEP_2; // = 0.0; (no data)

   // Rotation rate.
   static const double cOMEGA; // = 7.08821811e-5 rad/sec

   // Gravitation constant
   static const double cMASS; // = 0.64191e+24 kg

   // Gravitational Parameter
   static const double cGRAVITATIONAL_PARAMETER; // == G*cMASS = 4.28283142580671e+13 m^3 / s^2

   //! Air Specific Weight under Standard Sea Level Conditions, kg / m^3:
   //! (This is the one to use for calculation of dynamic pressure (q = 1/2 rho V^2) )
   static const double cSSL_AIR_DENSITY; //  0.020 kg / m^3
};

#endif
