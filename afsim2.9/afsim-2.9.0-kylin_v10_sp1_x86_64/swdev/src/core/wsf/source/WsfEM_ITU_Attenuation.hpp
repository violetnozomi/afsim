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

#ifndef WSFEM_ITU_ATTENUATION_HPP
#define WSFEM_ITU_ATTENUATION_HPP

#include "wsf_export.h"

#include <functional>
#include <vector>

#include "UtAtmosphere.hpp"
#include "WsfEM_Attenuation.hpp"
#include "WsfEM_Types.hpp"

//! An atmospheric attenuation model based on standards from the
//! International Telecommunications Union (ITU) Radio Communication Sector.
//!
//! The following ITU recommendations are used in this model.
//!
//! 1) "Recommendation ITU-R P.676-9, Attenuation by atmospheric gases".
//! 2) "Recommendation ITU-R P.835-4, Reference standard atmospheres".
//! 3) "Recommendation ITU-R P.838-3, Specific attenuation model for rain for use in prediction methods".
//! 4) "Recommendation ITU-R P.840-4, Attenuation due to clouds and fog".
class WSF_EXPORT WsfEM_ITU_Attenuation : public WsfEM_Attenuation
{
public:
   WsfEM_ITU_Attenuation(const UtAtmosphere& aAtm);
   WsfEM_ITU_Attenuation(const WsfEM_ITU_Attenuation& aSrc);
   WsfEM_ITU_Attenuation& operator=(const WsfEM_ITU_Attenuation& aRhs) = delete;
   WsfEM_Attenuation*     Clone() const override;

   static std::function<WsfEM_Attenuation*(const std::string&)> GetObjectFactory(WsfScenario& aScenario);

   bool ProcessInput(UtInput& aInput) override;

   bool AcceptsInlineBlockInput() const override { return true; }

   // Override of base class method.
   double ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                   WsfEnvironment&             aEnvironment,
                                   WsfEM_Interaction::Geometry aGeometry) override;

   double ComputeAttenuationFactor(double                    aRange,
                                   double                    aElevation,
                                   double                    aAltitude,
                                   double                    aFrequency,
                                   WsfEM_Types::Polarization aPolarization,
                                   double                    aEarthRadiusMultiplier,
                                   WsfEnvironment&           aEnvironment);

   static double
   ComputeGasSpecificAttenuation(double aFrequency, double aPressure, double aTemperature, double aWaterVaporDensity);

   static double ComputeCloudSpecificAttenuation(double aFrequency, double aTemperature, double aWaterDensity);

   static double ComputeRainSpecificAttenuation(double aFrequency, WsfEM_Types::Polarization aPolarization, double aRainRate);

   static void PlotGasFigure1();
   static void PlotGasFigure2();
   static void PlotGasFigure3();

   static void PlotCloudFigure1();

private:
   static void ComputeAtmosphereData(UtAtmosphere& aAtmosphere,
                                     double        aAltitude,
                                     double&       aPressure,
                                     double&       aTemperature,
                                     double&       aWaterVaporDensity);

   void GenerateTable(double aFrequency, WsfEM_Types::Polarization aPolarization, WsfEnvironment& aEnvironment);

   //! The atmosphere for computing pressure, temperature and water vapor density.
   UtAtmosphere mAtmosphere;

   //! The frequency at which the tables were last calculated
   double mFrequency;

   //! Specific attenuation as a function of altitude.
   struct Point
   {
      double mAltitude; //!< meters
      double mGamma;    //!< dB/km
   };
   std::vector<Point> mGammaTable;
};

#endif
