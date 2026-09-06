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

#include "WsfSixDOF_TunerGAManager.hpp"

void wsf::six_dof::TunerGAManager::Advance(std::vector<TunerGASpecimen::PidLogElement>& aSimData)
{
   if (mCompleted)
   {
      return;
   }

   // assess the objective for each specimen in the population
   if (mPopulation[mCurrentSpecimen].GetObjectiveValue() < 0)
   {
      mPopulation[mCurrentSpecimen].AssessObjective(aSimData);
   }

   // if there is a new best store it
   if (mPopulation[mCurrentSpecimen].GetObjectiveValue() > mOptimum.GetObjectiveValue())
   {
      mOptimum = mPopulation[mCurrentSpecimen];
   }

   // advance to next specimen
   ++mCurrentSpecimen;

   // if I've already visited the whole population move on to the next generation
   if (mCurrentSpecimen == mPopulationSize)
   {
      SelectNextPopulation();
      mCurrentSpecimen = 0;
      ++mCurrentGeneration;

      if (mCurrentGeneration == mMaxGenerations)
      {
         mCompleted = true;
         return;
      }
      else
      {
         for (auto& j : mPopulation)
         {
            j.Mutate(mMutationProbability);
         }

         for (auto& j : mPopulation)
         {
            if (mRandom.Uniform(0, 100) >= 100 * mCrossOverProbability)
            {
               // Pick two candidates to cross with j, and take the fitter of the two
               size_t crossoverWest = mRandom.Uniform<size_t>(0, mPopulationSize - 1);
               size_t crossoverEast = mRandom.Uniform<size_t>(0, mPopulationSize - 1);

               size_t crossoverSpec = 0;
               if (mPopulation[crossoverWest].GetObjectiveValue() > mPopulation[crossoverEast].GetObjectiveValue())
               {
                  crossoverSpec = crossoverWest;
               }
               else
               {
                  crossoverSpec = crossoverEast;
               }

               TunerGASpecimen copy = mPopulation[crossoverSpec];
               mPopulation[crossoverSpec].CrossOver(j);
               j.CrossOver(copy);
            }
         }
      }
   }
}

void wsf::six_dof::TunerGAManager::SetCarryOverFraction(double aCarryOverFrac)
{
   // clamp mCarryOverFraction to a valid value between 0 and 1;
   if (aCarryOverFrac > 1.0)
   {
      mCarryOverFraction = 1.0;
   }
   else if (aCarryOverFrac < 0.0)
   {
      mCarryOverFraction = 0.0;
   }
   else
   {
      mCarryOverFraction = aCarryOverFrac;
   }
}

void wsf::six_dof::TunerGAManager::SetMutationProbability(double aMutProb)
{
   if (aMutProb > 1.0)
   {
      mMutationProbability = 1.0;
   }
   else if (aMutProb < 0.0)
   {
      mMutationProbability = 0.0;
   }
   else
   {
      mMutationProbability = aMutProb;
   }
}

void wsf::six_dof::TunerGAManager::SetCrossOverProbability(double aCrossOverProb)
{
   if (aCrossOverProb > 1.0)
   {
      mCrossOverProbability = 1.0;
   }
   else if (aCrossOverProb < 0.0)
   {
      mCrossOverProbability = 0.0;
   }
   else
   {
      mCrossOverProbability = aCrossOverProb;
   }
}

void wsf::six_dof::TunerGAManager::InitializePopulation(const TunerGASpecimen& aSpecimen)
{
   if (!mPopulation.empty())
   {
      mPopulation.clear();
   }

   // Clear the RNG and initialize it to a static seed
   mRandom = ut::Random();
   mRandom.SetSeed(0xABC123);

   mParentSpecimen = aSpecimen;
   mParentSpecimen.SetRandomPtr(&mRandom);
   for (size_t i = 0; i < mPopulationSize; ++i)
   {
      mPopulation.push_back(mParentSpecimen);
      mPopulation[i].GenerateSpecimen();
   }

   mCurrentSpecimen   = 0;
   mCurrentGeneration = 0;
   mCompleted         = false;
   mOptimum           = mPopulation[0];
}

void wsf::six_dof::TunerGAManager::SelectNextPopulation()
{
   std::vector<TunerGASpecimen> newPopulation;

   std::vector<double> fitnessSlots;
   double              sum = 0;

   // fill a vector that contains the fitness values for all specimens
   // (specimens is actually the proper plural form, even though it sounds weird)
   for (const auto& i : mPopulation)
   {
      sum += i.GetObjectiveValue();
      fitnessSlots.push_back(i.GetObjectiveValue());
   }

   // normalize fitnessSlots vector and make it increase monotonically
   // difference between two adjacent slots will create a "slot" in which a
   // ramdomly generated number may fall. Fitter candidates will have larger
   // "slots" making them more liekly to be selected to move to the next generation.
   for (size_t i = 0; i < mPopulationSize; ++i)
   {
      fitnessSlots[i] /= sum;
      if (i != 0)
      {
         fitnessSlots[i] += fitnessSlots[i - 1];
      }
   }

   // determine number of specimens to carry to next generation
   auto carryOverCount = static_cast<size_t>(mPopulationSize * mCarryOverFraction);

   // grab specimens for the next generation
   for (size_t i = 1; i <= carryOverCount; ++i)
   {
      double randomNum        = mRandom.Uniform(0.0, 1.0);
      size_t selectedSpecimen = 1;

      for (size_t j = 1; j < mPopulationSize; ++j)
      {
         if (randomNum > fitnessSlots[j - 1] && randomNum <= fitnessSlots[j])
         {
            selectedSpecimen = j;
            break;
         }
      }
      newPopulation.push_back(mPopulation[selectedSpecimen]);
   }

   // fill remaining portion of population with new specimens
   for (size_t i = carryOverCount; i < mPopulationSize; ++i)
   {
      newPopulation.push_back(mParentSpecimen);
      newPopulation[i].GenerateSpecimen();
   }

   mPopulation = newPopulation;
}
