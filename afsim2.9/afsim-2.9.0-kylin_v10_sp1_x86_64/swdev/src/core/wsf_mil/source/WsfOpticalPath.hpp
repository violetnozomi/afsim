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

#ifndef WSFOPTICALPATH_HPP
#define WSFOPTICALPATH_HPP

#include "wsf_mil_export.h"

#include <string>
#include <vector>

class Integrand;
#include "UtGenericMappedList.hpp"
#include "UtIntegrand.hpp"
class UtInput;
class WsfEM_Xmtr;
class WsfOpticalPathCoefficientTypes;
class WsfScenario;

//! Evaluates a line-of-sight path through the atmosphere.
//! Calculates atmospheric transmission and extinction at a given
//! wavelength.  Coefficient tables must be provided for atmospheric
//! extinction (absorption and scattering).
//! @note This extinction calculation should be used for optical
//! wavelengths (IR-UV).  Generating new tables is done using MODTRAN.
class WSF_MIL_EXPORT WsfOpticalPath
{
public:
   friend class WsfOpticalPathCoefficientTypes;
   WsfOpticalPath(); // Use for deferred initialization of coefficients
   WsfOpticalPath(const WsfOpticalPathCoefficientTypes& aTypes,
                  unsigned                              aWavelength,
                  unsigned                              aAtmosphereModel = 2,
                  unsigned                              aHazeMode        = 1);

   class Coefficients;

   WsfOpticalPath(const WsfOpticalPath::Coefficients* aCoefficientsPtr); // For use with legacy code.

   virtual ~WsfOpticalPath() = default;

   enum Type
   {
      cABS,  //! absorption only
      cSCAT, //! scattering only
      cBOTH  //! both
   };

   bool Initialize(WsfOpticalPathCoefficientTypes& aTypes,
                   unsigned                        aWavelength_nm,
                   unsigned                        aAtmosphereModel = 2,
                   unsigned                        aHazeModel       = 1);

   double GetAttenuation(double aSlantRange, double aTargetHeight, double aShootHeight);

   //! Use atmospheric coefficients to fill atmosphere profiles.
   //!@note
   class WSF_MIL_EXPORT Coefficients
   {
   public:
      Coefficients()          = default;
      virtual ~Coefficients() = default;

      // virtual Coefficients* Clone() const = 0;

      virtual double GetExtinction(double aAltitude, Type aType = cBOTH) const = 0;
   };


   //! Current data are valid from 0 to 6 km altitude (Returns zero for altitudes
   //! outside the available data). Supported wavelengths (laser) are 1.064, 1.315, 1.620
   //! and 1.000 microns; COIL analysis for 1.31521 microns wavelength.
   class WSF_MIL_EXPORT LaserCoefficients : public Coefficients
   {
   public:
      friend class WsfOpticalPathCoefficientTypes;
      LaserCoefficients();
      ~LaserCoefficients() override = default;

      const Coefficients& GetCoefficients(const std::string& aWavelength_nm,
                                          unsigned           aAtmosphereModel = 2,
                                          unsigned           aHazeModel       = 1);

      double GetExtinction(double aAltitude, Type aType) const override;

      //! The following are necessary to copy into simple arrays with DynamicSpear.
      const std::vector<double>& GetAltitudes() const { return mAltitude; }
      const std::vector<double>& GetAttenuationCoefficients() const { return mAbs; }
      const std::vector<double>& GetScatteringCoefficients() const { return mScat; }

   protected:
   private:
      friend class WsfOpticalAttenuation; //! Needed for conversion.

      double mWavelength; //! Typical supported wavelengths are the following: 1000, 1060, 1064, 1080, 1315, 1620, 3800
      int    mAtmosphereModel;       //! The following atmosphere models are supported from the ModTran,
                                     //! Models 2,3, and 6 are supported for WSF at wavelength 1064;
                                     //! otherwise only model 2 is supported at other wavelengths.
                                     //!  1 - Tropical Atmosphere
                                     //!  2 - Midlatitude Summer (default)
                                     //!  3 - Midlatitude Winter
                                     //!  4 - Subarctic Summer
                                     //!  5 - Subarctic Winter
                                     //!  6 - 1976 U.S. Standard
      int mHazeModel;                //! The following haze values are supported;
                                     //!  Currently all models are only available for wavelength 1064;
                                     //!  otherwise only model 1 is available (see model extraction procedure, below.
                                     //!  to access other models at other wavelengths).
                                     //!  1   RURAL Extinction, VIS = 23 km (Clear) (default)
                                     //!  2   RURAL Extinction, VIS = 5 km (Hazy)
                                     //!  3   Navy maritime Extinction
                                     //!  4   MARITIME Extinction, VIS = 23 km
                                     //!  5   Urban Extinction, VIS = 5 km
      std::vector<double> mAltitude; //! Height array for which there are profile data.
      std::vector<double> mAbs;      //! Absorption, or attenuation, coefficient in 1/m.
      std::vector<double> mScat;     //! Scattering coefficient in 1/m.
   };

   //! Encapsulate all data needed for an integration of extinction along
   //! a path in the atmosphere.
   class WSF_MIL_EXPORT Integrand : public UtIntegrand
   {
   public:
      Integrand(double aSlantRange, double aTargetHeight, double aPlatformHeight, const Coefficients& aCoeffs, Type aType)
         : mSlantRange(aSlantRange)
         , mTargetHeight(aTargetHeight)
         , mPlatformHeight(aPlatformHeight)
         , mCoeffs(aCoeffs)
         , mType(aType)
      {
      }

      double Function(double aRange) override;

   private:
      double Height(double aRangeFromPlatform, double aTotalSlantRange, double aTargetHeight, double aPlatformHeight);

      double              mSlantRange;
      double              mTargetHeight;
      double              mPlatformHeight;
      const Coefficients& mCoeffs;
      Type                mType;
   };

   WsfOpticalPath(const WsfOpticalPathCoefficientTypes& aTypes,
                  double                                aWavelength,
                  unsigned                              aAtmosphereModel = 2,
                  unsigned                              aHazeMode        = 1);

private:
   const Coefficients* mCoeffsPtr;
};

class WsfOpticalPathCoefficientTypes
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_MIL_EXPORT WsfOpticalPathCoefficientTypes&       Get(WsfScenario& aScenario);
   static WSF_MIL_EXPORT const WsfOpticalPathCoefficientTypes& Get(const WsfScenario& aScenario);
   //@}

   using Coefficients = WsfOpticalPath::LaserCoefficients;

   WSF_MIL_EXPORT bool LoadType(UtInput& aInput);

   WSF_MIL_EXPORT const Coefficients& GetCoefficients(unsigned aWavelength_nm,
                                                      unsigned aAtmosphereModel = 2,
                                                      unsigned aHazeModel       = 1) const;

   WSF_MIL_EXPORT const Coefficients& GetCoefficients(double   aWavelength,
                                                      unsigned aAtmosphereModel = 2,
                                                      unsigned aHazeModel       = 1) const;

private:
   UtStdMappedList<Coefficients, std::string> mCoeffs;
};
#endif
