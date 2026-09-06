// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "P6DofTunerGASpecimen.hpp"

#include <algorithm>
#include <random>
#include <string>

#include "UtMath.hpp"

const double P6DofTunerGASpecimen::cDefaultLogKpRange[2] = {-9, 2};
const double P6DofTunerGASpecimen::cDefaultLogKiRange[2] = {-9, 2};
const double P6DofTunerGASpecimen::cDefaultLogKdRange[2] = {-9, 2};

P6DofTunerGASpecimen::P6DofTunerGASpecimen()
   : mObjectiveValue(-1)
   , mLogKp(0.0)
   , mLogKi(0.0)
   , mLogKd(0.0)
   , mErrorWeight(1.0)
   , mOvershootWeight(1.0)
{
   // TODO this is temporary, move from hard code to pulling from preferences, possibly use a mutator?
   mLogKpRange[0] = cDefaultLogKpRange[0];
   mLogKpRange[1] = cDefaultLogKpRange[1];

   mLogKiRange[0] = cDefaultLogKiRange[0];
   mLogKiRange[1] = cDefaultLogKiRange[1];

   mLogKdRange[0] = cDefaultLogKdRange[0];
   mLogKdRange[1] = cDefaultLogKdRange[1];

   mChromosomeLengths[0] = 0;
   mChromosomeLengths[1] = 0;
   mChromosomeLengths[2] = 0;
}

void P6DofTunerGASpecimen::GenerateSpecimen()
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
   }

   DetermineChromosomeLengths();

   GenerateChromosome();
}

double P6DofTunerGASpecimen::AssessObjective(std::vector<PidLogElement>& aSimData)
{
   if (aSimData.empty())
   {
      mObjectiveValue = 0;
      return 0;
   }

   double maxOvershoot = 0.0;
   double absErrorSum  = 0.0;
   double absRateSum   = 0.0;

   // Use [1] for the start rather than .front() or [0]
   // .front() contains cached data and will provide incorrect results
   double initialTime  = aSimData[1].timeStamp;
   double finalTime    = aSimData.back().timeStamp;
   double timeSpan     = finalTime - initialTime;
   double initialState = aSimData[1].pidValues.CurrentValue;
   double initialError = initialState - aSimData[1].pidValues.SetPoint;

   // Riemann sum to determine total error
   for (size_t i = 1; i < aSimData.size(); ++i)
   {
      double instantaneousError =
         (aSimData[i].pidValues.CurrentValue - aSimData[i].pidValues.SetPoint) / std::abs(initialError);

      double setPointDeltaSign = UtMath::Sign(aSimData[i].pidValues.SetPoint - initialState);
      double tempOvershoot     = instantaneousError * setPointDeltaSign;

      double deltaTime = aSimData[i].timeStamp - aSimData[i - 1].timeStamp;
      absErrorSum += std::abs(instantaneousError) * deltaTime;

      double instantaneousRate = (aSimData[i].pidValues.CurrentValue - aSimData[i - 1].pidValues.CurrentValue) /
                                 std::abs(initialError); // / deltaTime
      absRateSum += std::abs(instantaneousRate);         // * deltaTime

      if (tempOvershoot > maxOvershoot)
      {
         maxOvershoot = tempOvershoot;
      }
   }

   double meanAbsoluteError = absErrorSum / timeSpan;
   double meanAbsoluteRate  = absRateSum / timeSpan;

   double cost = mErrorWeight * meanAbsoluteError + mOvershootWeight * maxOvershoot;

   // Impose a small penalty for gain values to penalize flailing
   cost += 0.001 * (GetKp() + GetKi() + GetKd());

   // Impose a penalty for rate to penalize buzzy response
   cost += 1.0 * meanAbsoluteRate;

   mObjectiveValue = 1.0 / (1.0 + cost);

   return mObjectiveValue;
}

void P6DofTunerGASpecimen::GenerateChromosome()
{
   double logKpRange = mLogKpRange[1] - mLogKpRange[0];
   double logKiRange = mLogKiRange[1] - mLogKiRange[0];
   double logKdRange = mLogKdRange[1] - mLogKdRange[0];

   if (!UtMath::NearlyZero(logKpRange))
   {
      mChromosome = IntToBinString((int)((mLogKp - mLogKpRange[0]) * (pow(2, mChromosomeLengths[0]) - 1) / logKpRange),
                                   mChromosomeLengths[0]);
   }
   else
   {
      mChromosome = "";
   }

   if (!UtMath::NearlyZero(logKiRange))
   {
      mChromosome += IntToBinString((int)((mLogKi - mLogKiRange[0]) * (pow(2, mChromosomeLengths[1]) - 1) / logKiRange),
                                    mChromosomeLengths[1]);
   }

   if (!UtMath::NearlyZero(logKdRange))
   {
      mChromosome += IntToBinString((int)((mLogKd - mLogKdRange[0]) * (pow(2, mChromosomeLengths[2]) - 1) / logKdRange),
                                    mChromosomeLengths[2]);
   }
}

void P6DofTunerGASpecimen::Mutate(double aMutationProbablilty)
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

void P6DofTunerGASpecimen::CrossOver(P6DofTunerGASpecimen& aSpecimen)
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

void P6DofTunerGASpecimen::SetKpRange(double aLowerBound, double aUpperBound)
{
   const double safeMinimumGain = std::pow(10.0, cDefaultLogKpRange[0]);

   mLogKpRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKpRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void P6DofTunerGASpecimen::SetKiRange(double aLowerBound, double aUpperBound)
{
   const double safeMinimumGain = std::pow(10.0, cDefaultLogKiRange[0]);

   mLogKiRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKiRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void P6DofTunerGASpecimen::SetKdRange(double aLowerBound, double aUpperBound)
{
   const double safeMinimumGain = std::pow(10.0, cDefaultLogKdRange[0]);

   mLogKdRange[0] = std::log10(std::max(safeMinimumGain, aLowerBound));
   mLogKdRange[1] = std::log10(std::max(safeMinimumGain, aUpperBound));
}

void P6DofTunerGASpecimen::SetRandomPtr(ut::Random* aRandomPtr)
{
   mRandomPtr = aRandomPtr;
}

void P6DofTunerGASpecimen::DetermineChromosomeLengths()
{
   size_t i      = 0;
   double value1 = (mLogKpRange[1] - mLogKpRange[0]) * 10000;
   double value2 = (mLogKiRange[1] - mLogKiRange[0]) * 10000;
   double value3 = (mLogKdRange[1] - mLogKdRange[0]) * 10000;
   bool   size1  = UtMath::NearlyZero(value1);
   bool   size2  = UtMath::NearlyZero(value2);
   bool   size3  = UtMath::NearlyZero(value3);
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

      if (size1 && size2 && size3)
      {
         return;
      }
   }
}

std::string P6DofTunerGASpecimen::IntToBinString(size_t aInt, size_t aLength)
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

unsigned int P6DofTunerGASpecimen::BinStringToInt(const std::string& aBinString)
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

void P6DofTunerGASpecimen::UpdateBasedOnChromosome()
{
   size_t chromosomePosition = 0;

   std::string logKp = mChromosome.substr(0, mChromosomeLengths[0]);
   chromosomePosition += mChromosomeLengths[0];

   std::string logKi = mChromosome.substr(chromosomePosition, mChromosomeLengths[1]);
   chromosomePosition += mChromosomeLengths[1];

   std::string logKd = mChromosome.substr(chromosomePosition, mChromosomeLengths[2]);

   double logKpDomain = pow(2, mChromosomeLengths[0]) - 1;
   double logKiDomain = pow(2, mChromosomeLengths[1]) - 1;
   double logKdDomain = pow(2, mChromosomeLengths[2]) - 1;

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

   mObjectiveValue = -1.0;
}
