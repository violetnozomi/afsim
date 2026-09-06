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

#include "WsfOpticalPath.hpp"

#include <sstream>

#include "TblLookup.hpp"
#include "UtException.hpp"
#include "UtInputBlock.hpp"
#include "UtInputFile.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfMil.hpp"

// =================================================================================================
WsfOpticalPath::WsfOpticalPath()
   : mCoeffsPtr(nullptr)
{
}

// =================================================================================================
//! Constructor for use with laser coefficient tables.
WsfOpticalPath::WsfOpticalPath(const WsfOpticalPathCoefficientTypes& aTypes,
                               unsigned                              aWavelength,
                               unsigned                              aAtmosphereModel,
                               unsigned                              aHazeModel)
{
   mCoeffsPtr = &aTypes.GetCoefficients(aWavelength, aAtmosphereModel, aHazeModel);
}

// =================================================================================================
//! Older, deprecated version of custom constructor.
WsfOpticalPath::WsfOpticalPath(const WsfOpticalPathCoefficientTypes& aTypes,
                               double                                aWavelength,
                               unsigned                              aAtmosphereModel,
                               unsigned                              aHazeModel)
{
   unsigned wavelength_nm = static_cast<unsigned>(aWavelength * 1.0e9);
   mCoeffsPtr             = &aTypes.GetCoefficients(wavelength_nm, aAtmosphereModel, aHazeModel);
}

// =================================================================================================

WsfOpticalPath::WsfOpticalPath(const WsfOpticalPath::Coefficients* aCoefficientsPtr) // For use with legacy code.
   : mCoeffsPtr(aCoefficientsPtr)
{
}

// =================================================================================================
//! Initialize an already instantiated object.  This method is for monochromatic (laser) use only
//! (See WsfOpticalAttenuation::Initialize).
//! @param aTypes The optical path coefficient types
//! @param aWavelength_nm The monochromatic wavelength in nanometers.
//! @param aAtmosphereModel The atmosphere model (default of 2).
//! @param aHazeModel The haze model to use (default of 1).
bool WsfOpticalPath::Initialize(WsfOpticalPathCoefficientTypes& aTypes,
                                unsigned                        aWavelength_nm,
                                unsigned                        aAtmosphereModel,
                                unsigned                        aHazeModel)
{
   bool ok = true;
   try
   {
      mCoeffsPtr = &aTypes.GetCoefficients(aWavelength_nm, aAtmosphereModel, aHazeModel);
   }
   catch (std::exception&)
   {
      mCoeffsPtr = nullptr;
      ok         = false;
   }
   return ok;
}

// =================================================================================================
//! Compute the total atmospheric transmission along a path.
//! @param aSlantRange The slant range along which the propagation is calculated.
//! @param aTargetHeight The height of the endpoint of propagation.
//! @param aPlatformHeight The height of the start point of propagation.
//! @return The transmission along the path,
double WsfOpticalPath::GetAttenuation(double aSlantRange, double aTargetHeight, double aPlatformHeight)
{
   WsfOpticalPath::Integrand integrand(aSlantRange, aTargetHeight, aPlatformHeight, *mCoeffsPtr, WsfOpticalPath::cBOTH);
   return exp(-integrand.Integrate(0.0, aSlantRange, 5, 1.0e-6));
}

// =================================================================================================
// Nested class WsfOpticalPath::Coefficients
// =================================================================================================

// =================================================================================================
WsfOpticalPath::LaserCoefficients::LaserCoefficients()
   : mWavelength(0.0)
   , mAtmosphereModel(0)
   , mHazeModel(0)
   , mAltitude()
   , mAbs()
   , mScat()
{
}

//! Return the atmospheric ExtinctionCoefficient at a given altitude.
//! @param aAltitude The altitude at which the extinction is desired.
//! @param aType The type of extinction desired.
//! @note This method performs a linear extrapolation from the table's altitude to the desired
//!    altitude.  If the desired altitude is outside the limits of the table, the algorithm will
//!    extrapolate to the desired altitude.
double WsfOpticalPath::LaserCoefficients::GetExtinction(double aAltitude, WsfOpticalPath::Type aType) const
{
   double extinction = 0.0;

   // Linear interpolation on attenuation and scattering.
   int numRows = static_cast<int>(mAltitude.size());
   assert(numRows > 0);
   int row = 1;
   while (row < (numRows - 1))
   {
      bool found = (aAltitude < mAltitude[row]);
      if (found)
      {
         break;
      }
      ++row;
   }

   // Extrapolate even if our data was not found within the bounds of the table.
   double altInterval = (mAltitude[row] - mAltitude[row - 1]);
   double attenSlope  = (mAbs[row] - mAbs[row - 1]) / altInterval;
   double attenuation = attenSlope * (aAltitude - mAltitude[row]) + mAbs[row];
   double scatSlope   = (mScat[row] - mScat[row - 1]) / altInterval;
   double scattering  = scatSlope * (aAltitude - mAltitude[row]) + mScat[row];

   switch (aType)
   {
   case WsfOpticalPath::cABS:
      extinction = attenuation;
      break;

   case WsfOpticalPath::cSCAT:
      extinction = scattering;
      break;

   case WsfOpticalPath::cBOTH:
      extinction = scattering + attenuation;
      break;
   }

   return extinction;
}

// =================================================================================================
// Nested class WsfOpticalPath::Integrand
// =================================================================================================

// =================================================================================================
//! Integrate The extinction coefficient (in 1/m) over the given slant range,
//! from the given platform height to the target height.
//! @param aRange An intermediate range between 0 and the total slant range specified in the constructor.
double WsfOpticalPath::Integrand::Function(double aRange)
{
   double height = Height(aRange, mSlantRange, mTargetHeight, mPlatformHeight);
   return mCoeffs.GetExtinction(height, mType);
}

// =================================================================================================
//! Compute the end height to which we compute the optical path.
double WsfOpticalPath::Integrand::Height(double aRangeFromPlatform,
                                         double aTotalSlantRange,
                                         double aTargetHeight,
                                         double aPlatformHeight)
{
   // assume spherical earth, compute positions relative to earth center
   double TargetRadius   = UtSphericalEarth::cEARTH_RADIUS + aTargetHeight;
   double PlatformRadius = UtSphericalEarth::cEARTH_RADIUS + aPlatformHeight;

   // use law of cosines to compute angle between platform position vector and
   // platform-to-target vector
   double CosBeta = (pow(aTotalSlantRange, 2) + pow(PlatformRadius, 2) - pow(TargetRadius, 2)) /
                    (2 * aTotalSlantRange * PlatformRadius);

   // use law of cosines to compute height above earth at given range from platform
   return (sqrt(pow(aRangeFromPlatform, 2) + pow(PlatformRadius, 2) - 2 * aRangeFromPlatform * PlatformRadius * CosBeta) -
           UtSphericalEarth::cEARTH_RADIUS);
}

// =================================================================================================
// Type List
// =================================================================================================

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfOpticalPathCoefficientTypes& WsfOpticalPathCoefficientTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetOpticalPathCoefficientTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfOpticalPathCoefficientTypes& WsfOpticalPathCoefficientTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetOpticalPathCoefficientTypes();
}

// =================================================================================================
//! Convert parameters to a string in order to find the coefficient table.
const WsfOpticalPathCoefficientTypes::Coefficients&
WsfOpticalPathCoefficientTypes::GetCoefficients(unsigned aWavelength_nm, unsigned aAtmosphereModel, unsigned aHazeModel) const
{
   std::ostringstream oss;
   oss << aWavelength_nm << "_" << aAtmosphereModel << "_" << aHazeModel;
   auto* coeffsPtr = mCoeffs.Find(oss.str());
   if (coeffsPtr == nullptr)
   {
      std::string description = "WsfOpticalPath::Coefficients: Nonexistent instance: " + oss.str();
      auto        out         = ut::log::error() << "WsfOpticalPath::Coefficients: Nonexistent instance.";
      out.AddNote() << "Wavelength: " << aWavelength_nm;
      out.AddNote() << "Atmosphere Model: " << aAtmosphereModel;
      out.AddNote() << "Haze Model: " << aHazeModel;
      throw(UtException(description));
   }
   return *coeffsPtr;
}

// =================================================================================================
//! Convert parameters to a string in order to find the coefficient table (deprecated version; see above).
const WsfOpticalPathCoefficientTypes::Coefficients&
WsfOpticalPathCoefficientTypes::GetCoefficients(double aWavelength, unsigned aAtmosphereModel, unsigned aHazeModel) const
{
   return GetCoefficients((unsigned)(aWavelength * 1.0e9), aAtmosphereModel, aHazeModel);
}

// =================================================================================================
//! This method will process input from a standard UtInput stream,
//! placing filled tables in the mCoeffs map.
//! @param aInput aStandard input stream.
//! @return Whether the current input was processed.
bool WsfOpticalPathCoefficientTypes::LoadType(UtInput& aInput)
{
   std::string command;
   aInput.GetCommand(command);
   bool myCommand = false;

   if (command == "atmospheric_coefficients")
   {
      myCommand = true;
      WsfOpticalPath::LaserCoefficients type;
      UtInputBlock                      outerBlock(aInput, "end_atmospheric_coefficients");

      while (outerBlock.ReadCommand(command))
      {
         if (command == "altitude")
         {
            UtInputBlock block(aInput, "end_altitude");
            while (block.ReadCommand(command))
            {
               block.GetInput().PushBack(command);
               double alt;
               block.GetInput().ReadValue(alt);
               type.mAltitude.push_back(alt);
            }
         }
         else if (command == "wavelength")
         {
            aInput.ReadValueOfType(type.mWavelength, UtInput::cLENGTH);
         }
         else if ((command == "haze") || (command == "haze_model"))
         {
            aInput.ReadValue(type.mHazeModel);
            aInput.ValueInClosedRange(type.mHazeModel, 1, 6);
         }
         else if (command == "atmosphere_model")
         {
            aInput.ReadValue(type.mAtmosphereModel);
            aInput.ValueInClosedRange(type.mAtmosphereModel, 1, 6);
         }
         else if (command == "attenuation")
         {
            UtInputBlock block(aInput, "end_attenuation");
            while (block.ReadCommand(command))
            {
               block.GetInput().PushBack(command);
               double attenuation;
               block.GetInput().ReadValue(attenuation);
               type.mAbs.push_back(attenuation);
            }
            if (type.mAbs.size() != type.mAltitude.size())
            {
               throw(UtInput::EndOfData(block.GetInput()));
            }
         }
         else if (command == "scattering")
         {
            UtInputBlock block(aInput, "end_scattering");
            while (block.ReadCommand(command))
            {
               block.GetInput().PushBack(command);
               double scattering;
               block.GetInput().ReadValue(scattering);
               type.mScat.push_back(scattering);
            }
            if (type.mScat.size() != type.mAltitude.size())
            {
               throw(UtInput::EndOfData(block.GetInput()));
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }

      if (type.mWavelength == 0.0)
      {
         throw UtException("You must specify a wavelength value for a coefficients block");
      }
      std::ostringstream oss;
      int                wavelength_nm = static_cast<int>(type.mWavelength * 1.0e+9);
      oss << wavelength_nm << "_" << type.mAtmosphereModel << "_" << type.mHazeModel;
      mCoeffs.Add(type, oss.str());
   }
   return myCommand;
}
