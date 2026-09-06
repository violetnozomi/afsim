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

#ifndef WSFSIXDOFTUNERGASPECIMEN_HPP
#define WSFSIXDOFTUNERGASPECIMEN_HPP

#include "wsf_six_dof_export.h"

#include <vector>

#include "UtRandom.hpp"
#include "WsfSixDOF_VehicleData.hpp"

namespace wsf
{
namespace six_dof
{
class WSF_SIX_DOF_EXPORT TunerGASpecimen
{
public:
   struct PidLogElement
   {
      SinglePidValueData pidValues;
      double             timeStamp = 0.0;
   };

   TunerGASpecimen() = default;

   void        GenerateSpecimen();
   double      AssessObjective(std::vector<PidLogElement>& aSimData);
   double      GetObjectiveValue() const { return mObjectiveValue; }
   void        GenerateChromosome();
   std::string GetChromosome() const { return mChromosome; }
   void        Mutate(double aMutationProbability);
   void        CrossOver(const TunerGASpecimen& aSpecimen);

   // Set weight factor for error portion of objective calculation
   void   SetErrorWeight(double aErrorWeight) { mErrorWeight = aErrorWeight; }
   double GetErrorWeight() const { return mErrorWeight; }

   // Set weight factor for overshoot portion of objective calculation
   void   SetOvershootWeight(double aOvershootWeight) { mOvershootWeight = aOvershootWeight; }
   double GetOvershootWeight() const { return mOvershootWeight; }

   // Sets the Kp range
   void SetKpRange(double aLowerBound, double aUpperBound);

   // Sets the Ki range
   void SetKiRange(double aLowerBound, double aUpperBound);

   // Sets the Kd range
   void SetKdRange(double aLowerBound, double aUpperBound);

   // Sets the pointer to the RNG
   void SetRandomPtr(ut::Random* aRandom);

   // Returns PID Kp
   double GetKp() const { return std::pow(10.0, mLogKp); }

   // Returns PID Ki
   double GetKi() const { return std::pow(10.0, mLogKi); }

   // Returns PID Kd
   double GetKd() const { return std::pow(10.0, mLogKd); }

   // Returns PID low-pass alpha
   double GetLowPassAlpha() const { return mLowPassAlpha; }

private:
   void         DetermineChromosomeLengths();
   std::string  IntToBinString(size_t aInt, size_t aLength);
   unsigned int BinStringToInt(const std::string& aBinString);
   void         UpdateBasedOnChromosome();

   double      mObjectiveValue = -1.0;
   double      mLogKp          = 0.0;
   double      mLogKi          = 0.0;
   double      mLogKd          = 0.0;
   double      mLowPassAlpha   = 0.0;
   double      mLogKpRange[2]{-9.0, 2.0};
   double      mLogKiRange[2]{-9.0, 2.0};
   double      mLogKdRange[2]{-9.0, 2.0};
   double      mLowPassAlphaRange[2] = {0.0, 1.0};
   std::string mChromosome;
   size_t      mChromosomeLengths[4]{0, 0, 0, 0};

   double      mErrorWeight     = 1.0;
   double      mOvershootWeight = 1.0;
   ut::Random* mRandomPtr       = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif
