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

#include "WsfGENAP_AntennaPattern.hpp"

#include <cstdlib>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"

WsfGENAP_AntennaPattern::WsfGENAP_AntennaPattern()
   : WsfALARM_AntennaPattern(new GENAP_Data())
{
}

//! Factory method for WsfAntennaPatternTypes.
//! This is called by WsfAntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfGENAP_AntennaPattern::ObjectFactory(const std::string& aTypeName)
{
   WsfAntennaPattern* patternPtr = nullptr;
   if (aTypeName == "genap_pattern")
   {
      patternPtr = new WsfGENAP_AntennaPattern();
   }
   return patternPtr;
}

// virtual
WsfAntennaPattern* WsfGENAP_AntennaPattern::Clone() const
{
   return new WsfGENAP_AntennaPattern(*this);
}

// virtual
bool WsfGENAP_AntennaPattern::ProcessInput(UtInput& aInput)
{
   // NOTE: WsfALARM_AntennaPattern::ProcessInput is NOT called here
   //       because it also accepts the file input. We don't allow that, but instead
   //       process all our arguments except for the base class values in this routine.
   return WsfAntennaPattern::ProcessInput(aInput);
}

// =================================================================================================
// Nested class 'GENAP_Data'.
// =================================================================================================
WsfGENAP_AntennaPattern::GENAP_Data::GENAP_Data()
   : ALARM_Data()
   , mAzDistribution(cDF_UNKNOWN)
   , mElDistribution(cDF_UNKNOWN)
   , mBlockageShape(cBS_NONE)
   , mAzExponent(0.0)
   , mAzSidelobeLevelInput(0.0)
   , mAzSidelobeLevel(0.0)
   , mAzSamples(0)
   , mAzBlockage(0.1)
   , mAzSidelobePosition(0.0)
   , mAzNullPosition(0.0)
   , mElRotation(0.0)
   , mElExponent(0.0)
   , mElSidelobeLevelInput(0.0)
   , mElSidelobeLevel(0.0)
   , mElCosecantLimit(0.0)
   , mElSamples(0)
   , mElBlockage(0.1)
   , mElEfficiency(0.0)
   , mElSidelobePosition(0.0)
   , mElNullPosition(0.0)
   , mApertureWidth(1.0)
   , mApertureHeight(1.0)
{
   // Initialize base class values to our defaults.
   mApertureShape = cAS_UNKNOWN;
   mPolarizations.push_back(WsfEM_Types::cPOL_DEFAULT);
   mPatternMap[WsfEM_Types::cPOL_DEFAULT].mAzBeamwidth = 0.0;
   mPatternMap[WsfEM_Types::cPOL_DEFAULT].mElBeamwidth = 0.0;

   // initialize azimuth weight function arrays
   for (int i = 0; i < cNAZMAX; ++i)
   {
      mAzWeights[i]         = 0.0;
      mAzBlockageWeights[i] = 0.0;
   }

   // initialize elevation weight function arrays
   for (int i = 0; i < cNELMAX; ++i)
   {
      mElWeights[i]         = 0.0;
      mElBlockageWeights[i] = 0.0;
   }
}

bool WsfGENAP_AntennaPattern::GENAP_Data::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   bool ok = true;
   if (mPatternMap[WsfEM_Types::cPOL_DEFAULT].mPeakGain <= 0.0)
   {
      ut::log::error() << "Unspecified 'peak_gain'.";
      ok = false;
   }

   if (mApertureShape == cAS_UNKNOWN)
   {
      ut::log::error() << "Unspecified 'aperture_shape'.";
      ok = false;
   }

   if (mAzDistribution == cDF_UNKNOWN)
   {
      ut::log::error() << "Unspecified 'azimuth_distribution'.";
      ok = false;
   }
   else if ((mAzDistribution == cDF_COSINE) && (mAzExponent <= 0.0))
   {
      ut::log::error() << "'azimuth_exponent' must be specified if 'azimuth_distribution' is 'cosine'.";
      ok = false;
   }
   else if ((mAzDistribution == cDF_BW_SLL) && (mAzSidelobeLevel <= 0.0))
   {
      ut::log::error() << "'azimuth_side_lobe_level' must be specified if 'azimuth_distribution' is 'bw_sll'.";
      ok = false;
   }

   if (mElDistribution == cDF_UNKNOWN)
   {
      ut::log::error() << "Unspecified 'elevation_distribution'.";
      ok = false;
   }
   else if ((mElDistribution == cDF_COSINE) && (mElExponent <= 0.0))
   {
      ut::log::error() << "'elevation_exponent' must be specified if 'elevation_distribution' is 'cosine'.";
      ok = false;
   }
   else if ((mElDistribution == cDF_BW_SLL) && (mElSidelobeLevel <= 0.0))
   {
      ut::log::error() << "'elevation_side_lobe_level' must be specified if 'elevation_distribution' is 'bw_sll'.";
      ok = false;
   }
   else if ((mElDistribution == cDF_COSECANT) && (mElCosecantLimit <= 0.0))
   {
      ut::log::error() << "'elevation_cosecant_limit' must be specified if 'elevation_distribution' is 'cosecant'.";
      ok = false;
   }

   if (!ok)
   {
      return false;
   }

   // Invoke the pattern generator.
   GeneratePattern();

   // Initialize the base class AFTER we've generated the pattern as it uses data we've generated.
   if (!WsfALARM_AntennaPattern::ALARM_Data::InitializeBase())
   {
      return false;
   }

   // Skip the ALARM_Data::Initialize as it is not required to be called.
   return BaseData::Initialize(aAntennaPattern);
}

// virtual
bool WsfGENAP_AntennaPattern::GENAP_Data::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool         myCommand = true;
   std::string  command(aInput.GetCommand());
   PatternData& patternData = mPatternMap[WsfEM_Types::cPOL_DEFAULT];
   if (command == "peak_gain")
   {
      aInput.ReadValueOfType(patternData.mPeakGain, UtInput::cRATIO);
      aInput.ValueGreater(patternData.mPeakGain, 0.0);
   }
   else if (command == "aperture_shape")
   {
      std::string shape;
      aInput.ReadValue(shape);
      if (shape == "rectangular")
      {
         mApertureShape = cAS_RECTANGULAR;
      }
      else if (shape == "elliptical")
      {
         mApertureShape = cAS_ELLIPTICAL;
      }
      else if (shape == "circular")
      {
         mApertureShape = cAS_CIRCULAR;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "azimuth_distribution")
   {
      std::string dist;
      aInput.ReadValue(dist);
      if (dist == "uniform")
      {
         mAzDistribution = cDF_UNIFORM;
      }
      else if (dist == "cosine")
      {
         mAzDistribution = cDF_COSINE;
      }
      else if (dist == "bw/sll")
      {
         mAzDistribution = cDF_BW_SLL;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "azimuth_beamwidth")
   {
      aInput.ReadValueOfType(patternData.mAzBeamwidth, UtInput::cANGLE);
      aInput.ValueGreater(patternData.mAzBeamwidth, 0.0);
      patternData.mInputAzBeamwidth = patternData.mAzBeamwidth;
   }
   else if (command == "azimuth_exponent")
   {
      aInput.ReadValue(mAzExponent);
      aInput.ValueInClosedRange(mAzExponent, 1.0, 4.0);
   }
   else if (command == "azimuth_side_lobe_level")
   {
      aInput.ReadValueOfType(mAzSidelobeLevel, UtInput::cRATIO);
      aInput.ValueInClosedRange(mAzSidelobeLevel, UtMath::DB_ToLinear(15.0), UtMath::DB_ToLinear(55.0));
      mAzSidelobeLevelInput = mAzSidelobeLevel;
   }
   else if (command == "elevation_distribution")
   {
      std::string dist;
      aInput.ReadValue(dist);
      if (dist == "uniform")
      {
         mElDistribution = cDF_UNIFORM;
      }
      else if (dist == "cosine")
      {
         mElDistribution = cDF_COSINE;
      }
      else if (dist == "bw/sll")
      {
         mElDistribution = cDF_BW_SLL;
      }
      else if (dist == "cosecant")
      {
         mElDistribution = cDF_COSECANT;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "elevation_beamwidth")
   {
      aInput.ReadValueOfType(patternData.mElBeamwidth, UtInput::cANGLE);
      aInput.ValueGreater(patternData.mElBeamwidth, 0.0);
      patternData.mInputElBeamwidth = patternData.mElBeamwidth;
   }
   else if (command == "elevation_exponent")
   {
      aInput.ReadValue(mElExponent);
      aInput.ValueInClosedRange(mElExponent, 1.0, 4.0);
   }
   else if (command == "elevation_side_lobe_level")
   {
      aInput.ReadValueOfType(mElSidelobeLevel, UtInput::cRATIO);
      aInput.ValueInClosedRange(mElSidelobeLevel, UtMath::DB_ToLinear(15.0), UtMath::DB_ToLinear(55.0));
      mElSidelobeLevelInput = mElSidelobeLevel;
   }
   else if (command == "elevation_cosecant_limit")
   {
      aInput.ReadValueOfType(mElCosecantLimit, UtInput::cANGLE);
      aInput.ValueGreater(mElCosecantLimit, 0.0);
   }
   else
   {
      // NOTE: WsfALARM_AntennaPattern::ShareALARM_Data::ProcessInput is NOT called here
      //       because it also accepts the file input. We don't allow that, but instead
      //       process all our arguments except for the base class values in this routine.
      myCommand = WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput);
   }
   return myCommand;
}


//! generate the antenna pattern
void WsfGENAP_AntennaPattern::GENAP_Data::GeneratePattern()
{
   PatternData& patternData = mPatternMap[WsfEM_Types::cPOL_DEFAULT];

   // define frequency as defined in genap.f
   static const double cFREQ = 3.0e9;

   // compute wavelength in meters
   double wavelength = UtMath::cLIGHT_SPEED / cFREQ;

   // back compute aperture width to work with Suppressor with does
   // not use aperture dimensions as inputs.  The kfac is part of the
   // formula bw = k * lambda / d, where d is aperture width
   if ((mAzDistribution == cDF_UNIFORM) || (mAzDistribution == cDF_COSINE) || (mAzDistribution == cDF_COSECANT))
   {
      mApertureWidth =
         KFactor(mAzDistribution, mAzExponent) * wavelength / (patternData.mAzBeamwidth * UtMath::cDEG_PER_RAD);
   }
   if ((mElDistribution == cDF_UNIFORM) || (mElDistribution == cDF_COSINE) || (mElDistribution == cDF_COSECANT))
   {
      mApertureHeight =
         KFactor(mElDistribution, mElExponent) * wavelength / (patternData.mElBeamwidth * UtMath::cDEG_PER_RAD);
   }

   // assign zero values to a non-zero value of -299 dB
   if (mAzSidelobeLevel <= 0.0)
   {
      mAzSidelobeLevel = 1.25e-30;
   }
   if (mElSidelobeLevel <= 0.0)
   {
      mElSidelobeLevel = 1.25e-30;
   }

   // compute aperture weights
   ComputeApertureWeights(wavelength);

   // modify weights due to aperture shape and blockage
   VectorReplace(mAzSamples, mAzWeights, mAzBlockageWeights);
   VectorReplace(mElSamples, mElWeights, mElBlockageWeights);
   if (mElDistribution == cDF_COSECANT)
   {
      VectorReplace(mElSamples, &mElWeights[mElSamples], &mElBlockageWeights[mElSamples]);
   }
   else
   {
      // modify aperture weights to simulate aperture blockage
      ModifyApertureDistribution(mAzWeights,
                                 mAzSamples,
                                 mElWeights,
                                 mElSamples,
                                 mAzBlockage,
                                 mElBlockage,
                                 mApertureShape,
                                 mBlockageShape,
                                 mAzBlockageWeights);
      ModifyApertureDistribution(mElWeights,
                                 mElSamples,
                                 mAzWeights,
                                 mAzSamples,
                                 mElBlockage,
                                 mAzBlockage,
                                 mApertureShape,
                                 mBlockageShape,
                                 mElBlockageWeights);
   }

   // compute directivity and efficiency
   double dir = 12.566 * mApertureWidth * mApertureHeight / wavelength / wavelength;

   // genap.f has apertureShape hard-coded to cAS_RECTANGULAR for Suppressor
   // interface; bypass if checks below
   // if (mApertureShape == cAS_ELLIPTICAL)
   //   dir *= 0.7854;
   if (mElDistribution != cDF_COSECANT)
   {
      mElEfficiency =
         IlluminationEfficiency(mAzWeights, mAzSamples, mElWeights, mElSamples, mAzBlockage, mElBlockage, mApertureShape, mBlockageShape);
   }
   // if (mApertureShape == cAS_CIRCULAR)
   //    mElEfficiency /= 0.7854;
   dir *= mElEfficiency;

   ComputeCrudePattern(wavelength);

   // for Suppressor, compute the positive half of the azimuth
   // patterns since there is symmetry. compute 0-90 at 0.1 deg
   // increments
   double azmin  = 0.0;
   double azincr = 0.1;
   int    nazpts = static_cast<int>((90.0 - azmin) / azincr + 0.5) + 1;

   // for Suppressor, compute the positive half of the elevation
   // patterns since there is symmetry.  compute 0-90 at 0.1 deg
   // increments.  but when the cosecant pattern is selected, -90->90
   // needs to be computed because there is no symmetry
   double elmin = 0.0;
   if (mElDistribution == cDF_COSECANT)
   {
      elmin = -90.0;
   }
   double elincr = 0.1;
   int    nelpts = static_cast<int>((90.0 - elmin) / elincr + 0.5) + 1;

   // calculate final antenna patterns
   //
   // AZIMUTH
   double azPattern[cNAZMAX];
   PatternFourierTransform(mAzBlockageWeights,
                           mAzSamples,
                           mApertureWidth / wavelength,
                           nazpts,
                           azmin * UtMath::cRAD_PER_DEG,
                           azincr * UtMath::cRAD_PER_DEG,
                           false,
                           azPattern);

   // ELEVATION
   double elPattern[cNELMAX];
   if (mElDistribution == cDF_COSECANT)
   {
      PatternComplexFourierTransform(mElBlockageWeights,
                                     mElSamples,
                                     mApertureHeight / wavelength,
                                     nelpts,
                                     elmin * UtMath::cRAD_PER_DEG,
                                     elincr * UtMath::cRAD_PER_DEG,
                                     elPattern);
      CosecantFix(nelpts, elincr, elmin, elPattern);
   }
   else
   {
      PatternFourierTransform(mElBlockageWeights,
                              mElSamples,
                              mApertureHeight / wavelength,
                              nelpts,
                              elmin * UtMath::cRAD_PER_DEG,
                              elincr * UtMath::cRAD_PER_DEG,
                              false,
                              elPattern);
   }

   // amplitude normalize the antenna patterns if necessary
   double gNorm = 0.0;
   PatternFourierTransform(mAzBlockageWeights, mAzSamples, mApertureWidth / wavelength, 1, 0.0, UtMath::cDEG_PER_RAD, false, &gNorm);
   if (gNorm != 0.0)
   {
      double gNormInv = 1.0 / gNorm;
      VectorProduct(-nazpts, &gNormInv, azPattern, azPattern);
      VectorProduct(-nelpts, &gNormInv, elPattern, elPattern);
   }

   // determine the beamwidths from the final pattern and
   // also find the angle of the first null
   double azBeamwidth = patternData.mAzBeamwidth * UtMath::cDEG_PER_RAD;
   CalculateBeamwidth(mAzDistribution,
                      nazpts,
                      azmin,
                      azincr,
                      azPattern,
                      azBeamwidth,
                      mAzSidelobeLevel,
                      mAzSidelobePosition,
                      mAzNullPosition);
   patternData.mAzBeamwidth = azBeamwidth * UtMath::cRAD_PER_DEG;

   double elBeamwidth = patternData.mElBeamwidth * UtMath::cDEG_PER_RAD;
   CalculateBeamwidth(mElDistribution,
                      nelpts,
                      elmin,
                      elincr,
                      elPattern,
                      elBeamwidth,
                      mElSidelobeLevel,
                      mElSidelobePosition,
                      mElNullPosition);
   patternData.mElBeamwidth = elBeamwidth * UtMath::cRAD_PER_DEG;

   // apply cosecanting roll-off
   if (mElDistribution == cDF_COSECANT)
   {
      CosecantRolloff(nelpts,
                      elmin,
                      elincr,
                      elPattern,
                      patternData.mElBeamwidth * UtMath::cDEG_PER_RAD,
                      mElCosecantLimit * UtMath::cDEG_PER_RAD);
   }

   // store data in vectors
   patternData.mAzAngles.resize(nazpts);
   patternData.mAzGains.resize(nazpts);
   for (int i = 0; i < nazpts; ++i)
   {
      patternData.mAzAngles[i] = (azmin + azincr * i) * UtMath::cRAD_PER_DEG;
      patternData.mAzGains[i]  = std::max(azPattern[i], 1.0E-37);
   }

   patternData.mElAngles.resize(nelpts);
   patternData.mElGains.resize(nelpts);
   for (int i = 0; i < nelpts; ++i)
   {
      patternData.mElAngles[i] = (elmin + elincr * i) * UtMath::cRAD_PER_DEG;
      patternData.mElGains[i]  = std::max(elPattern[i], 1.0E-37);
   }
}

//! computes a variety of aperture distribution functions
void WsfGENAP_AntennaPattern::GENAP_Data::ApertureDistribution(Distribution aDistributionFunction,
                                                               double       aExponent,
                                                               double       aBroadside,
                                                               double       aApertureSize,
                                                               double&      aEfficiency,
                                                               double       aSidelobeLevelDB,
                                                               int          aNumSamples,
                                                               double*      aWeightingFunction)
{
   if (aDistributionFunction == cDF_UNIFORM)
   {
      // uniform distribution
      double one = 1.0;
      VectorReplace(-aNumSamples, &one, aWeightingFunction);
   }
   else if (aDistributionFunction == cDF_COSINE)
   {
      // cosine to the nth power distribution
      double dx = 1.0 / aNumSamples;
      double x  = 0.5 * (-1.0 + dx);
      for (int i = 0; i < aNumSamples; ++i)
      {
         aWeightingFunction[i] = pow(cos(0.5 * UtMath::cTWO_PI * x), aExponent);
         x += dx;
      }
   }
   else if (aDistributionFunction == cDF_COSECANT)
   {
      // cosecant beam
      CosecantBeam(aExponent,
                   aBroadside,
                   aApertureSize,
                   aNumSamples,
                   aWeightingFunction,
                   &aWeightingFunction[aNumSamples],
                   aEfficiency);
      double patternNorm = sqrt(Energy(aNumSamples, aWeightingFunction, &aWeightingFunction[aNumSamples]));
      double normInverse = 1.0 / patternNorm;
      VectorProduct(-aNumSamples, &normInverse, aWeightingFunction, aWeightingFunction);

      VectorProduct(-aNumSamples, &normInverse, &aWeightingFunction[aNumSamples], &aWeightingFunction[aNumSamples]);
      return;
   }
   else if (aDistributionFunction == cDF_BW_SLL)
   {
      // specified beamwidth and sidelobe ratio
      TaylorOne(aSidelobeLevelDB, aNumSamples, aWeightingFunction);
   }
   else
   {
      return;
   }

   // normalize weights
   double patternNorm = ZSum(aNumSamples, aWeightingFunction);
   double normInverse = 1.0 / patternNorm;
   VectorProduct(-aNumSamples, &normInverse, aWeightingFunction, aWeightingFunction);
}


//! modify the aperture distribution functions for the shape of the aperture and
//! blockage. result is projected onto the x-axis
void WsfGENAP_AntennaPattern::GENAP_Data::ModifyApertureDistribution(double*       aXAxisDistribution,
                                                                     int           aNumXPoints,
                                                                     const double* aYAxisDistribution,
                                                                     int           aNumYPoints,
                                                                     double        aBlockageX,
                                                                     double        aBlockageY,
                                                                     ApertureShape aApertureShape,
                                                                     BlockageShape aBlockageShape,
                                                                     double*       aModifiedXDistribution)
{
   // NOTE: apertureShape and blockageShape are fixed in genap.f for the Suppressor interface
   // we modify it here for usage in this subroutine as to not effect the global value
   aApertureShape = cAS_RECTANGULAR;
   aBlockageShape = cBS_NONE;

   // begin projection calculations
   double dx = 1.0 / aNumXPoints;
   double dy = 1.0 / aNumYPoints;
   double x0 = 0.5 * (1.0 + aNumXPoints);
   double x  = 0.5 * (-1.0 + dx);

   for (int i = 0; i < aNumXPoints; ++i)
   {
      double xp  = fabs(x) / (0.5 * aBlockageX + 0.001) + 0.001;
      double y   = 0.5 * (-1.0 + dy);
      double sum = 0.0;

      for (int j = 0; j < aNumYPoints; ++j)
      {
         double yp           = fabs(y) / (0.5 * aBlockageY + 0.001) + 0.001;
         double rangeSquared = x * x + y * y;

         if (((aApertureShape == cAS_ELLIPTICAL) && (rangeSquared >= 0.25001)) ||
             ((aBlockageShape == cBS_RECTANGULAR) && (xp <= 1.0) && (yp <= 1.0)) ||
             ((aBlockageShape == cBS_ELLIPTICAL) && ((xp * xp + yp * yp) <= 1.0)) ||
             ((aBlockageShape == cBS_DIAMOND) && (xp <= 1.0) && (yp <= (1.0 - xp))))
         {
            y += dy;
         }
         else
         {
            if (aApertureShape == cAS_RECTANGULAR)
            {
               sum += aYAxisDistribution[j];
            }
            else if (aApertureShape == cAS_ELLIPTICAL)
            {
               sum += InterpolatedValue(aXAxisDistribution, aNumXPoints, x0 + sqrt(rangeSquared) / dx);
            }
            y += dy;
         }
      }

      if (aApertureShape == cAS_RECTANGULAR)
      {
         aModifiedXDistribution[i] = sum * aXAxisDistribution[i];
      }
      else
      {
         aModifiedXDistribution[i] = sum;
      }

      x += dx;
   }
}


//! computes the illumination efficiency for the two-dimensional aperture
double WsfGENAP_AntennaPattern::GENAP_Data::IlluminationEfficiency(double*       aXAxisDistribution,
                                                                   int           aNumXPoints,
                                                                   const double* aYAxisDistribution,
                                                                   int           aNumYPoints,
                                                                   double        aBlockageX,
                                                                   double        aBlockageY,
                                                                   ApertureShape aApertureShape,
                                                                   BlockageShape aBlockageShape)
{
   // NOTE: apertureShape and blockageShape are fixed in genap.f for the Suppressor interface
   // we modify it here for usage in this subroutine as to not effect the global value
   aApertureShape = cAS_RECTANGULAR;
   aBlockageShape = cBS_NONE;

   // begin calculating the illumination efficiency
   double dx = 1.0 / aNumXPoints;
   double dy = 1.0 / aNumYPoints;
   double x0 = 0.5 * (1.0 + aNumXPoints);

   double sum        = 0.0;
   double sumSquared = 0.0;
   double x          = 0.5 * (-1.0 + dx);

   for (int i = 0; i < aNumXPoints; ++i)
   {
      double xp = fabs(x) / (0.5 * aBlockageX + 0.001) + 0.001;
      double y  = 0.5 * (-1.0 + dy);
      for (int j = 0; j < aNumYPoints; ++j)
      {
         double yp           = fabs(y) / (0.5 * aBlockageY + 0.001) + 0.001;
         double rangeSquared = x * x + y * y;

         if (((aApertureShape == cAS_ELLIPTICAL) && (rangeSquared >= 0.25001)) ||
             ((aBlockageShape == cBS_RECTANGULAR) && (xp <= 1.0) && (yp <= 1.0)) ||
             ((aBlockageShape == cBS_ELLIPTICAL) && ((xp * xp + yp * yp) <= 1.0)) ||
             ((aBlockageShape == cBS_DIAMOND) && (xp <= 1.0) && (yp <= (1.0 - xp))))
         {
            y += dy;
         }
         else
         {
            double s = 0.0;
            if (aApertureShape == cAS_RECTANGULAR)
            {
               s = aXAxisDistribution[i] * aYAxisDistribution[j];
            }
            else if (aApertureShape == cAS_ELLIPTICAL)
            {
               s = InterpolatedValue(aXAxisDistribution, aNumXPoints, x0 + sqrt(rangeSquared) / dx);
            }
            sum += s;
            sumSquared += s * s;
            y += dy;
         }
      }
      x += dx;
   }

   double efficiency = sum * sum / (aNumXPoints * aNumYPoints * sumSquared);
   return efficiency;
}


//! finds the 3 dB beamwidth from the final antenna pattern array, computes
//! the max sidelobe level and its position, and computes the location
//! of the first null
//! NOTE: sidelobe data not valid for a cosecant distribution
void WsfGENAP_AntennaPattern::GENAP_Data::CalculateBeamwidth(Distribution  aDistributionFunction,
                                                             int           aMaxNumPoints,
                                                             double        aMinAngle,
                                                             double        aAngularSpacing,
                                                             const double* aAntennaPattern,
                                                             double&       aBeamwidth,
                                                             double&       aMaxSidelobe,
                                                             double&       aSidelobePosition,
                                                             double&       aNullPosition)
{
   // initialize variables
   double firstTrip  = -999.0;
   double secondTrip = -999.0;
   aNullPosition     = 0.0;
   aMaxSidelobe      = -999.0;
   if (aMinAngle == 0.0) // If input pattern is symmetric
   {
      firstTrip = aMinAngle;
   }
   double angle = aMinAngle;

   for (int i = 0; i < aMaxNumPoints; ++i)
   {
      // check to see if the value of the pattern has gone above 0.5
      // (3 dB) point for the first time.  Then perform straight line
      // interpolation to determine lower 3 dB point
      if ((firstTrip == -999.0) && (aAntennaPattern[i] > 0.5))
      {
         double yIntercept =
            aAntennaPattern[i] - ((aAntennaPattern[i] - aAntennaPattern[i - 1]) / aAngularSpacing * angle);
         firstTrip = (0.5 - yIntercept) * aAngularSpacing / (aAntennaPattern[i] - aAntennaPattern[i - 1]);
      }
      else if ((secondTrip == -999.0) && (firstTrip != -999.0) && (aAntennaPattern[i] < 0.5))
      {
         // check to see if the value of the pattern has gone below 0.5
         // (3 dB) point after the lower 3 dB point has been determined.
         // then perform straight line interpolation to determine
         // the upper 3 dB point
         double yIntercept =
            aAntennaPattern[i] - ((aAntennaPattern[i] - aAntennaPattern[i - 1]) / aAngularSpacing * angle);
         secondTrip = (0.5 - yIntercept) * aAngularSpacing / (aAntennaPattern[i] - aAntennaPattern[i - 1]);
      }
      else if ((secondTrip != -999.0) && (aDistributionFunction != cDF_COSECANT))
      {
         // after both 3 dB points have been determined, locate
         // the first "null." after the first "null," search for the
         // maximum sidelobe level
         if ((aAntennaPattern[i] > aAntennaPattern[i - 1]) && (aNullPosition == 0.0))
         {
            aNullPosition = angle - aAngularSpacing;
         }
         else if ((aAntennaPattern[i] > aMaxSidelobe) && (aNullPosition != 0.0))
         {
            aMaxSidelobe      = aAntennaPattern[i];
            aSidelobePosition = angle;
         }
      }

      angle += aAngularSpacing;
   }

   // calculate the 3 dB beamwidth
   if (aMinAngle == 0.0) // If input pattern is symmetric
   {
      aBeamwidth = secondTrip * 2.0;
   }
   else
   {
      aBeamwidth = -firstTrip + secondTrip;
   }
}


//! computes the antenna gain pattern for a complex aperture distribution where the
//! samples are uniformly spaced in angle.  The sampling of the aperture is such
//! that the aperture will be divided into N equal segments with a sample in the
//! center of each segment
//! private
void WsfGENAP_AntennaPattern::GENAP_Data::PatternComplexFourierTransform(const double* aWeightingFunction,
                                                                         int           aNumApertureSamples,
                                                                         double        aApertureLength,
                                                                         int           aNumOutputSamples,
                                                                         double        aFirstAngle,
                                                                         double        aAngleSpacing,
                                                                         double*       aGainPattern)
{
   // begin calculating the pattern
   double dx      = 1.0 / aNumApertureSamples;
   double angle   = aFirstAngle;
   double gainMax = 0.0;

   for (int i = 0; i < aNumOutputSamples; ++i)
   {
      double sumR = 0.0;
      double sumI = 0.0;
      if (fabs(angle) <= (0.25 * UtMath::cTWO_PI))
      {
         double sa = sin(angle);
         double x  = 0.5 * (-1.0 + dx);

         for (int j = 0; j < aNumApertureSamples; ++j)
         {
            double arg = UtMath::cTWO_PI * x * aApertureLength * sa;
            double cc  = cos(arg);
            double ss  = sin(arg);
            sumR += aWeightingFunction[j] * cc - aWeightingFunction[aNumApertureSamples + j] * ss;
            sumI += aWeightingFunction[j] * ss + aWeightingFunction[aNumApertureSamples + j] * cc;
            x += dx;
         }
      }

      aGainPattern[i] = sumR * sumR + sumI * sumI;
      gainMax         = std::max(gainMax, aGainPattern[i]);
      angle           = aFirstAngle + ((i + 1) * aAngleSpacing);
   }

   // normalize the pattern by gMax
   double gainInverse = 1.0 / gainMax;
   VectorProduct(-aNumOutputSamples, &gainInverse, aGainPattern, aGainPattern);
}


//! computes the antenna gain pattern for a linear aperture where the samples
//! are uniformly spaced i angle.  It is assumed that the weighting function
//! is symmetrical about "zero"
void WsfGENAP_AntennaPattern::GENAP_Data::PatternFourierTransform(const double* aWeightingFunction,
                                                                  int           aNumSamples,
                                                                  double        aApertureLength,
                                                                  int           aNumOutputSamples,
                                                                  double        aFirstAngle,
                                                                  double        aAngleSpacing,
                                                                  bool          aVolt,
                                                                  double*       aGainPattern)
{
   // begin calculating the pattern
   double dx    = 1.0 / aNumSamples;
   double angle = aFirstAngle;

   for (int i = 0; i < aNumOutputSamples; ++i)
   {
      double gSum = 0.0;
      if (fabs(angle) <= (0.25 * UtMath::cTWO_PI))
      {
         double sa = sin(angle);
         double x  = 0.5 * (-1.0 + dx);

         for (int j = 0; j < aNumSamples; ++j)
         {
            double arg = UtMath::cTWO_PI * x * aApertureLength * sa;
            gSum += aWeightingFunction[j] * cos(arg);
            x += dx;
         }
      }

      aGainPattern[i] = gSum;

      // the following statement is skipped if a voltage pattern
      // is being calculated
      if (!aVolt)
      {
         aGainPattern[i] = aGainPattern[i] * aGainPattern[i];
      }
      angle = aFirstAngle + ((i + 1) * aAngleSpacing);
   }
}


//! compute the vector product of two vectors if aVectorSize > 0
//! compute the vector scale if aVectorSize < 0
void WsfGENAP_AntennaPattern::GENAP_Data::VectorProduct(int           aVectorSize,
                                                        const double* aFirstVector,
                                                        const double* aSecondVector,
                                                        double*       aOutputVector)
{
   if (aVectorSize < 0)
   {
      for (int i = 0; i < std::abs(aVectorSize); ++i)
      {
         aOutputVector[i] = aFirstVector[0] * aSecondVector[i];
      }
   }
   else if (aVectorSize > 0)
   {
      for (int i = 0; i < std::abs(aVectorSize); ++i)
      {
         aOutputVector[i] = aFirstVector[i] * aSecondVector[i];
      }
   }
}


//! returns the interpolated value A(x) where A is an array
//! of length N.  four-point interpolation is used
double WsfGENAP_AntennaPattern::GENAP_Data::InterpolatedValue(const double* aVector, int aNumPoints, double aX)
{
   int    i           = static_cast<int>(aX);
   double p           = aX - i;
   double returnValue = aVector[0];
   if (aX <= 1.0)
   {
      return returnValue;
   }

   returnValue = aVector[aNumPoints - 1];
   if (aX >= aNumPoints)
   {
      return returnValue;
   }

   if (aX < 2.0)
   {
      returnValue = (1.0 - p) * aVector[0] + p * aVector[1];
   }
   else if (aX >= (aNumPoints - 1.0))
   {
      returnValue = -p * aVector[aNumPoints - 2] + (1.0 + p) * aVector[aNumPoints - 1];
   }
   else
   {
      returnValue = 0.1666667 * p * (p - 1) * ((p + 1.0) * aVector[i + 1] - (p - 2.0) * aVector[i - 2]) +
                    0.5 * (p + 1.0) * (p - 2.0) * ((p - 1.0) * aVector[i - 1] - p * aVector[i]);
   }
   return returnValue;
}

//! vector replacement
void WsfGENAP_AntennaPattern::GENAP_Data::VectorReplace(int aVectorSize, const double* aInputVector, double* aOutputVector)
{
   if (aVectorSize < 0)
   {
      for (int i = 0; i < std::abs(aVectorSize); ++i)
      {
         aOutputVector[i] = aInputVector[0];
      }
   }
   else if (aVectorSize > 0)
   {
      for (int i = 0; i < aVectorSize; ++i)
      {
         aOutputVector[i] = aInputVector[i];
      }
   }
}


//! vector summation
double WsfGENAP_AntennaPattern::GENAP_Data::ZSum(int aVectorSize, const double* aVector)
{
   double sum = 0.0;

   for (int i = 0; i < aVectorSize; ++i)
   {
      sum += aVector[i];
   }

   return sum;
}


//! creates the Taylor One-parameter weighting function for a linear
//! aperture in array W of length N for the design sidelobe ratio in dB
void WsfGENAP_AntennaPattern::GENAP_Data::TaylorOne(double aSidelobeRatio, int aVectorSize, double* aDistribution)
{
   // use Newton's method to solve for the inverse of the
   // sinH function
   double b = 0.06 * (aSidelobeRatio - 13.0);
   for (unsigned int k = 0; k < 3; ++k)
   {
      double f1 = BetaFunction(b) - aSidelobeRatio;
      double f2 = BetaFunction(b + 0.01) - aSidelobeRatio;
      double d  = 100.0 * (f2 - f1);
      b -= f1 / d;
   }

   double beta = b;

   // compute the Taylor One-parameter distribution function
   // and store it in array aDistribution
   double pb    = UtMath::cPI * beta;
   double wNorm = ZeroBessel(pb);
   double dx    = 1.0 / aVectorSize;
   double x     = 0.5 * (-1.0 + dx);
   for (int i = 0; i < aVectorSize; ++i)
   {
      aDistribution[i] = ZeroBessel(pb * sqrt(1.0 - 4.0 * x * x)) / wNorm;
      x += dx;
   }
}


//! compute function of beta for Taylor-One
double WsfGENAP_AntennaPattern::GENAP_Data::BetaFunction(double aBeta)
{
   double value =
      20.0 * log10(4.6033 * (exp(UtMath::cPI * aBeta) - exp(-UtMath::cPI * aBeta)) / (2.0 * UtMath::cPI * aBeta));

   return value;
}


//! returns the value of the Zero-th modified Bessel function at the value
//! of the real input variable
//! polynomial approximation from Abramowitz & Stegun 9.8.1 and 9.8.2
double WsfGENAP_AntennaPattern::GENAP_Data::ZeroBessel(double aX)
{
   // polynomial expansion coefficients taken from A and S 9.8.1
   static const double c1 = 1.0;
   static const double c2 = 3.5156229;
   static const double c3 = 3.0899424;
   static const double c4 = 1.2067492;
   static const double c5 = 0.2659732;
   static const double c6 = 0.0360768;
   static const double c7 = 0.0045813;

   // polynomial expansion coefficients taken from A and S 9.8.2
   static const double d1 = 0.39894228;
   static const double d2 = 0.01328592;
   static const double d3 = 2.25319e-3;
   static const double d4 = -1.57565e-3;
   static const double d5 = 9.16281e-3;
   static const double d6 = -0.02057704;
   static const double d7 = 0.02635537;
   static const double d8 = -0.01647633;
   static const double d9 = 0.00392377;

   double xx = fabs(aX);
   double wx = xx / 3.75;
   double wj = 0.0;
   if (xx <= 3.75)
   {
      // use Abramowitz and Stegun equation 9.4.4
      wj = c1 + c2 * wx * wx + c3 * pow(wx, 4.0) + c4 * pow(wx, 6.0) + c5 * pow(wx, 8.0) + c6 * pow(wx, 10.0) +
           c7 * pow(wx, 12.0);
   }
   else // if (xx > 3.75)
   {
      // use Abramowitz and Stegun equation 9.4.4
      wj = d1 + d2 / wx + d3 / pow(wx, 2.0) + d4 / pow(wx, 3.0) + d5 / pow(wx, 4.0) + d6 / pow(wx, 5.0) +
           d7 / pow(wx, 6.0) + d8 / pow(wx, 7.0) + d9 / pow(wx, 8.0);
      wj = wj * exp(xx) / sqrt(xx);
   }
   return wj;
}


//! shifts the peak of the cosecant beam to zero degrees
void WsfGENAP_AntennaPattern::GENAP_Data::CosecantFix(int     aVectorSize,
                                                      double  aAngularSpacing,
                                                      double& aMinimumAngle,
                                                      double* aPattern)
{
   // locate the position of the peak of the beam
   double patternMax = -999.0;
   double maxAngle   = 0.0;

   for (int i = 0; i < aVectorSize; ++i)
   {
      if (aPattern[i] > patternMax)
      {
         patternMax = aPattern[i];
         maxAngle   = aMinimumAngle + aAngularSpacing * i;
      }
   }

   // make the initial adjustment to the minimum angle
   aMinimumAngle -= maxAngle;

   // since aMinimumAngle is now less than -90, some point must be thrown
   // out.  the next few lines determine how many points the pattern
   // must be shifted and the new MinimumAngle
   int    shift    = 0;
   double minAngle = aMinimumAngle;
   while (minAngle < -90.0)
   {
      ++shift;
      minAngle = aMinimumAngle + (shift * aAngularSpacing);
   }
   aMinimumAngle = minAngle;

   // shift the data points in the pattern and set the unused points to zero
   for (int i = shift; i < aVectorSize; ++i)
   {
      aPattern[i - shift] = aPattern[i];
   }

   for (int i = 0; i < shift; ++i)
   {
      aPattern[aVectorSize - i - 1] = 1.25e-30;
   }
}


//! apply a cosine distribution-like rolloff to antenna pattern
void WsfGENAP_AntennaPattern::GENAP_Data::CosecantRolloff(int     aVectorSize,
                                                          double  aMinimumAngle,
                                                          double  aAngularSpacing,
                                                          double* aPattern,
                                                          double  aBeamwidth,
                                                          double  aRolloff)
{
   double base  = 0.0;
   bool   first = true;
   for (int i = 0; i < aVectorSize; ++i)
   {
      double angle = aMinimumAngle + aAngularSpacing * i;

      if (angle >= aRolloff)
      {
         if (first)
         {
            base = aPattern[i];
         }
         first = false;
         // apply rolloff
         double psi  = 2.783 * (angle - aRolloff) / aBeamwidth;
         double sinc = 0.0;
         if (psi == 0.0)
         {
            sinc = 1.0;
         }
         else
         {
            sinc = sin(psi) / psi;
         }
         aPattern[i] = base * sinc * sinc;
      }
   }
}


//! compute K-factor for aperture width computation
double WsfGENAP_AntennaPattern::GENAP_Data::KFactor(Distribution aDistribution, double aExponent)
{
   static const int    cSIZE           = 10;
   static const double exponent[cSIZE] = {1.00, 1.25, 1.50, 1.75, 2.00, 2.25, 2.50, 2.75, 3.00, 4.00};

   static const double cosineFactor[cSIZE] = {68.1, 72.0, 75.6, 79.2, 82.5, 85.8, 87.6, 89.4, 90.9, 96.6};

   static const double cosecantFactor[cSIZE] = {101.4, 93.9, 98.4, 86.4, 84.0, 81.9, 80.4, 78.9, 77.7, 74.4};

   // K is constant for uniform
   if (aDistribution == cDF_UNIFORM)
   {
      return 50.7;
   }

   // first locate the proper exponent index
   int k = 1;
   while ((k < cSIZE) && (aExponent > exponent[k]))
   {
      ++k;
   }

   // now compute the interpolated k factor
   double y1 = 0.0;
   double y2 = 0.0;
   if (aDistribution == cDF_COSINE)
   {
      y1 = cosineFactor[k - 1];
      y2 = cosineFactor[k];
   }
   else if (aDistribution == cDF_COSECANT)
   {
      y1 = cosecantFactor[k - 1];
      y2 = cosecantFactor[k];
   }
   double ee = aExponent - exponent[k - 1];
   double x  = ee / (exponent[k] - exponent[k - 1]);
   return y1 * (1.0 - x) + y2 * x;
}


//! compute aperture weights for azimuth and elevation
void WsfGENAP_AntennaPattern::GENAP_Data::ComputeApertureWeights(double aWavelength)
{
   // the original FORTRAN code used a do loop with index 1 for azimuth
   // and 2 for elevation
   // we are not using the same structure so the code below will be duplicated
   // for azimuth and elevation

   PatternData& patternData = mPatternMap[WsfEM_Types::cPOL_DEFAULT];

   // AZIMUTH
   //
   // If the distribution is beamwidth and sidelobe specified, go
   // calculate the aperture width required to meet the
   // specifications
   if (mAzDistribution == cDF_BW_SLL)
   {
      ComputeApertureSize(patternData.mAzBeamwidth * UtMath::cDEG_PER_RAD,
                          UtMath::LinearToDB(mAzSidelobeLevel),
                          aWavelength,
                          mApertureWidth);
   }

   // calculate the number of samples across the aperture for
   // distributions other than the cosecant and user defined file.
   // For the beamwidth sidelobe distribution, check to see that
   // NMAX is not exceeded if so recalculate N(I).
   if (mAzDistribution != cDF_COSECANT)
   {
      mAzSamples = static_cast<int>(4.0 * mApertureWidth / aWavelength + 0.5);
   }
   if ((mAzDistribution == cDF_BW_SLL) && (mAzSamples > cNAZMAX))
   {
      mAzSamples = static_cast<int>(2.0 * mApertureWidth / aWavelength + 0.5);
   }

   // compute the aperture distribution function
   double dummy = 0.0;
   ApertureDistribution(mAzDistribution,
                        mAzExponent,
                        0.0,
                        mApertureWidth,
                        dummy,
                        UtMath::LinearToDB(mAzSidelobeLevel),
                        mAzSamples,
                        mAzWeights);

   // ELEVATION
   //
   // If the distribution function is cosecant calculate the
   // wavelengths across the aperture, broadside value (linear) and
   // the number of samples for the weighting function.  The number
   // of sample is first set to four times the number of
   // wavelengths across the aperture and then checked to see that
   // it is not larger than half of the maximum number of points
   // allowed.  If it is too large then the number of samples is
   // set to two times the number of wavelengths across the
   // aperture.  The cosecant distribution uses a complex weighting
   // function.  The first N samples are the real part and the
   // imaginary samples start at N+1.
   double apertureWavelengths = 0.0;
   double broadside           = 0.0;
   if (mElDistribution == cDF_COSECANT)
   {
      apertureWavelengths = mApertureHeight / aWavelength;
      broadside           = pow(10.0, -0.1); // as defined in genap.f
      mElSamples          = static_cast<int>(4.0 * mApertureHeight / aWavelength + 0.5);
      if (mElSamples > static_cast<int>(cNELMAX) * 0.5)
      {
         mElSamples = static_cast<int>(2.0 * mApertureHeight / aWavelength + 0.5);
      }
   }

   // If the distribution is beamwidth and sidelobe specified, go
   // calculate the aperture width required to meet the
   // specifications
   if (mElDistribution == cDF_BW_SLL)
   {
      ComputeApertureSize(patternData.mElBeamwidth * UtMath::cDEG_PER_RAD,
                          UtMath::LinearToDB(mElSidelobeLevel),
                          aWavelength,
                          mApertureHeight);
   }

   // calculate the number of samples across the aperture for
   // distributions other than the cosecant and user defined file.
   // For the beamwidth sidelobe distribution, check to see that
   // NMAX is not exceeded if so recalculate N(I).
   if (mElDistribution != cDF_COSECANT)
   {
      mElSamples = static_cast<int>(4.0 * mApertureHeight / aWavelength + 0.5);
   }
   if ((mElDistribution == cDF_BW_SLL) && (mElSamples > cNELMAX))
   {
      mElSamples = static_cast<int>(2.0 * mApertureHeight / aWavelength + 0.5);
   }

   // compute the aperture distribution function
   if (mElDistribution == cDF_COSECANT)
   {
      ApertureDistribution(mElDistribution,
                           mElExponent,
                           broadside,
                           apertureWavelengths,
                           mElEfficiency,
                           UtMath::LinearToDB(mElSidelobeLevel),
                           mElSamples,
                           mElWeights);
   }
   else
   {
      ApertureDistribution(mElDistribution,
                           mElExponent,
                           broadside,
                           mApertureHeight,
                           mElEfficiency,
                           UtMath::LinearToDB(mElSidelobeLevel),
                           mElSamples,
                           mElWeights);
   }

   if (mElDistribution == cDF_COSECANT)
   {
      double s1 = 0.0;
      double s2 = 0.0;
      for (int i = 0; i < mAzSamples; ++i)
      {
         s1 += mAzWeights[i];
         s2 += mAzWeights[i] * mAzWeights[i];
      }
      mElEfficiency *= s1 * s1 / (mAzSamples * s2);
   }
}


//! calculate crude values for the azimuth and elevation beamwidths
//! these beamwidths will be used to calculate the angular increment
//! required in the final antenna pattern
void WsfGENAP_AntennaPattern::GENAP_Data::ComputeCrudePattern(double aWavelength)
{
   // the original FORTRAN code used a do loop with index 1 for azimuth
   // and 2 for elevation
   // we are not using the same structure so the code below will be duplicated
   // for azimuth and elevation

   PatternData& patternData = mPatternMap[WsfEM_Types::cPOL_DEFAULT];

   // AZIMUTH
   //
   double dwl = mApertureWidth / aWavelength;
   double pattern[81];
   for (int n = 0; n < 81; ++n)
   {
      pattern[n] = -1.0;
   }
   PatternFourierTransform(mAzBlockageWeights, mAzSamples, dwl, 81, -0.05 / dwl * 15.0, 0.05 / dwl, false, pattern);

   double gNorm = 0.0;
   for (int i = 0; i < 81; ++i)
   {
      gNorm = std::max(gNorm, pattern[i]);
      if (pattern[i] <= 0.0)
      {
         pattern[i] = 1.25e-30;
      }
   }
   double inverseNorm = 1.0 / gNorm;
   VectorProduct(-81, &inverseNorm, pattern, pattern);

   double azBeamwidth = patternData.mAzBeamwidth * UtMath::cDEG_PER_RAD;
   CalculateBeamwidth(mAzDistribution,
                      81,
                      -0.05 / dwl * 15.0 * UtMath::cDEG_PER_RAD,
                      0.05 / dwl * UtMath::cDEG_PER_RAD,
                      pattern,
                      azBeamwidth,
                      mAzSidelobeLevel,
                      mAzSidelobePosition,
                      mAzNullPosition);
   patternData.mAzBeamwidth = azBeamwidth * UtMath::cRAD_PER_DEG;

   // ELEVATION
   //
   if (mElDistribution == cDF_COSECANT)
   {
      dwl = mApertureHeight / aWavelength;
      PatternComplexFourierTransform(mElBlockageWeights, mElSamples, dwl, 81, -0.05 / dwl * 15.0, 0.05 / dwl, pattern);
      double elBeamwidth = patternData.mElBeamwidth * UtMath::cDEG_PER_RAD;
      CalculateBeamwidth(mElDistribution,
                         81,
                         -0.05 / dwl * 15.0 * UtMath::cDEG_PER_RAD,
                         0.05 / dwl * UtMath::cDEG_PER_RAD,
                         pattern,
                         elBeamwidth,
                         mElSidelobeLevel,
                         mElSidelobePosition,
                         mElNullPosition);
   }
   else
   {
      dwl = mApertureHeight / aWavelength;
      PatternFourierTransform(mElBlockageWeights, mElSamples, dwl, 81, -0.05 / dwl * 15.0, 0.05 / dwl, false, pattern);

      gNorm = 0.0;
      for (int i = 0; i < 81; ++i)
      {
         gNorm = std::max(gNorm, pattern[i]);
         if (pattern[i] <= 0.0)
         {
            pattern[i] = 1.25e-30;
         }
      }
      double normInverse = 1.0 / gNorm;
      VectorProduct(-81, &normInverse, pattern, pattern);

      double elBeamwidth = patternData.mElBeamwidth * UtMath::cDEG_PER_RAD;
      CalculateBeamwidth(mElDistribution,
                         81,
                         -0.05 / dwl * 15.0 * UtMath::cDEG_PER_RAD,
                         0.05 / dwl * UtMath::cDEG_PER_RAD,
                         pattern,
                         elBeamwidth,
                         mElSidelobeLevel,
                         mElSidelobePosition,
                         mElNullPosition);
      patternData.mElBeamwidth = elBeamwidth * UtMath::cRAD_PER_DEG;
   }
}


//! create the complex weighting function for a cosecant-type
//! elevation beam
void WsfGENAP_AntennaPattern::GENAP_Data::CosecantBeam(double  aExponent,
                                                       double  aBroadside,
                                                       double  aApertureSize,
                                                       int     aNumSamples,
                                                       double* aRealPattern,
                                                       double* aImagPattern,
                                                       double& aEfficiency)
{
   int nl = static_cast<int>(aApertureSize + 1.0);

   // initialize the array F
   double f[101] = {0.0};
   for (int i = 1; i < nl; ++i)
   {
      f[i] = pow((1.0 / (i)), (0.5 * aExponent));
   }
   // genap.f has the broadsideDB hard-coded to -1.0
   // f[0] = f[1] * pow(10.0, (0.05 * UtMath::LinearToDB(aBroadside)));
   f[0] = f[1] * pow(10.0, (0.05 * -1.0));

   double dx = 1.0 / aNumSamples;
   double x  = 0.5 * (-1.0 + dx);

   // calculate the real and imaginary parts of the weighting function
   for (int i = 0; i < aNumSamples; ++i)
   {
      aRealPattern[i] = 0.0;
      aImagPattern[i] = 0.0;

      for (int j = 0; j < nl; ++j)
      {
         double arg = -UtMath::cTWO_PI * j * x;
         aRealPattern[i] += f[j] * cos(arg);
         aImagPattern[i] += f[j] * sin(arg);
      }

      x += dx;
   }

   // calculate the efficiency
   double sum = 0.0;
   for (int i = 0; i < nl; ++i)
   {
      sum += f[i] * f[i];
   }

   aEfficiency = f[1] * f[1] / sum;
}


//! compute total energy
double WsfGENAP_AntennaPattern::GENAP_Data::Energy(int aNum, const double* aRealPattern, const double* aImagPattern)
{
   double energy = 0.0;
   for (int i = 0; i < aNum; ++i)
   {
      energy += aRealPattern[i] * aRealPattern[i] + aImagPattern[i] * aImagPattern[i];
   }
   return energy;
}


//! calculates the aperture size needed to product an antenna
//! pattern based on the Taylor One-Parameter distribution.
void WsfGENAP_AntennaPattern::GENAP_Data::ComputeApertureSize(double  aBeamwidth,
                                                              double  aSidelobeLevelDB,
                                                              double  aWavelength,
                                                              double& aApertureSize)
{
   // initialize factor points for sidelobe levels of 15 - 55 dB in
   // 5 dB increments
   static const double data[9] = {52.90419, 58.66695, 63.94779, 68.78490, 73.24518, 77.39154, 81.27618, 84.94086, 88.41831};

   // locate desired sidelobe ratio index
   int    i      = static_cast<int>((aSidelobeLevelDB - 15.0) / 5.0);
   double slrval = 15.0 + i * 5.0;

   double yIntercept = data[i] - (data[i + 1] - data[i]) * slrval / 5.0;
   double factor     = (data[i + 1] - data[i]) / 5.0 * aSidelobeLevelDB + yIntercept;

   // calculate the aperture size that corresponds to the desired beamwidth
   // and sidelobe ratio
   aApertureSize = factor * aWavelength / aBeamwidth;
}
