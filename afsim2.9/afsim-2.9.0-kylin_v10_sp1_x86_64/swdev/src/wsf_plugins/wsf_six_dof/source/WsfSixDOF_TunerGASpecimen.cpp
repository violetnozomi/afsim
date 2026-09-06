// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_TunerGASpecimen.hpp"

#include <algorithm>
#include <random>
#include <string>

#include "UtMath.hpp"

void wsf::six_dof::TunerGASpecimen::GenerateSpecimen()
{
   if (mRandomPtr)
   {
      if (UtMath::NearlyEqual(mLogKpRange[0], mLogKpRange[1]))
      {
         mLogKp = mLogKpRange[0];
      }
      else
      {
         mLogKp = mRandomPtr->Uniform(mLogKpRange[0], mLogKpRange[1]);
      }

      if (UtMath::NearlyEqual(mLogKiRange[0], mLogKiRange[1]))
      {
         mLogKi = mLogKiRange[0];
      }
      else
      {
         mLogKi = mRandomPtr->Uniform(mLogKiRange[0], mLogKiRange[1]);
      }

      if (UtMath::NearlyEqual(mLogKdRange[0], mLogKdRange[1]))
      {
         mLogKd = mLogKdRange[0];
      }
      else
      {
         mLogKd = mRandomPtr->Uniform(mLogKdRange[0], mLogKdRange[1]);
      }

      if (UtMath::NearlyEqual(mLowPassAlphaRange[0], mLowPassAlphaRange[1]))
      {
         mLowPassAlpha = mLowPassAlphaRange[0];
      }
      else
      {
         mLowPassAlpha = mRandomPtr->Uniform(mLowPassAlphaRange[0], mLowPassAlphaRange[1]);
      }
   }

   DetermineChromosomeLengths();

   GenerateChromosome();
}

double wsf::six_dof::TunerGASpecimen::AssessObjective(std::vector<PidLogElement>& aSimData)
{
   if (aSimData.empty())
   {
      mObjectiveValue = 0;
      return 0;
   }

   double maxOvershoot = 0.0;
   double sqErrorSum   = 0.0;
   double absRateSum   = 0.0;

   // Use [1] for the start rather than .front() or [0]
   // .front() contains cached data and will provide incorrect results
   double initialTime  = aSimData[1].timeStamp;
   double finalTime    = aSimData.back().timeStamp;
   double timeSpan     = finalTime - initialTime;
   double initialState = aSimData[1].pidValues.CurrentValue;
   double initialError = aSimData[1].pidValues.SetPoint - initialState;

   // Riemann sum to determine total error

   for (size_t i = 1; i < aSimData.size(); ++i)
   {
      double instantaneousError           = (aSimData[i].pidValues.SetPoint - aSimData[i].pidValues.CurrentValue);
      double instantaneousErrorNormalized = instantaneousError / std::abs(initialError);

      double setPointDeltaSign = UtMath::Sign(aSimData[i].pidValues.SetPoint - initialState);
      double tempOvershoot     = instantaneousErrorNormalized * setPointDeltaSign;

      double deltaTime = aSimData[i].timeStamp - aSimData[i - 1].timeStamp;
      sqErrorSum += pow(instantaneousErrorNormalized, 2) * deltaTime;

      if (i > 1)
      {
         double instantaneousRate = (aSimData[i].pidValues.CurrentValue - aSimData[i - 1].pidValues.CurrentValue) /
                                    std::abs(initialError); // divide by deltaTime
         absRateSum += std::abs(instantaneousRate);         // multiply by deltaTime
      }

      if (tempOvershoot > maxOvershoot)
      {
         maxOvershoot = tempOvershoot;
      }
   }

   double rmsError         = sqrt(sqErrorSum / timeSpan);
   double meanAbsoluteRate = absRateSum / timeSpan;

   double cost     = mErrorWeight * rmsError + mOvershootWeight * maxOvershoot + meanAbsoluteRate;
   mObjectiveValue = 1.0 / (1.0 + cost);

   return mObjectiveValue;
}

void wsf::six_dof::TunerGASpecimen::GenerateChromosome()
{
   double logKpRange        = mLogKpRange[1] - mLogKpRange[0];
   double logKiRange        = mLogKiRange[1] - mLogKiRange[0];
   double logKdRange        = mLogKdRange[1] - mLogKdRange[0];
   double lowPassAlphaRange = mLowPassAlphaRange[1] - mLowPassAlphaRange[0];

   if (!UtMath::NearlyZero(logKpRange))
   {
      mChromosome =
         IntToBinString(static_cast<int>((mLogKp - mLogKpRange[0]) * (pow(2, mChromosomeLengths[0]) - 1) / logKpRange),
                        mChromosomeLengths[0]);
   }
   else
   {
      mChromosome = "";
   }

   if (!UtMath::NearlyZero(logKiRange))
   {
      mChromosome +=
         IntToBinString(static_cast<int>((mLogKi - mLogKiRange[0]) * (pow(2, mChromosomeLengths[1]) - 1) / logKiRange),
                        mChromosomeLengths[1]);
   }

   if (!UtMath::NearlyZero(logKdRange))
   {
      mChromosome +=
         IntToBinString(static_cast<int>((mLogKd - mLogKdRange[0]) * (pow(2, mChromosomeLengths[2]) - 1) / logKdRange),
                        mChromosomeLengths[2]);
   }

   if (!UtMath::NearlyZero(lowPassAlphaRange))
   {
      mChromosome += IntToBinString(static_cast<int>((mLowPassAlpha - mLowPassAlphaRange[0]) *
                                                     (pow(2, mChromosomeLengths[2]) - 1) / lowPassAlphaRange),
                                    mChromosomeLengths[2]);
   }
}

void wsf::six_dof::TunerGASpecimen::Mutate(double aMutationProbablilty)
{
   if (mRandomPtr)
   {
      for (auto& i : mChromosome)
      {
         if (mRandomPtr->Uniform(0.0, 1.0) <= aMutationProbablilty)
         {
            if (i == '1')
            {
               i = '0';
            }
            else
            {
               i = '1';
            }
         }
      }
      UpdateBasedOnChromosome();
   }
}

void wsf::six_dof::TunerGASpecimen::CrossOver(const TunerGASpecimen& aSpecimen)
{
   if (mRandomPtr)
   {
      std::string aChrom = aSpecimen.GetChromosome();
      for (size_t crossoverLocation = mRandomPtr->Uniform<size_t>(0, mChromosome.size());
           crossoverLocation < mChromosome.size();
           ++crossoverLocation)
      {
         mChromosome[crossoverLocation] = aChrom[crossoverLocation];
      }
      UpdateBasedOnChromosome();
   }
}

void wsf::six_dof::TunerGASpecimen::SetKpRange(double aLowerBound, double aUpperBound)
{
   constexpr double safeMinimumGain = std::numeric_limits<float>::epsilon();

   mLogKpRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKpRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void wsf::six_dof::TunerGASpecimen::SetKiRange(double aLowerBound, double aUpperBound)
{
   constexpr double safeMinimumGain = std::numeric_limits<float>::epsilon();

   mLogKiRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKiRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void wsf::six_dof::TunerGASpecimen::SetKdRange(double aLowerBound, double aUpperBound)
{
   constexpr double safeMinimumGain = std::numeric_limits<float>::epsilon();

   mLogKdRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKdRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void wsf::six_dof::TunerGASpecimen::SetRandomPtr(ut::Random* aRandomPtr)
{
   mRandomPtr = aRandomPtr;
}

void wsf::six_dof::TunerGASpecimen::DetermineChromosomeLengths()
{
   size_t i      = 0;
   double value1 = (mLogKpRange[1] - mLogKpRange[0]) * 10000;
   double value2 = (mLogKiRange[1] - mLogKiRange[0]) * 10000;
   double value3 = (mLogKdRange[1] - mLogKdRange[0]) * 10000;
   double value4 = (mLowPassAlphaRange[1] - mLowPassAlphaRange[0]) * 10000;
   bool   size1  = UtMath::NearlyZero(value1);
   bool   size2  = UtMath::NearlyZero(value2);
   bool   size3  = UtMath::NearlyZero(value3);
   bool   size4  = UtMath::NearlyZero(value4);
   while (true)
   {
      ++i;
      if (value1 > std::pow(2, i - 1) && value1 <= std::pow(2, i) - 1 && !size1)
      {
         mChromosomeLengths[0] = i;
         size1                 = true;
      }

      if (value2 > std::pow(2, i - 1) && value2 <= std::pow(2, i) - 1 && !size2)
      {
         mChromosomeLengths[1] = i;
         size2                 = true;
      }

      if (value3 > std::pow(2, i - 1) && value3 <= std::pow(2, i) - 1 && !size3)
      {
         mChromosomeLengths[2] = i;
         size3                 = true;
      }

      if (value4 > std::pow(2, i - 1) && value4 <= std::pow(2, i) - 1 && !size4)
      {
         mChromosomeLengths[3] = i;
         size4                 = true;
      }

      if (size1 && size2 && size3 && size4)
      {
         return;
      }
   }
}

std::string wsf::six_dof::TunerGASpecimen::IntToBinString(size_t aInt, size_t aLength)
{
   std::string s;
   for (; aInt > 0; aInt /= 2)
   {
      s = std::to_string(aInt % 2) + s;
   }

   for (size_t i = s.length(); i < aLength; ++i)
   {
      s = "0" + s;
   }

   return s;
}

unsigned int wsf::six_dof::TunerGASpecimen::BinStringToInt(const std::string& aBinString)
{
   unsigned int n = 0;
   for (size_t i = 0; i < aBinString.length(); ++i)
   {
      if (aBinString[aBinString.length() - (i + 1)] == '1')
      {
         n += static_cast<unsigned int>(std::pow(2, i));
      }
   }
   return n;
}

void wsf::six_dof::TunerGASpecimen::UpdateBasedOnChromosome()
{
   size_t chromosomePosition = 0;

   std::string logKp = mChromosome.substr(0, mChromosomeLengths[0]);
   chromosomePosition += mChromosomeLengths[0];

   std::string logKi = mChromosome.substr(chromosomePosition, mChromosomeLengths[1]);
   chromosomePosition += mChromosomeLengths[1];

   std::string logKd = mChromosome.substr(chromosomePosition, mChromosomeLengths[2]);
   chromosomePosition += mChromosomeLengths[2];

   std::string lowPassAlpha = mChromosome.substr(chromosomePosition, mChromosomeLengths[3]);

   double logKpDomain        = pow(2, mChromosomeLengths[0]) - 1;
   double logKiDomain        = pow(2, mChromosomeLengths[1]) - 1;
   double logKdDomain        = pow(2, mChromosomeLengths[2]) - 1;
   double lowPassAlphaDomain = pow(2, mChromosomeLengths[3]) - 1;

   if (UtMath::NearlyZero(logKpDomain))
   {
      mLogKp = mLogKpRange[0];
   }
   else
   {
      mLogKp = BinStringToInt(logKp) * (mLogKpRange[1] - mLogKpRange[0]) / logKpDomain + mLogKpRange[0];
   }

   if (UtMath::NearlyZero(logKiDomain))
   {
      mLogKi = mLogKiRange[0];
   }
   else
   {
      mLogKi = BinStringToInt(logKi) * (mLogKiRange[1] - mLogKiRange[0]) / logKiDomain + mLogKiRange[0];
   }

   if (UtMath::NearlyZero(logKdDomain))
   {
      mLogKd = mLogKdRange[0];
   }
   else
   {
      mLogKd = BinStringToInt(logKd) * (mLogKdRange[1] - mLogKdRange[0]) / logKdDomain + mLogKdRange[0];
   }

   if (UtMath::NearlyZero(lowPassAlphaDomain))
   {
      mLowPassAlpha = mLowPassAlphaRange[0];
   }
   else
   {
      mLowPassAlpha = BinStringToInt(lowPassAlpha) * (mLowPassAlphaRange[1] - mLowPassAlphaRange[0]) / lowPassAlphaDomain +
                      mLowPassAlphaRange[0];
   }

   mObjectiveValue = -1.0;
}
