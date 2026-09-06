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

#include "WsfStandardAntennaPattern.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtStringUtil.hpp"

WsfStandardAntennaPattern::WsfStandardAntennaPattern()
   : WsfAntennaPattern(new StandardData())
{
}

WsfStandardAntennaPattern::WsfStandardAntennaPattern(const WsfStandardAntennaPattern& aSrc)
   : WsfAntennaPattern(aSrc)
{
}

// virtual
WsfStandardAntennaPattern::~WsfStandardAntennaPattern() {}

// virtual
WsfAntennaPattern* WsfStandardAntennaPattern::Clone() const
{
   return new WsfStandardAntennaPattern(*this);
}

// virtual
double WsfStandardAntennaPattern::GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El)
{
   return GetStandardData()->GetGain(aFrequency, aTargetAz, aTargetEl, aEBS_Az, aEBS_El);
}

// virtual
double WsfStandardAntennaPattern::GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(GetStandardData()->mAzBeamwidth, aEBS_Azimuth, 0.0);
}

// virtual
double WsfStandardAntennaPattern::GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   return ApplyEBS(GetStandardData()->mElBeamwidth, 0.0, aEBS_Elevation);
}

// virtual
double WsfStandardAntennaPattern::GetPeakGain(double aFrequency) const
{
   return GetStandardData()->PerformGainAdjustment(aFrequency, GetStandardData()->mPeakGain);
}


// =================================================================================================
// Nested class 'StandardData'.
// =================================================================================================
WsfStandardAntennaPattern::StandardData::StandardData()
   : BaseData()
   , mPatternType(cUNIFORM_PATTERN)
   , mTablePtr(nullptr)
   , mPeakGain(1.0)
   , mInputAzBeamwidth(UtMath::cTWO_PI)
   , mInputElBeamwidth(UtMath::cPI)
   , mAzBeamwidth(UtMath::cTWO_PI)
   , mElBeamwidth(UtMath::cPI)
   , mSineAngleModificationFactor(1.0)
   , mSineFunctionType(1)
   , mCscMinElForPeakGain(0.0)
   , mCscElOfPeakCsc2Boundary(0.0)
   , mCscMaxElForCsc2(0.0)
   , mCscMinimumGainAdjusted(0.0)
   , mCscPeakGainAdjusted(0.0)
   , mCscK_Az(0.0)
   , mCscK_El(0.0)
   , mCscWithinCsc2Factor(0.0)
   , mCscAboveCsc2Factor(0.0)
   , mFirstTime(true)
{
}

WsfStandardAntennaPattern::StandardData::~StandardData()
{
   delete mTablePtr;
}

bool WsfStandardAntennaPattern::StandardData::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool         myCommand = true;
   std::string  command(aInput.GetCommand());
   UtAzElTable* tablePtr = nullptr;
   std::string  tableUnits("db");

   if (UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits) && (tablePtr != nullptr))
   {
      ResetPatternType();

      UtStringUtil::ToLower(tableUnits);
      if (tableUnits == "db")
      {
         // Input values are in dB
         tablePtr->ConvertValuesFromDB(1.0F);
      }
      else if (tableUnits != "absolute")
      {
         throw UtInput::BadValue(aInput, command + ": table units must be 'db' or 'absolute'");
      }
      mTablePtr    = tablePtr;
      mPatternType = cTABLE;

      // Determine the peak gain.
      mPeakGain      = 0.0;
      size_t numRows = mTablePtr->mAzValues.GetSize();
      size_t numCols = mTablePtr->mElValues.GetSize();
      for (size_t row = 0; row < numRows; ++row)
      {
         for (size_t col = 0; col < numCols; ++col)
         {
            mPeakGain = std::max(mPeakGain, static_cast<double>(mTablePtr->mDataValues.Get(row, col)));
         }
      }
   }
   else if (command == "cosecant_squared_pattern")
   {
      ResetPatternType();
      ProcessCosecantSquaredInput(aPattern, aInput);
      mPatternType = cCOSECANT_SQUARED_PATTERN;
   }
   else if ((command == "circular_pattern") || (command == "lambda_pattern"))
   {
      ResetPatternType();
      ProcessCircularInput(aPattern, aInput);
      mPatternType = cCIRCULAR_PATTERN;
   }
   else if (command == "pattern_table")
   {
      ResetPatternType();
      ProcessPatternTableInput(aPattern, aInput);
      mPatternType = cTABLE;
   }
   else if ((command == "rectangular_pattern") || (command == "sine_pattern"))
   {
      ResetPatternType();
      ProcessSineInput(aPattern, aInput);
      mPatternType = cSINE_PATTERN;
   }
   else if ((command == "uniform_pattern") || (command == "constant_pattern"))
   {
      ResetPatternType();
      ProcessUniformInput(aPattern, aInput);
      mPatternType = cUNIFORM_PATTERN;
   }
   else
   {
      myCommand = WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput);
   }
   return myCommand;
}

// virtual
bool WsfStandardAntennaPattern::StandardData::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   return BaseData::Initialize(aAntennaPattern);
}

// virtual
double WsfStandardAntennaPattern::StandardData::GetGain(double aFrequency,
                                                        double aTargetAz,
                                                        double aTargetEl,
                                                        double aEBS_Az,
                                                        double aEBS_El)
{
   double gain = 1.0;
   switch (mPatternType)
   {
   case cTABLE:
   {
      UtAzElLookup context;
      mTablePtr->GetContext(context);
      gain = context.Lookup(aTargetAz, aTargetEl);
   }
   break;
   case cCIRCULAR_PATTERN:
      gain = CircularPattern(aTargetAz, aTargetEl, mPeakGain, mAzBeamwidth * UtMath::cDEG_PER_RAD);
      break;
   case cCOSECANT_SQUARED_PATTERN:
      gain = CosecantSquaredPattern(aTargetAz, aTargetEl);
      break;
   case cSINE_PATTERN:
      gain = SinePattern(aTargetAz,
                         aTargetEl,
                         mPeakGain,
                         mAzBeamwidth * UtMath::cDEG_PER_RAD,
                         mElBeamwidth * UtMath::cDEG_PER_RAD,
                         0.0,
                         mSineAngleModificationFactor,
                         mSineFunctionType);
      break;
   case cUNIFORM_PATTERN:
      gain = mPeakGain;
      if ((fabs(aTargetAz) > (0.5 * mAzBeamwidth)) || (fabs(aTargetEl) > (0.5 * mElBeamwidth)))
      {
         // We exit directly with the minimum gain at this point without applying any
         // user-specified gain adjustment. We are outside the beam and the definition of
         // this pattern says we return the minimum gain when this occurs.
         return mMinimumGain;
      }
      break;
   default: // To avoid compiler warning
      break;
   }

   // Perform user-specified gain adjustment and lower-bound limiting
   gain = PerformGainAdjustment(aFrequency, gain);
   return gain;
}

// private
void WsfStandardAntennaPattern::StandardData::ProcessCircularInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "peak_gain")
      {
         aInput.ReadValueOfType(mPeakGain, UtInput::cRATIO);
         aInput.ValueGreater(mPeakGain, 0.0);
      }
      else if (command == "beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mElBeamwidth      = mAzBeamwidth;
         mInputAzBeamwidth = mAzBeamwidth;
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "end_antenna_pattern") // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
      else if (!WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// private
void WsfStandardAntennaPattern::StandardData::ProcessCosecantSquaredInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   std::string command;

   // The constructor initializes the azimuth and elevation beamwidths to 2pi and pi respectively,
   // but they are REQUIRED values. Initialize them to zero so we can easily tell what was entered.

   mAzBeamwidth = 0.0;
   mElBeamwidth = 0.0;

   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "peak_gain")
      {
         aInput.ReadValueOfType(mPeakGain, UtInput::cRATIO);
         aInput.ValueGreater(mPeakGain, 0.0);
      }
      else if (command == "azimuth_beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mInputAzBeamwidth = mAzBeamwidth;
      }
      else if (command == "elevation_beamwidth")
      {
         aInput.ReadValueOfType(mElBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mElBeamwidth, 0.0);
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "minimum_elevation_for_peak_gain")
      {
         aInput.ReadValueOfType(mCscMinElForPeakGain, UtInput::cANGLE);
         // Documentation says this is supposed to be >= 0, but some input files had negative values.
         // aInput.ValueGreaterOrEqual(mCscMinElForPeakGain, 0.0);
      }
      else if (command == "elevation_of_peak/csc2_boundary")
      {
         aInput.ReadValueOfType(mCscElOfPeakCsc2Boundary, UtInput::cANGLE);
         aInput.ValueGreater(mCscElOfPeakCsc2Boundary, 0.0);
      }
      else if (command == "maximum_elevation_for_csc2")
      {
         aInput.ReadValueOfType(mCscMaxElForCsc2, UtInput::cANGLE);
         aInput.ValueGreater(mCscMaxElForCsc2, 0.0);
      }
      else if (command == "end_antenna_pattern") // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
      else if (!WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   // If the elevation beamwidth was not specified then default to the azimuth beamwidth.
   // This is for compatibility with old input files that did not specify a value.

   if (mElBeamwidth == 0.0)
   {
      mElBeamwidth      = mAzBeamwidth;
      mInputElBeamwidth = mAzBeamwidth;
   }

   if ((mAzBeamwidth == 0.0) || (mElBeamwidth == 0.0))
   {
      throw UtInput::BadValue(aInput, "Azimuth and elevation beamwidth must be non-zero");
   }

   // Documentation says minElForPeak < elOfPeakCsc2Boundary < maxElForCsc2, but some input files had the
   // boundary elevation greater than the maxElForCsc2. This had to be disabled so these input files would run.
#if 0
   if ((mCscElOfPeakCsc2Boundary <= mCscMinElForPeakGain) ||
       (mCscMaxElForCsc2         <= mCscElOfPeakCsc2Boundary))
   {
      throw UtInput::BadValue(aInput, "Invalid relationship between angles");
   }
#endif
}

// private
void WsfStandardAntennaPattern::StandardData::ProcessPatternTableInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   UtAzElTable* tablePtr = nullptr;
   std::string  tableUnits("db");
   bool         findPeakGain = true;

   while (inputBlock.ReadCommand(command))
   {
      if (UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits) && (tablePtr != nullptr))
      {
         delete mTablePtr;
         UtStringUtil::ToLower(tableUnits);
         if (tableUnits == "db")
         {
            // Input values are in dB
            tablePtr->ConvertValuesFromDB(1.0F);
         }
         else if (tableUnits != "absolute")
         {
            throw UtInput::BadValue(aInput, command + ": table values must be 'db' or 'absolute'");
         }
         mTablePtr = tablePtr;
      }
      else if (command == "peak_gain")
      {
         aInput.ReadValueOfType(mPeakGain, UtInput::cRATIO);
         aInput.ValueGreater(mPeakGain, 0.0);
         findPeakGain = false;
      }
      else if (command == "azimuth_beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mInputAzBeamwidth = mAzBeamwidth;
      }
      else if (command == "elevation_beamwidth")
      {
         aInput.ReadValueOfType(mElBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mElBeamwidth, 0.0);
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "end_antenna_pattern") // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
      else if (!WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (mTablePtr == nullptr)
   {
      throw UtInput::BadValue(aInput, "A table must be defined");
   }
   if ((mAzBeamwidth == 0.0) || (mElBeamwidth == 0.0))
   {
      throw UtInput::BadValue(aInput, "Azimuth and elevation beamwidth must be non-zero");
   }

   if (findPeakGain)
   {
      // Determine the peak gain.
      mPeakGain      = 0.0;
      size_t numRows = mTablePtr->mAzValues.GetSize();
      size_t numCols = mTablePtr->mElValues.GetSize();
      for (size_t row = 0; row < numRows; ++row)
      {
         for (size_t col = 0; col < numCols; ++col)
         {
            mPeakGain = std::max(mPeakGain, static_cast<double>(mTablePtr->mDataValues.Get(row, col)));
         }
      }
   }
}

// private
void WsfStandardAntennaPattern::StandardData::ProcessSineInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "peak_gain")
      {
         aInput.ReadValueOfType(mPeakGain, UtInput::cRATIO);
         aInput.ValueGreater(mPeakGain, 0.0);
      }
      else if (command == "beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mElBeamwidth      = mAzBeamwidth;
         mInputAzBeamwidth = mAzBeamwidth;
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "azimuth_beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mInputAzBeamwidth = mAzBeamwidth;
      }
      else if (command == "elevation_beamwidth")
      {
         aInput.ReadValueOfType(mElBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mElBeamwidth, 0.0);
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "circular")
      {
         mSineFunctionType = 1;
      }
      else if (command == "rectangular")
      {
         mSineFunctionType = 4;
      }
      else if (command == "angle_modification_factor")
      {
         aInput.ReadValue(mSineAngleModificationFactor);
         aInput.ValueGreater(mSineAngleModificationFactor, 0.0);
      }
      else if (command == "end_antenna_pattern") // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
      else if (!WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// private
void WsfStandardAntennaPattern::StandardData::ProcessUniformInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "peak_gain")
      {
         aInput.ReadValueOfType(mPeakGain, UtInput::cRATIO);
         aInput.ValueGreater(mPeakGain, 0.0);
      }
      else if (command == "beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mElBeamwidth      = mAzBeamwidth;
         mInputAzBeamwidth = mAzBeamwidth;
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "azimuth_beamwidth")
      {
         aInput.ReadValueOfType(mAzBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mAzBeamwidth, 0.0);
         mInputAzBeamwidth = mAzBeamwidth;
      }
      else if (command == "elevation_beamwidth")
      {
         aInput.ReadValueOfType(mElBeamwidth, UtInput::cANGLE);
         aInput.ValueGreater(mElBeamwidth, 0.0);
         mInputElBeamwidth = mElBeamwidth;
      }
      else if (command == "end_antenna_pattern") // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
      else if (!WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput))
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
}

// private
void WsfStandardAntennaPattern::StandardData::ResetPatternType()
{
   delete mTablePtr;
   mTablePtr    = nullptr;
   mPatternType = cUNIFORM_PATTERN;
}

//! Calculate Bessel function J1(x)

// static
double WsfStandardAntennaPattern::StandardData::BesselJ1(double aX)
{
   // Note: Added a [0] element to allow direct copying of the FORTRAN code.

   static const double a[8] = {0.0, 1.109E-5, -3.1761E-4, 4.43319E-3, -3.954289E-2, .2109357, -.5624999, .5};
   static const double b[8] = {0.0, -2.0033E-4, 1.13653E-3, -2.49511E-3, 1.7105E-4, 1.659667E-2, 1.56E-6, .7978846};
   static const double c[8] = {0.0, -2.9166E-4, 7.9824E-4, 7.4348E-4, -6.37879E-3, 5.65E-5, .1249961, -2.356194};

   double q  = 0.0;
   double xa = fabs(aX);
   if (xa <= 3.0)
   {
      double y = (xa * xa) / 9.0;
      q        = ((((((a[1] * y + a[2]) * y + a[3]) * y + a[4]) * y + a[5]) * y + a[6]) * y + a[7]) * xa;
   }
   else
   {
      double y      = 3.0 / xa;
      double f1     = (((((b[1] * y + b[2]) * y + b[3]) * y + b[4]) * y + b[5]) * y + b[6]) * y + b[7];
      double theta1 = xa + (((((c[1] * y + c[2]) * y + c[3]) * y + c[4]) * y + c[5]) * y + c[6]) * y + c[7];
      q             = f1 * cos(theta1) / sqrt(xa);
   }
   if (aX < 0.0)
   {
      q = -q;
   }
   return q;
}

//! Calculate the gain in a cosecant squared pattern
//!
//! This is a direct conversion of the routine COSEC2.
//! (Author: W RING /A GUENDEL  MAR 1984)

double WsfStandardAntennaPattern::StandardData::CosecantSquaredPattern(double aAzimuth, double aElevation)
{
   static const double cCONST_T1 = 2.78;
   static const double cCONST_T2 = 0.230258;

   // Perform first time calculations

   if (mFirstTime)
   {
      mFirstTime              = false;
      mCscK_Az                = cCONST_T1 / mAzBeamwidth;
      mCscK_El                = cCONST_T1 / mElBeamwidth;
      mCscMinimumGainAdjusted = exp(cCONST_T2 * 10.0 * log10(mMinimumGain));
      mCscPeakGainAdjusted    = exp(cCONST_T2 * 10.0 * log10(mPeakGain));
      double sinx             = sin(mCscElOfPeakCsc2Boundary);
      mCscWithinCsc2Factor    = mCscPeakGainAdjusted * (sinx * sinx);
      sinx                    = sin(mCscMaxElForCsc2);
      mCscAboveCsc2Factor     = mCscWithinCsc2Factor / (sinx * sinx);
   }

   // Compute azimuth dependent gain distribution

   double azGain = 1.0; // gain when azimuth == 0.0
   if (aAzimuth != 0.0)
   {
      double x           = mCscK_Az * aAzimuth;
      double sinx_over_x = sin(x) / x;
      azGain             = sinx_over_x * sinx_over_x;
   }

   // Compute elevation dependent gain distribution

   double elGain = 1.0;
   if (aElevation < mCscMinElForPeakGain)
   {
      // Elevation is below the peak gain region
      double ee          = aElevation - mCscMinElForPeakGain;
      double x           = mCscK_El * ee;
      double sinx_over_x = sin(x) / x;
      elGain             = (sinx_over_x * sinx_over_x) * mCscPeakGainAdjusted;
   }
   else if (aElevation < mCscElOfPeakCsc2Boundary)
   {
      // Elevation is in the peak gain region
      elGain = mCscPeakGainAdjusted;
   }
   else if (aElevation < mCscMaxElForCsc2)
   {
      // Elevation is in the cosecant^2 region
      double sinx = sin(aElevation);
      elGain      = mCscWithinCsc2Factor / (sinx * sinx);
   }
   else
   {
      // Elevation is above the cosecant^2 region
      double ee          = aElevation - mCscMaxElForCsc2;
      double x           = mCscK_El * ee;
      double sinx_over_x = sin(x) / x;
      elGain             = (sinx_over_x * sinx_over_x) * mCscAboveCsc2Factor;
   }

   // Compute the total gain
   double gain = azGain * elGain;
   return gain;
}

//! Get gain from a uniformly illuminated circular aperture.
//!
//! @param aAzimuth Azimuth angle for lookup.
//! @param aElevation Elevation angle for lookup.
//! @param aBeamwidth Beamwidth of the mainbeam (radians????)
//! @param aPeakGain The peak gain in the pattern.
//! @return Absolute gain (NOT in dB).
//!
//! This is a direct conversion of the routine GENCIR.
//! (Author: A GUENDEL 27 MAR 1984)
// static
double WsfStandardAntennaPattern::StandardData::CircularPattern(double aAzimuth,
                                                                double aElevation,
                                                                double aPeakGain,
                                                                double aBeamwidth)
{
   double gain = 1.0;
   double barg = acos(cos(aAzimuth) * cos(aElevation));
   if (fabs(barg) >= 0.001)
   {
      // If azimuth > 90 deg then barg = 90 deg
      if (fabs(aAzimuth) > UtMath::cPI_OVER_2)
      {
         barg = UtMath::cPI_OVER_2;
      }
      // I'm not sure where the 58.4 come from (it was in GENCIR)
      double pidu = UtMath::cPI * 58.4 * sin(barg) / aBeamwidth;
      double a    = 2.0 * BesselJ1(pidu);
      gain        = (a * a) / (pidu * pidu);
   }
   gain *= aPeakGain;
   return gain;
}

//! From the routine GENANT
// static
double WsfStandardAntennaPattern::StandardData::SinePattern(double aAzimuth,
                                                            double aElevation,
                                                            double aPeakGain,
                                                            double aAzimuthBeamwidth,
                                                            double aElevationBeamwidth,
                                                            double aOffset,
                                                            double aFact,
                                                            int    aType)
{
   // amagic converts radians to degrees and multiplies by:
   //  2 to get the full beam width
   //  1.616 which is the value of X where 2*BesselJ1(X)/X = .707
   // bmajic converts radians to degrees and multiplies by:
   //  2 to get full beam width
   //  1.393 which is the value of X where sin(X)/X = .707

   static const double amagic = 185.17995;
   static const double bmagic = 159.626;

   double az     = aAzimuth;
   double el     = aElevation;
   double agzero = aPeakGain;
   double hbw    = aAzimuthBeamwidth;
   double vbw    = aElevationBeamwidth;
   double offset = aOffset;
   double fact   = aFact;
   int    itype  = aType;

   double gain = agzero;

   double oel = el - offset; // * DEGRAD

   if ((az != 0.0) || (oel != 0.0))
   {
      double taz  = az / fact;
      double toel = oel / fact;

      double caz = cos(taz);
      double tel = tan(toel);

      double rot = 0.0;
      if ((caz != 0.0) || (tel != 0.0))
      {
         rot = atan2(tel, caz);
      }

      if (itype == 4)
      {
         // Antenna type = B
         double t1   = taz / hbw * fact;
         double t2   = fact * rot / vbw;
         double tang = bmagic * sqrt(t1 * t1 + t2 * t2);
         double t3   = sin(tang) / tang;
         gain        = agzero * t3 * t3;
      }
      else
      {
         // Antenna type = A
         double t1   = taz / hbw * fact;
         double t2   = fact * rot / vbw;
         double tang = amagic * sqrt(t1 * t1 + t2 * t2);
         double t3   = 2.0 * BesselJ1(tang) / tang;
         gain        = agzero * t3 * t3;

         // Set the lowest value of X where J1(X) = 0.0.
         double ttest = 3.832;

         // Check for sidelobe gain.  If so then reduce gain by 6 dB.
         if (tang >= ttest)
         {
            gain /= pow(10.0, 6.0 / 10.0);
         }
      }
   }
   return gain;
}
