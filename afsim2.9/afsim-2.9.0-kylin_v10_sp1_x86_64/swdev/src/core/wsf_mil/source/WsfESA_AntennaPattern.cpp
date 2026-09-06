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

#include "WsfESA_AntennaPattern.hpp"

#include <algorithm>
#include <cfloat> // for DBL_MAX
#include <cmath>
#include <unordered_set>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "WsfAntennaPattern.hpp"
#include "WsfAntennaPatternTypes.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStandardAntennaPattern.hpp"

WsfESA_AntennaPattern::WsfESA_AntennaPattern(ESA_Data* aEsaData)
   : WsfAntennaPattern(aEsaData != nullptr ? aEsaData : new ESA_Data)
   , mElements()
   , mLengthX(0.0)
   , mLengthY(0.0)
   , mNX(0)
   , mNY(0)
   , mWeightVecX()
   , mWeightVecY()
   , mApertureEff(1.0)
   , mApertureEffX(1.0)
   , mApertureEffY(1.0)
   , mElementPatternPtr(nullptr)
{
   mESA_DataPtr = dynamic_cast<ESA_Data*>(mSharedDataPtr);
}

WsfESA_AntennaPattern::WsfESA_AntennaPattern(const WsfESA_AntennaPattern& aSrc)
   : WsfAntennaPattern(aSrc)
   , mElements()
   , mLengthX(0.0)
   , mLengthY(0.0)
   , mNX(0)
   , mNY(0)
   , mWeightVecX()
   , mWeightVecY()
   , mApertureEff(aSrc.mApertureEff)
   , mApertureEffX(aSrc.mApertureEffX)
   , mApertureEffY(aSrc.mApertureEffY)
   , mElementPatternPtr(nullptr)
   , mESA_DataPtr(aSrc.mESA_DataPtr)
{
   mESA_DataPtr = dynamic_cast<ESA_Data*>(mSharedDataPtr);
   Initialize(nullptr);
}

// virtual
WsfESA_AntennaPattern::~WsfESA_AntennaPattern()
{
   delete mElementPatternPtr;
}

//! Factory method for WsfAntennaPatternTypes.
//! This is called by WsfAntennaPatternTypes::LoadType to determine if a pattern
//! represented by this class is being requested.
// static
WsfAntennaPattern* WsfESA_AntennaPattern::ObjectFactory(const std::string& aTypeName)
{
   WsfAntennaPattern* patternPtr = nullptr;
   if (aTypeName == "esa_pattern")
   {
      patternPtr = new WsfESA_AntennaPattern();
   }
   return patternPtr;
}

// virtual
WsfAntennaPattern* WsfESA_AntennaPattern::Clone() const
{
   return new WsfESA_AntennaPattern(*this);
}

//! Return the antenna gain at a specified azimuth and elevation.
//!
//! @param aFrequency The frequency at which to get the gain (Hz).
//! @param aTargetAz  Target azimuth with respect to the gain pattern boresight (radians).
//! @param aTargetEl  Target elevation with respect to the gain pattern boresight (radians).
//! @param aEBS_Az    The Electronic Beam Steering azimuth angle (radians).
//! @param aEBS_El    The Electronic Beam Steering elevation angle (radians).
//! @return The gain multiplier (NOT in dB!)
// virtual
double WsfESA_AntennaPattern::GetGain(double aFrequency, double aTargetAz, double aTargetEl, double aEBS_Az, double aEBS_El)
{
   double gain = 1.0;

   double gainAz = aTargetAz + aEBS_Az;
   double gainEl = aTargetEl + aEBS_El;

   if (mESA_DataPtr->mBackBaffled &&
       ((fabs(gainAz) > (UtMath::cPI_OVER_2 + 0.00001)) || (fabs(gainEl) > (UtMath::cPI_OVER_2 + 0.00001))))
   {
      gain = mESA_DataPtr->mMinimumGain;
   }
   else
   {
      gain = ComputeGain(aFrequency, gainAz, gainEl, aEBS_Az, aEBS_El);
      // Perform user-specified gain adjustment and lower-bound limiting
      gain = mESA_DataPtr->PerformGainAdjustment(aFrequency, gain);
   }
   return gain;
}

// private
double WsfESA_AntennaPattern::ComputeGain(double aFrequency, double aAzAngle, double aElAngle, double aEBS_Az, double aEBS_El) const
{
   // if (aFrequency <= 0.0)
   //{
   //    ut::log::warning() << "Frequency of '0' entered for " << GetName()
   //       << " an electronically scanned/steered array antenna pattern!"
   //       << " Will be using a matched element to calculate the gain pattern.";
   // }

   double gain = ComputeElementFactor(aFrequency, aEBS_Az, aEBS_El, aAzAngle, aElAngle) *
                 ComputeArrayFactor(aFrequency, aEBS_Az, aEBS_El, aAzAngle, aElAngle);

   return gain;
}

//! Return the azimuth beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return The azimuth beamwidth (radians)
// virtual
double WsfESA_AntennaPattern::GetAzimuthBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   double dX = mESA_DataPtr->mdX;
   // assume that the spacing is lambda/2 by default
   double BW = (sqrt(3.0) / mNX);
   if ((aFrequency > 0.0) && (mNX != 1) && (dX > 0.0))
   {
      double lambda = UtMath::cLIGHT_SPEED / aFrequency;

      // Including the aperture efficiency gives a better answer than leaving it out
      // but it is dependent on the efficiency in the x-direction only.
      BW = ((sqrt(3.0) / 2) * (lambda / (mNX * dX))) / mApertureEffX;
   }
   BW = ApplyEBS(BW, aEBS_Azimuth, 0.0);

   return BW;
}

//! Return the elevation beamwidth at the specified frequency.
//! The beamwidth is defined to be the width of the main lobe where the gain is 3 dB
//! down from the peak gain.
//! @param aFrequency The frequency in Hz.
//! @param aEBS_Azimuth The Electronic Beamwidth Steering in terms of Azimuth (radians)
//! @param aEBS_Elevation The Electronic Beamwidth Steering in terms of Elevation (radians)
//! @return The azimuth beamwidth (radians)
// virtual
double WsfESA_AntennaPattern::GetElevationBeamwidth(double aFrequency, double aEBS_Azimuth, double aEBS_Elevation) const
{
   double dY = mESA_DataPtr->mdY;
   // assume that the spacing is lambda/2 by default
   double BW = sqrt(3.0) / mNY;
   if ((aFrequency > 0.0) && (mNY != 1) && (dY > 0.0))
   {
      double lambda = UtMath::cLIGHT_SPEED / aFrequency;

      // Including the aperture efficiency gives a better answer than leaving it out
      // but it is dependent on the efficiency in the y-direction only.
      BW = ((sqrt(3.0) / 2) * (lambda / (mNY * dY))) / mApertureEffY;
   }
   BW = ApplyEBS(BW, 0.0, aEBS_Elevation);

   return BW;
}

//! Return the peak (boresight) gain in the pattern at the specified frequency.
//! @param aFrequency The frequency in Hz.
//! @return The peak gain (as an absolute ratio, NOT dB)
// virtual
double WsfESA_AntennaPattern::GetPeakGain(double aFrequency) const
{
   double gain = ComputeGain(aFrequency, 0.0, 0.0, 0.0, 0.0);

   // Prevent the gain from going below the defined lower bound.
   gain = std::max(mESA_DataPtr->PerformGainAdjustment(aFrequency, gain), mESA_DataPtr->mMinimumGain);
   return gain;
}

//! Initialize the antenna pattern.
//! This is called after all the input for an antenna pattern has been processed.
//! @returns true if successful or false if not.
// virtual
bool WsfESA_AntennaPattern::Initialize(WsfSimulation* aSimulationPtr)
{
   bool ok = WsfAntennaPattern::Initialize(aSimulationPtr);

   if (aSimulationPtr != nullptr)
   {
      if (mESA_DataPtr->mElementPatternTypeId != 0)
      {
         mElementPatternPtr =
            WsfAntennaPatternTypes::Get(aSimulationPtr->GetScenario()).Clone(mESA_DataPtr->mElementPatternTypeId);

         // Shouldn't happen, but just in case
         if (mElementPatternPtr == nullptr)
         {
            auto out = ut::log::error() << "Could not find 'element_pattern'.";
            out.AddNote() << "Type: " << GetType();
            out.AddNote() << "Pattern: " << mESA_DataPtr->mElementPatternTypeId;
            ok = false;
         }
      }

      if (ok)
      {
         AdjustApertureElements(aSimulationPtr->GetRandom());

         if (mESA_DataPtr->mDebug)
         {
            auto out = ut::log::debug() << "Aperture Efficiency:";
            out.AddNote() << "X: " << mApertureEffX;
            out.AddNote() << "Y: " << mApertureEffY;
            out.AddNote() << "Total: " << mApertureEff;
         }
      }
   }
   return ok;
}

// protected virtual
void WsfESA_AntennaPattern::ResizeArray()
{
   mLengthX = mESA_DataPtr->mLengthX;
   mLengthY = mESA_DataPtr->mLengthY;
   mNX      = mESA_DataPtr->mNX;
   mNY      = mESA_DataPtr->mNY;
}


// protected virtual
void WsfESA_AntennaPattern::AdjustApertureElements(ut::Random& aRandom)
{
   ResizeArray();

   mElements.clear();
   mElements.reserve(mNX * mNY);

   AdjustElementLocations();

   // Initialize the X-weighting array
   mWeightVecX.clear();
   mWeightVecX.resize(mNX, 1.0);

   // Initialize the Y-weighting array
   mWeightVecY.clear();
   mWeightVecY.resize(mNY, 1.0);

   // Order is important in the calling of these methods currently
   // TODO - put checks in each to call the ones required.
   ComputeDistributionWeights();
   ComputeFailedModulesWeights(aRandom);
   ComputeApertureEfficiency();
}

// protected virtual
void WsfESA_AntennaPattern::AdjustElementLocations()
{
   double xOffset = 0.0;
   if (mESA_DataPtr->mLatticeType == cLT_TRIANGLULAR)
   {
      xOffset = mESA_DataPtr->mdX / 4.0;
   }

   for (int j = 0; j < mNY; ++j)
   {
      xOffset = -xOffset;
      for (int i = 0; i < mNX; ++i)
      {
         Element element;
         element.mLocation[0] = ((2.0 * (i + 1) - (mNX + 1)) / 2.0) * mESA_DataPtr->mdX + xOffset;
         element.mLocation[1] = ((2.0 * (j + 1) - (mNY + 1)) / 2.0) * mESA_DataPtr->mdY;
         element.mLocation[2] = 0.0;
         mElements.push_back(element);
      }
   }
}

// private
void WsfESA_AntennaPattern::ComputeDistributionWeights()
{
   if (mESA_DataPtr->mDistribution == cWT_Taylor)
   {
      double B;
      double A;
      double sigmaSquare;
      double temp1;
      double temp2;
      double Fm;
      double cosSum;

      ESA_Data::TaylorWeightingData taylorData = mESA_DataPtr->mTaylorData;

      // Get double version for calculations
      double nX = mNX;
      double nY = mNY;

      // Azimuth element weighting
      if (taylorData.mN_BarX > 1)
      {
         B           = pow(10.0, (UtMath::LinearToDB(taylorData.mSidelobeLevelX) / 20.0));
         A           = log(B + sqrt(pow(B, 2.0) - 1)) / UtMath::cPI;
         sigmaSquare = pow((double)taylorData.mN_BarX, 2) / (pow(A, 2) + (pow((taylorData.mN_BarX - 0.5), 2)));

         for (int n = 0; n < mNX; ++n)
         {
            cosSum = 0.0;
            for (int m = 1; m < taylorData.mN_BarX; ++m)
            {
               temp1 = 1.0;
               temp2 = 1.0;
               for (int i = 1; i < taylorData.mN_BarX; ++i)
               {
                  temp1 *= 1.0 - ((pow((double)m, 2.0) / sigmaSquare) / (pow(A, 2.0) + pow((i - 0.5), 2.0)));
                  if (i != m)
                  {
                     temp2 *= 1.0 - (pow((double)m, 2.0) / pow((double)i, 2.0));
                  }
               }
               int FmSign = -1;
               if ((m + 1) % 2 == 0)
               {
                  FmSign = 1;
               }
               Fm = FmSign * temp1 / (2 * temp2);
               cosSum += Fm * cos((2.0 * UtMath::cPI * m * (n - ((nX - 1) / 2.0))) / nX);
            }
            mWeightVecX[n] = 1.0 + (2.0 * cosSum);
         }
      }

      // Elevation element weighting
      if (taylorData.mN_BarY > 1)
      {
         B           = pow(10.0, (UtMath::LinearToDB(taylorData.mSidelobeLevelY) / 20.0));
         A           = log(B + sqrt(pow(B, 2.0) - 1)) / UtMath::cPI;
         sigmaSquare = pow((double)taylorData.mN_BarY, 2) / (pow(A, 2) + (pow((taylorData.mN_BarY - 0.5), 2)));

         for (int n = 0; n < mNY; ++n)
         {
            cosSum = 0.0;
            for (int m = 1; m < taylorData.mN_BarY; ++m)
            {
               temp1 = 1.0;
               temp2 = 1.0;
               for (int i = 1; i < taylorData.mN_BarY; ++i)
               {
                  temp1 *= (1.0 - ((pow((double)m, 2) / sigmaSquare) / (pow(A, 2) + pow((i - 0.5), 2))));
                  if (i != m)
                  {
                     temp2 *= (1.0 - (pow((double)m, 2) / pow((double)i, 2)));
                  }
               }
               int FmSign = -1;
               if ((m + 1) % 2 == 0)
               {
                  FmSign = 1;
               };
               Fm = FmSign * temp1 / (2 * temp2);
               cosSum += (Fm * cos((2.0 * UtMath::cPI * m * (n - ((nY - 1) / 2.0))) / nY));
            }
            mWeightVecY[n] = 1.0 + (2.0 * cosSum);
         }
      }

      // fill the Dist weighting array and normalize
      double maxValue = 0.0;
      for (int i = 0; i < mNX; ++i)
      {
         if (mWeightVecX[i] > maxValue)
         {
            maxValue = mWeightVecX[i];
         }
      }
      for (int i = 0; i < mNX; ++i)
      {
         mWeightVecX[i] /= maxValue;
      }

      maxValue = 0.0;
      for (int i = 0; i < mNY; ++i)
      {
         if (mWeightVecY[i] > maxValue)
         {
            maxValue = mWeightVecY[i];
         }
      }
      for (int i = 0; i < mNY; ++i)
      {
         mWeightVecY[i] /= maxValue;
      }

      int ampNumBits = mESA_DataPtr->mAmpNumBits;

      for (int j = 0; j < mNY; ++j)
      {
         for (int i = 0; i < mNX; ++i)
         {
            mElements[i + (j * mNX)].mWeight *= mWeightVecX[i] * mWeightVecY[j];
            mElements[i + (j * mNX)].mWeight = ComputeQuantizationError(ampNumBits, 1, mElements[i + (j * mNX)].mWeight);
         }
      }

      // Compute the quantization errors on the X and Y weight arrays after getting the mWeightVec populated
      for (int i = 0; i < mNX; ++i)
      {
         mWeightVecX[i] = ComputeQuantizationError(ampNumBits, 1, mWeightVecX[i]);
      }
      for (int j = 0; j < mNY; ++j)
      {
         mWeightVecY[j] = ComputeQuantizationError(ampNumBits, 1, mWeightVecY[j]);
      }
   }
}

// private
double WsfESA_AntennaPattern::ComputeArrayFactor(double aFrequency,
                                                 double aEBS_Az,
                                                 double aEBS_El,
                                                 double aAzAngle,
                                                 double aElAngle) const
{
   double nX = mNX;
   double nY = mNY;
   double dX = mESA_DataPtr->mdX;
   double dY = mESA_DataPtr->mdY;

   double lambda = 2;
   if (aFrequency > 0.0)
   {
      lambda = UtMath::cLIGHT_SPEED / aFrequency;
   }
   else if ((mNX != 1) && (mNY != 1))
   {
      lambda = 2 * ((dX + dY) / 2);
   }
   else if ((mNX != 1) && (mNY == 1))
   {
      lambda = 2 * dX;
   }
   else if ((mNX == 1) && (mNY != 1))
   {
      lambda = 2 * dY;
   }

   double phaseFactor = UtMath::cTWO_PI / lambda;
   double scaleFactor = 1.0 / (nX * nY);

   double  pattern[2];
   UtVec3d elemLocXYZ;

   pattern[0] = 0.0;
   pattern[1] = 0.0;
   for (int i = 0; i < (mNX * mNY); ++i)
   {
      elemLocXYZ       = mElements[i].mLocation;
      double psiXs     = elemLocXYZ[0] * phaseFactor * sin(aEBS_Az) * cos(aEBS_El);
      double psiYs     = elemLocXYZ[1] * phaseFactor * sin(aEBS_El);
      double psiZs     = elemLocXYZ[2] * phaseFactor * cos(aEBS_Az) * cos(aEBS_El);
      double phaseXYZs = psiXs + psiYs + psiZs;
      phaseXYZs        = ComputeQuantizationError(mESA_DataPtr->mPhaseNumBits, UtMath::cTWO_PI, phaseXYZs);

      double psiX     = elemLocXYZ[0] * phaseFactor * sin(aAzAngle) * cos(aElAngle);
      double psiY     = elemLocXYZ[1] * phaseFactor * sin(aElAngle);
      double psiZ     = elemLocXYZ[2] * phaseFactor * cos(aAzAngle) * sin(aElAngle);
      double phaseXYZ = psiX + psiY + psiZ;

      pattern[0] += mElements[i].mWeight * (cos(phaseXYZ - phaseXYZs));
      pattern[1] += mElements[i].mWeight * (sin(phaseXYZ - phaseXYZs));
   }

   double arrayFactor = pow(scaleFactor, 2.0) * (pattern[0] * pattern[0] + pattern[1] * pattern[1]);

   return arrayFactor;
}

// private
void WsfESA_AntennaPattern::ComputeFailedModulesWeights(ut::Random& aRandom)
{
   size_t numfailed = static_cast<size_t>(mNX * mNY * mESA_DataPtr->mFailedRatio);

   std::unordered_set<int> elementList;
   while (elementList.size() < numfailed)
   {
      elementList.insert(aRandom.Uniform(0, mNX * mNY - 1));
   }

   for (int index : elementList)
   {
      mElements[index].mWeight = 0.0;
   }
}

// private
double WsfESA_AntennaPattern::ComputeQuantizationError(int aNumBits, double aRangePerBit, double aValue) const
{
   if (aNumBits <= 0)
   {
      return aValue;
   }

   int    numStates = static_cast<int>(pow(2.0, aNumBits));
   double bitSize   = aRangePerBit / numStates;
   int    numsteps  = static_cast<int>(aValue / bitSize);

   return numsteps * bitSize;
}

// This function leaves out the efficiency term as it is included in the ComputeArrayFactor(..)
// within the weighting array.
// private
double WsfESA_AntennaPattern::ComputeElementGain(double aFrequency, double aAzAngle, double aElAngle) const
{
   double elementGain(1.0);
   if (mElementPatternPtr != nullptr)
   {
      elementGain = 0.0;
      for (const auto& elem : mElements)
      {
         elementGain +=
            mElementPatternPtr->GetGain(aFrequency, aAzAngle - elem.mNormal[0], aElAngle - elem.mNormal[1], 0.0, 0.0);
      }
      elementGain /= (mNX * mNY);
   }
   return elementGain;
}

// This function leaves out the efficiency term as it is included in the ComputeArrayFactor(..)
// within the weighting array.
// private
double WsfESA_AntennaPattern::ComputeElementFactor(double aFrequency,
                                                   double aEBS_Az,
                                                   double aEBS_El,
                                                   double aAzAngle,
                                                   double aElAngle) const
{
   // NOTE: The Element factor is being applied here consists of only the Gain portion according to
   // "Skolnik "Radar Handbook", 2nd ed. Chapter 7, Phased Array Radar Antennas, the steering loss is applied in
   // WsfEM_Antenna::ComputeBeamSteeringLoss outside this class. This allows it to be extended to other
   // antenna pattern classes. An example of how this would be applied is below.
   // double elementFactor = gain * pow(cos(aEBS_Az), mScanExponentX) * pow(cos(aEBS_El), mScanExponentY);

   // Divide out the aperture efficiency as it is included in ComputeDirectivityGain(...) and ComputeArrayFactor(...)
   // so it is not double counted.
   return (ComputeDirectivityGain(aFrequency, aEBS_Az, aEBS_El) *
           (ComputeElementGain(aFrequency, aAzAngle, aElAngle) / mApertureEff));
}

// This method includes the efficiency in retrospect to the ComputeElementFactor(..) method.
// It is expected to be used when NOT being used in conjunction with the ComputeArrayFactor(...) method.
// private
double WsfESA_AntennaPattern::ComputeDirectivityGain(double aFrequency, double aEBS_Az, double aEBS_El) const
{
   // The full equation for directivity gain is as follows:
   // double directivityGain = (4.0 * UtMath::cPI * (mNX * mdX) * (mNY * mdY)) / pow(lambda, 2.0);
   // It is divided up here to account for different conditions and array configurations

   // assume lambda/2 spacing and matched frequency if frequency not provided
   double directivityGain = UtMath::cPI * mNX * mNY;
   if (aFrequency > 0.0)
   {
      double lambda = UtMath::cLIGHT_SPEED / aFrequency;
      if (mNX != 1)
      {
         // multiply in the x direction directivity gain part
         directivityGain *= (2.0 * mESA_DataPtr->mdX / lambda);
      }

      if (mNY != 1)
      {
         // multiply in the x direction directivity gain part
         directivityGain *= (2.0 * mESA_DataPtr->mdY / lambda);
      }
   }

   // NOTE: The Array gain is being applied here consists of only the Gain portion according to
   // "Skolnik "Radar Handbook", 2nd ed. Chapter 7, Phased Array Radar Antennas, the steering loss is applied in
   // WsfEM_Antenna::ComputeBeamSteeringLoss outside this class. This allows it to be extended to other
   // antenna pattern classes. An example of how this would be applied is below.
   // directivityGain *= pow(cos(aEBS_Az), loss_exponent) * pow(cos(aEBS_El), loss_exponent);

   return directivityGain * mApertureEff;
}

// private
double WsfESA_AntennaPattern::ComputeApertureEfficiency()
{
   // Get double version for calculations
   double nX = mNX;
   double nY = mNY;

   // Efficiency
   double AmnX  = 0.0;
   double AmnSX = 0.0;
   for (int n = 0; n < mNX; ++n)
   {
      AmnX += sqrt(mWeightVecX[n] * mWeightVecX[n]);
      AmnSX += mWeightVecX[n] * mWeightVecX[n];
   }
   mApertureEffX = pow(AmnX, 2.0) / (nX * AmnSX);

   // Efficiency
   double AmnY  = 0.0;
   double AmnSY = 0.0;
   for (int m = 0; m < mNY; ++m)
   {
      AmnY += sqrt(mWeightVecY[m] * mWeightVecY[m]);
      AmnSY += mWeightVecY[m] * mWeightVecY[m];
   }
   mApertureEffY = pow(AmnY, 2.0) / (nY * AmnSY);

   // Efficiency
   double Amn  = 0.0;
   double AmnS = 0.0;
   for (int j = 0; j < mNY; ++j)
   {
      for (int i = 0; i < mNX; ++i)
      {
         Amn += sqrt(mElements[i + (j * mNX)].mWeight * mElements[i + (j * mNX)].mWeight);
         AmnS += mElements[i + (j * mNX)].mWeight * mElements[i + (j * mNX)].mWeight;
      }
   }
   mApertureEff = pow(Amn, 2.0) / (nX * nY * AmnS);

   return mApertureEff;
}

// =================================================================================================
// Nested class 'ESA_Data'.
// =================================================================================================
WsfESA_AntennaPattern::ESA_Data::ESA_Data()
   : BaseData()
   , mdX(0.0)
   , mdY(0.0)
   , mLengthX(0.0)
   , mLengthY(0.0)
   , mNX(0)
   , mNY(0)
   , mBackBaffled(true)
   , mFailedRatio(0.0)
   , mDistribution(cWT_None)
   , mScanExponentX(0.0)
   , mScanExponentY(0.0)
   , mPhaseNumBits(0)
   , mAmpNumBits(0)
   , mLatticeType(cLT_RECTANGLULAR)
   , mElementPatternTypeId(nullptr)
   , mDebug(false)
{
   mTaylorData.Clear();
}

// virtual
bool WsfESA_AntennaPattern::ESA_Data::ProcessInput(WsfAntennaPattern& aPattern, UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "element_spacing_x")
   {
      aInput.ReadValueOfType(mdX, UtInput::cLENGTH);
      aInput.ValueGreater(mdX, 0.0);
   }
   else if (command == "element_spacing_y")
   {
      aInput.ReadValueOfType(mdY, UtInput::cLENGTH);
      aInput.ValueGreater(mdY, 0.0);
   }
   else if (command == "length_x")
   {
      aInput.ReadValueOfType(mLengthX, UtInput::cLENGTH);
      aInput.ValueGreater(mLengthX, 0.0);
   }
   else if (command == "length_y")
   {
      aInput.ReadValueOfType(mLengthY, UtInput::cLENGTH);
      aInput.ValueGreater(mLengthY, 0.0);
   }
   else if (command == "number_elements_x")
   {
      aInput.ReadValue(mNX);
      aInput.ValueGreater(mNX, 0);
   }
   else if (command == "number_elements_y")
   {
      aInput.ReadValue(mNY);
      aInput.ValueGreater(mNY, 0);
   }
   else if (command == "exponent_x")
   {
      aInput.ReadValue(mScanExponentX);
      aInput.ValueInClosedRange(mScanExponentX, 1.0, 4.0);
   }
   else if (command == "exponent_y")
   {
      aInput.ReadValue(mScanExponentY);
      aInput.ValueInClosedRange(mScanExponentY, 1.0, 4.0);
   }
   else if (command == "distribution_type")
   {
      std::string weightingType;
      aInput.ReadValue(weightingType);
      if (weightingType == "taylor")
      {
         ProcessTaylorWeightingInput(aInput);
         mDistribution = cWT_Taylor;
      }
      else if (weightingType == "uniform")
      {
         mDistribution = cWT_Uniform;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Distribution type not recognized.");
      }
   }
   else if (command == "amplitude_quantization_bits")
   {
      aInput.ReadValue(mAmpNumBits);
      aInput.ValueGreater(mAmpNumBits, 0);
   }
   else if (command == "phase_quantization_bits")
   {
      aInput.ReadValue(mPhaseNumBits);
      aInput.ValueGreater(mPhaseNumBits, 0);
   }
   else if (command == "failed_elements_ratio")
   {
      aInput.ReadValue(mFailedRatio);
      aInput.ValueInClosedRange(mFailedRatio, 0.0, 1.0);
   }
   else if (command == "lattice")
   {
      std::string latticeType;
      aInput.ReadValue(latticeType);
      if (latticeType == "rectangular")
      {
         mLatticeType = cLT_RECTANGLULAR;
      }
      else if (latticeType == "triangular")
      {
         mLatticeType = cLT_TRIANGLULAR;
      }
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (command == "element_pattern")
   {
      std::string patternType;
      aInput.ReadValue(patternType);
      WsfAntennaPattern* elementPatternPtr = WsfScenario::FromInput(aInput).FindTypeT<WsfAntennaPattern>(patternType);
      if (elementPatternPtr == nullptr)
      {
         throw UtInput::BadValue(aInput);
      }
      mElementPatternTypeId = elementPatternPtr->GetTypeId();
   }
   else if (command == "back_baffled")
   {
      aInput.ReadValue(mBackBaffled);
   }
   else
   {
      myCommand = WsfAntennaPattern::BaseData::ProcessInput(aPattern, aInput);
   }
   return myCommand;
}

// virtual
bool WsfESA_AntennaPattern::ESA_Data::Initialize(WsfAntennaPattern& aAntennaPattern)
{
   // calculate rest of X-Axis data from input.
   if (mNX == 1)
   {
      mLengthX = 0.0;
      mdX      = 0.0;
   }
   else if (mdX != 0)
   {
      if (mLengthX > 0.0)
      {
         // X-Axis spacing and length is given
         mNX = static_cast<int>(mLengthX / mdX);
         if (mDebug)
         {
            auto out = ut::log::debug() << "Calculated 'number_elements_x' for antenna pattern.";
            out.AddNote() << "Number Elements: " << mNX;
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
         }
      }
      else
      {
         // X-Axis spacing and number of elements are given
         if (mNX != 0)
         {
            mLengthX = mNX * mdX;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Calculated 'length_x' for antenna pattern.";
               out.AddNote() << "Length: " << mLengthX << " m";
               out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            }
         }
         else
         {
            auto out = ut::log::error() << "Number of X-Axis elements is 0.";
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            return false;
         }
      }
   }
   else
   {
      if (mNX != 0)
      {
         // X-Axis length and number of elements are given
         if (mLengthX > 0.0)
         {
            mdX = mLengthX / mNX;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Calculated 'element_spacing_x' for antenna pattern.";
               out.AddNote() << "Element Spacing: " << mdX << " m";
               out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            }
         }
         else
         {
            auto out = ut::log::error() << "Either the Element Spacing or the Length are not defined.";
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            return false;
         }
      }
   }

   // calculate rest of Y-Axis data from input.
   if (mNY == 1)
   {
      mLengthY = 0.0;
      mdY      = 0.0;
   }
   else if (mdY != 0)
   {
      if (mLengthY > 0.0)
      {
         // Y-Axis spacing and length is given
         mNY = static_cast<int>(mLengthY / mdY);
         if (mDebug)
         {
            auto out = ut::log::debug() << "Calculated 'number_elements_y' for antenna pattern.";
            out.AddNote() << "Number Elements: " << mNY;
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
         }
      }
      else
      {
         // Y-Axis spacing and number of elements are given
         if (mNY != 0)
         {
            mLengthY = mNY * mdY;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Calculated 'length_y' for antenna pattern.";
               out.AddNote() << "Length: " << mLengthY << " m";
               out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            }
         }
         else
         {
            auto out = ut::log::error() << "Number of Y-Axis elements is 0.";
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            return false;
         }
      }
   }
   else
   {
      if (mNY != 0)
      {
         // Y-Axis length and number of elements are given
         if (mLengthY > 0.0)
         {
            mdY = mLengthY / mNY;
            if (mDebug)
            {
               auto out = ut::log::debug() << "Calculated 'element_spacing_y' for antenna pattern.";
               out.AddNote() << "Element Spacing: " << mdY << " m";
               out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            }
         }
         else
         {
            auto out = ut::log::error() << "Either the 'element_spacing_y' or the 'length_y' not defined.";
            out.AddNote() << "Antenna Pattern: " << aAntennaPattern.GetType();
            return false;
         }
      }
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "ESA Pattern: " << aAntennaPattern.GetType() << " data:";
      out.AddNote() << "Element Spacing (x,y): " << mdX << ", " << mdY << " m";
      out.AddNote() << "Number of elements (x,y): " << mNX << ", " << mNY;
      out.AddNote() << "Antenna Length (x,y):  " << mLengthX << ", " << mLengthY << " m";
   }

   return BaseData::Initialize(aAntennaPattern);
}


// private
void WsfESA_AntennaPattern::ESA_Data::ProcessTaylorWeightingInput(UtInput& aInput)
{
   std::string  command;
   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand(command))
   {
      if (command == "sidelobe_level_x")
      {
         aInput.ReadValueOfType(mTaylorData.mSidelobeLevelX, UtInput::cRATIO);
         aInput.ValueInClosedRange(mTaylorData.mSidelobeLevelX, UtMath::DB_ToLinear(15.0), UtMath::DB_ToLinear(55.0));
      }
      else if (command == "sidelobe_level_y")
      {
         aInput.ReadValueOfType(mTaylorData.mSidelobeLevelY, UtInput::cRATIO);
         aInput.ValueInClosedRange(mTaylorData.mSidelobeLevelY, UtMath::DB_ToLinear(15.0), UtMath::DB_ToLinear(55.0));
      }
      else if (command == "n_bar_x")
      {
         aInput.ReadValue(mTaylorData.mN_BarX);
      }
      else if (command == "n_bar_y")
      {
         aInput.ReadValue(mTaylorData.mN_BarY);
      }
      else // don't require additional block closure
      {
         inputBlock.TerminateBlock();
         aInput.PushBack(command);
         break;
      }
   }
}
