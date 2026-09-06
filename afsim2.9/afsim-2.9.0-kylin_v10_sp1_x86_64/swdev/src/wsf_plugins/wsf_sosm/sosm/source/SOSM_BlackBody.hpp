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

#ifndef SOSM_BLACKBODY_HPP
#define SOSM_BLACKBODY_HPP

#include <vector>

class SOSM_SpectralObject;
class UtInput;

class SOSM_BlackBody
{
public:
   SOSM_BlackBody();
   // Using default copy constructor
   // SIRM_BlackBody(const SIRM_BlackBody& aSrc);
   virtual ~SOSM_BlackBody();

   virtual bool ProcessInput(UtInput& aInput);

   //! Get the temperature of the body.
   //! @return The temperature of the body in deg-K.
   double GetTemperature() const { return mTemperature; }

   //! Set the temperature of the body.
   //! @param aTemperature The body temperature in deg-K.
   void SetTemperature(double aTemperature) { mTemperature = aTemperature; }

   //! Compute the spectral radiant exitance for a specified wavelength.
   //!
   //! This form uses the current temperature of the body (as defined by SetTemperature).
   //!
   //! @param aWavelength  The wavelength of interest (um)
   //! @returns The spectral radiant exitance (W / (cm^2 * um)).
   double SpectralRadiantExitance(double aWavelength) { return SpectralRadiantExitance(mTemperature, aWavelength); }

   void ComputeRadiantExitance(std::vector<float>& aExitance, const SOSM_SpectralObject& aRequestor)
   {
      ComputeRadiantExitance(mTemperature, aExitance, aRequestor);
   }

   static void ComputeRadiantExitance(double                     aTemperature,
                                      std::vector<float>&        aExitance,
                                      const SOSM_SpectralObject& aRequestor);

   static double SpectralRadiantExitance(double aTemperature, double aWavelenth);

private:
   // Assignment operator declared but not defined to prevent use.
   SOSM_BlackBody operator=(const SOSM_BlackBody& aRhs) = delete;

   double mTemperature;
};

#endif
