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

#ifndef P6DOFTUNERGASPECIMEN_HPP
#define P6DOFTUNERGASPECIMEN_HPP

#include "p6dof_export.h"

#include <vector>

#include "P6DofVehicleData.hpp"
#include "UtRandom.hpp"

class P6DOF_EXPORT P6DofTunerGASpecimen
{
public:
   struct PidLogElement
   {
      P6DOF::SinglePidValueData pidValues;
      double                    timeStamp;
   };

   P6DofTunerGASpecimen();

   void        GenerateSpecimen();
   double      AssessObjective(std::vector<PidLogElement>& aSimData);
   double      GetObjectiveValue() const { return mObjectiveValue; }
   void        GenerateChromosome();
   std::string GetChromosome() const { return mChromosome; }
   void        Mutate(double aMutationProbability);
   void        CrossOver(P6DofTunerGASpecimen& aSpecimen);

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

private:
   void         DetermineChromosomeLengths();
   std::string  IntToBinString(size_t aInt, size_t aLength);
   unsigned int BinStringToInt(const std::string& aBinString);
   void         UpdateBasedOnChromosome();

   static const double cDefaultLogKpRange[2];
   static const double cDefaultLogKiRange[2];
   static const double cDefaultLogKdRange[2];

   double      mObjectiveValue;
   double      mLogKp, mLogKi, mLogKd;
   double      mLogKpRange[2];
   double      mLogKiRange[2];
   double      mLogKdRange[2];
   std::string mChromosome;
   size_t      mChromosomeLengths[3];

   double      mErrorWeight;
   double      mOvershootWeight;
   ut::Random* mRandomPtr = nullptr;
};

#endif
