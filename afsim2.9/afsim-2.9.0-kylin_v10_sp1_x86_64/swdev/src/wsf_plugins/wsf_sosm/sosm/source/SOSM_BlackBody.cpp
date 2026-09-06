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

// =================================================================================================
// References:
//
// 1) "Infrared System Engineering"; Richard D. Hudson, Jr
//        2006, John Wiley and Sons, Inc.
// =================================================================================================

#include "SOSM_BlackBody.hpp"

#include <cmath>

#include "SOSM_SpectralObject.hpp"
#include "UtInput.hpp"

namespace
{
#ifdef NOT_NEEDED
//! c - speed of light
const double cLIGHT_SPEED = 2.99792458E+10; // cm / s (NIST-CODATA)
//                        = 2.997925E+10;               // cm/s (Ref 1)

//! h - Planck's constant
const double cPLANCK_CONSTANT = 6.62606896E-34; // J * s = W * s^2 (NIST-CODATA)
//                            = 6.62606896E-34;         // J * s (NIST-CODATA)
//                            = 6.6256E-34;             // W * s^2 (Ref 1)

//! k - Boltzmann's constant
const double cBOLTZMANN_CONSTANT = 1.3806504E-23; // J / degK = W * sec * degK (NIST-CODATA)
//                               = 1.3806504E-23;       // J / degK (NIST-CODATA)
//                               = 1.38054E-23;         // W * sec * degK
#endif

//! c1 - First radiation constant = 2 * pi * h * c^2
const double cC1 = 3.74177118E+4; // (W / cm^2) * um^4 (NIST-CODATA)
//               = 3.74177118E-16;                      // W * m^2 (NIST-CODATA)
//               = 3.7413E+4;                           // (W / cm^2) * um^4 (Ref 1)

//! c2 - Second radiation constant = c * h / k
const double cC2 = 1.4387752E+4; // um * deg-K (NIST-CODATA)
//               = 1.4387752E-2;                        // m * deg-K (NIST-CODATA)
//               = 14388.0;                             // um * deg-K (Ref 1)
} // namespace

// =================================================================================================
SOSM_BlackBody::SOSM_BlackBody()
   : mTemperature(290.0)
{
}

// =================================================================================================
// virtual
SOSM_BlackBody::~SOSM_BlackBody() {}

// =================================================================================================
// virtual
bool SOSM_BlackBody::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "temperature")
   {
      aInput.ReadValueOfType(mTemperature, UtInput::cTEMPERATURE);
      aInput.ValueGreater(mTemperature, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Compute the spectral radiant exitance for a specified wavelength.
//!
//! This form uses a specified temperature rather than the internally maintained temperature.
//!
//! @param aTemperature The body temperature (deg-K)
//! @param aWavelength  The wavelength of interest (um)
//! @returns The spectral radiant exitance (W / (cm^2 * um)).
// static
double SOSM_BlackBody::SpectralRadiantExitance(double aTemperature, double aWavelength)
{
   // Ref. 1, equation 2.8

   double lambda   = aWavelength;
   double lambda_2 = lambda * lambda;
   double t1       = cC1 / (lambda * lambda_2 * lambda_2);
   double t2       = 1.0 / (exp(cC2 / (lambda * aTemperature)) - 1.0);
   return t1 * t2;
}

// =================================================================================================
//! Compute the spectral radiant exitance
//! @param aTemperature [input]  Body temperature in degrees Kelvin
//! @param aExitance    [output] The exitance (W/cm^2/um)
//! @param aRequestor   [input]  The object defining the spectrum of interest.
// static
void SOSM_BlackBody::ComputeRadiantExitance(double                     aTemperature,
                                            std::vector<float>&        aExitance,
                                            const SOSM_SpectralObject& aRequestor)
{
   for (int i = 0; i < aRequestor.Count(); ++i)
   {
      double wavenumber = aRequestor.Origin() + (i * aRequestor.Increment());
      double wavelength = 1.0E+4 / wavenumber;                                                   // um
      aExitance[i]      = static_cast<float>(SpectralRadiantExitance(aTemperature, wavelength)); // W/cm^2/um
   }
}
