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

#include "UtMars.hpp"

//! Surface gravitational acceleration (m / s^2)
//! NASA, http://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html)
const double UtMars::cACCEL_OF_GRAVITY = 3.71;

//! Semi-major axis (a) in meters
//! JPL, http://mars.jpl.nasa.gov/msp98/INTRODUCTION.PDF
const double UtMars::cA = 3393940;

//! Reciprocal of ellipsoid flattening (1/f)
//! JPL, http://mars.jpl.nasa.gov/msp98/INTRODUCTION.PDF
const double UtMars::cRF = 197.7820;

//! Semi-minor axis (b) in meters
//! @note This value is average over the b and c axes;
//!      it is a triaxial ellipsoid.
//! JPL, http://mars.jpl.nasa.gov/msp98/INTRODUCTION.PDF
const double UtMars::cB = 3376780;

//! (First eccentricity squared (e ^ 2)
const double UtMars::cE_2 = 0.0; //! No data.

//! Second eccentricity squared (e' ^2)
const double UtMars::cEP_2 = 0.0; //! No data.

//! General constants

//! Rotation rate (rad/sec)
//! Astro. Alnamac (posted by JPL, in http://ssd.jpl.nasa.gov/?planet_phys_par
const double UtMars::cOMEGA = 7.08821811e-5;

//! Mass (kg)
//! Astro. Alnamac (posted by JPL, in http://ssd.jpl.nasa.gov/?planet_phys_par
const double UtMars::cMASS = 0.64191e+24;

//! Gravitational Parameter (m^3 / s^2)
//! JPL, http://mars.jpl.nasa.gov/msp98/INTRODUCTION.PDF
const double UtMars::cGRAVITATIONAL_PARAMETER = 4.28283142580671e+13;

//! Air Density under Standard Sea Level Conditions, kg / m^3
//! NASA, http://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html
const double UtMars::cSSL_AIR_DENSITY = 0.020; // kg / m^3
