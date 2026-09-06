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

#include "AlphaBetaGammaFilter.hpp"

#include <fstream>
#include <iomanip>

#include "UtInput.hpp"
#include "UtMeasurementData.hpp"
#include "UtVec3.hpp"

namespace TrackingFilters
{

AlphaBetaGammaFilter::AlphaBetaGammaFilter()
   : Filter()
   , mAlpha(0.0)
   , mBeta(0.0)
   , mGamma(0.0)
   , mLastUpdateTime(0.0)
   , mUpdateCount(0)
   , mFilterFirstPassTime(0.0)
   , mStateTransitionMatrix(6, 6)
   , mLastFilteredState(6, 1)
   , mDebugFlag(false)
{
   UtVec3d::Set(mFilteredLocation, 0.0);
   UtVec3d::Set(mFilteredVelocity, 0.0);
   UtVec3d::Set(mPredictedLocation, 0.0);
   UtVec3d::Set(mFilteredAcceleration, 0.0);
   UtVec3d::Set(mPMeasLocation, 0.0);
   UtVec3d::Set(mP2MeasLocation, 0.0);
   UtVec3d::Set(mPFiltVel, 0.0);
   UtVec3d::Set(mPFiltAcc, 0.0);
}

AlphaBetaGammaFilter::AlphaBetaGammaFilter(const AlphaBetaGammaFilter& aSrc)
   : Filter(aSrc)
   , mAlpha(aSrc.mAlpha)
   , mBeta(aSrc.mBeta)
   , mGamma(aSrc.mGamma)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mUpdateCount(aSrc.mUpdateCount)
   , mFilterFirstPassTime(aSrc.mFilterFirstPassTime)
   , mStateTransitionMatrix(aSrc.mStateTransitionMatrix)
   , mLastFilteredState(aSrc.mLastFilteredState)
   , mDebugFlag(aSrc.mDebugFlag)
{
   UtVec3d::Set(mFilteredLocation, 0.0);
   UtVec3d::Set(mFilteredVelocity, 0.0);
   UtVec3d::Set(mPredictedLocation, 0.0);
   UtVec3d::Set(mFilteredAcceleration, 0.0);
   UtVec3d::Set(mPMeasLocation, 0.0);
   UtVec3d::Set(mP2MeasLocation, 0.0);
   UtVec3d::Set(mPFiltVel, 0.0);
   UtVec3d::Set(mPFiltAcc, 0.0);
}

// virtual
Filter* AlphaBetaGammaFilter::Clone() const
{
   return new AlphaBetaGammaFilter(*this);
}

// virtual
bool AlphaBetaGammaFilter::ProcessInput(UtInput& aInput)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());
   if (command == "alpha")
   {
      myCommand = true;
      aInput.ReadValue(mAlpha);
      aInput.ValueInClosedRange(mAlpha, 0.0, 1.0);
   }
   else if (command == "beta")
   {
      myCommand = true;
      aInput.ReadValue(mBeta);
      aInput.ValueInClosedRange(mBeta, 0.0, 1.0);
   }
   else if (command == "gamma")
   {
      myCommand = true;
      aInput.ReadValue(mGamma);
      aInput.ValueInClosedRange(mGamma, 0.0, 1.0);
   }
   else if (command == "debug")
   {
      if (!mDebugFlag)
      {
         InitializeDebug();
      }
      mDebugFlag = true;
      myCommand  = true;
   }
   return myCommand;
}

// virtual
void AlphaBetaGammaFilter::InitializeDebug()
{
   std::ofstream f1;
   f1.open("ABG_FILTER_DEBUG.out", std::ios::out);
   f1 << "% Time        MX             MY             MX             ";
   f1 << "PX             PY             PZ             ";
   f1 << "FX             FY             FZ             R_X            R_Y            R_Z" << std::endl;
   f1.close();
}

// virtual
void AlphaBetaGammaFilter::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr)
{
   Filter::Reset(aSimTime, aInitialStatePtr);
   mUpdateCount    = 0;
   mLastUpdateTime = aSimTime;
}

// virtual
void AlphaBetaGammaFilter::OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   double residual[3];

   const double* measuredLocation = aDataIn.GetLocationWCS().GetData();
   if (mUpdateCount > 1)
   {
      UtVec3d::Subtract(residual, measuredLocation, mPredictedLocation);
   }
   else
   {
      UtVec3d::Set(residual, 0.0);
   }

   std::ofstream f1;
   f1.open("ABG_FILTER_DEBUG.out", std::ios::out | std::ios::app);
   f1.setf(std::ios::left | std::ios::fixed | std::ios::showpoint);
   // clang-format off
   f1 << "  " << std::setw(12) << std::setprecision(4) << aSimTime
      << std::setw(15) << std::setprecision(4) << measuredLocation[0]
      << std::setw(15) << std::setprecision(4) << measuredLocation[1]
      << std::setw(15) << std::setprecision(4) << measuredLocation[2]
      << std::setw(15) << std::setprecision(4) << mPredictedLocation[0]
      << std::setw(15) << std::setprecision(4) << mPredictedLocation[1]
      << std::setw(15) << std::setprecision(4) << mPredictedLocation[2]
      << std::setw(15) << std::setprecision(4) << mFilteredLocation[0]
      << std::setw(15) << std::setprecision(4) << mFilteredLocation[1]
      << std::setw(15) << std::setprecision(4) << mFilteredLocation[2]
      << std::setw(15) << std::setprecision(4) << residual[0]
      << std::setw(15) << std::setprecision(4) << residual[1]
      << std::setw(15) << std::setprecision(4) << residual[2] << std::endl;
   // clang-format on
}

// virtual
void AlphaBetaGammaFilter::Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   unsigned int i;

   double        deltaTime        = aSimTime - mLastUpdateTime;
   const double* measuredLocation = aDataIn.GetLocationWCS().GetData();

   if (mUpdateCount == 0)
   {
      // Initialize filtered location with measurement data.

      UtVec3d::Set(mFilteredLocation, measuredLocation);
      UtVec3d::Set(mFilteredVelocity, 0.0);
      UtVec3d::Set(mFilteredAcceleration, 0.0);
      UtVec3d::Set(mPredictedLocation, measuredLocation);

      UtVec3d::Set(mPMeasLocation, measuredLocation);
      UtVec3d::Set(mPFiltVel, mFilteredVelocity);
      UtVec3d::Set(mPFiltAcc, mFilteredAcceleration);

      ++mUpdateCount;
      mLastUpdateTime      = aSimTime;
      mFilterFirstPassTime = aSimTime;
   }
   else if (mUpdateCount == 1)
   {
      if (deltaTime > 0.0)
      {
         // Update filtered velocity and Filtered Location vectors
         //
         // Vf(n) = (Xm - Xf(n-1)) / T
         // Xf(n) = (Xp(n) + alpha * (Xm(n) - Xm(n-1))

         for (i = 0; i < 3; ++i)
         {
            mFilteredVelocity[i] = (measuredLocation[i] - mPMeasLocation[i]) / deltaTime;
         }

         UtVec3d::Set(mFilteredLocation, measuredLocation);
         UtVec3d::Set(mPredictedLocation, measuredLocation);

         UtVec3d::Set(mP2MeasLocation, mPMeasLocation);
         UtVec3d::Set(mPMeasLocation, measuredLocation);
         UtVec3d::Set(mPFiltVel, mFilteredVelocity);
         UtVec3d::Set(mFilteredAcceleration, 0.0);
         UtVec3d::Set(mPFiltAcc, mFilteredAcceleration);

         mLastUpdateTime = aSimTime;
         ++mUpdateCount;
      }
   }
   else if (mUpdateCount == 2)
   {
      if (deltaTime > 0.0)
      {
         // Computed Filtered Location
         // Compute Filtered Velocity
         // First Computation of Filtered Acceleration, Need 3 Measurements
         for (i = 0; i < 3; ++i)
         {
            mPredictedLocation[i] = mFilteredLocation[i] + deltaTime * mFilteredVelocity[i];
         }

         double deltaLocation[3];
         UtVec3d::Subtract(deltaLocation, measuredLocation, mPredictedLocation);

         for (i = 0; i < 3; ++i)
         {
            mFilteredVelocity[i]     = mPFiltVel[i] + (mBeta / (deltaTime)) * (deltaLocation[i]);
            mFilteredLocation[i]     = mPredictedLocation[i] + mAlpha * (deltaLocation[i]);
            mFilteredAcceleration[i] = (measuredLocation[i] - 2.0 * mPMeasLocation[i] + mP2MeasLocation[i]) /
                                       pow((aSimTime - mFilterFirstPassTime), 2);
         }

         mLastUpdateTime = aSimTime;
         UtVec3d::Set(mP2MeasLocation, mPMeasLocation);
         UtVec3d::Set(mPMeasLocation, measuredLocation);
         UtVec3d::Set(mPFiltVel, mFilteredVelocity);
         UtVec3d::Set(mPFiltAcc, mFilteredAcceleration);
         ++mUpdateCount;
      }
   }
   else
   {
      if (deltaTime > 0.0)
      {
         // Compute the filtered state vectors
         //
         // Xf(n) = Xp(n-1) + a * (Xm - Xp(n-1))
         // Vf(n) = Vf(n-1) + (T * Af(n-1)) + (b / T) * (Xm - Xp(n-1))
         // Af(n) = Af(n-1) + (g / T^2) * (Xm - Xp(n-1))

         for (i = 0; i < 3; ++i)
         {
            mPredictedLocation[i] = mFilteredLocation[i] + deltaTime * mFilteredVelocity[i] +
                                    pow(deltaTime, 2) / 2 * mFilteredAcceleration[i];
         }

         double deltaLocation[3];
         UtVec3d::Subtract(deltaLocation, measuredLocation, mPredictedLocation);

         for (i = 0; i < 3; ++i)
         {
            mFilteredLocation[i] = mPredictedLocation[i] + (mAlpha * deltaLocation[i]);
            mFilteredVelocity[i] = mPFiltVel[i] + (deltaTime * mPFiltAcc[i]) + ((mBeta / deltaTime) * deltaLocation[i]);

            mFilteredAcceleration[i] = mPFiltAcc[i] + (mGamma / (pow(deltaTime, 2))) * deltaLocation[i];
         }

         mLastUpdateTime = aSimTime;
         UtVec3d::Set(mPFiltVel, mFilteredVelocity);
         UtVec3d::Set(mPFiltAcc, mFilteredAcceleration);
         ++mUpdateCount;
      }
   }

   aDataOut.SetLocationWCS(mFilteredLocation);
   aDataOut.SetWCS_LocationValid(true);
   aDataOut.SetVelocityWCS(mFilteredVelocity);
   aDataOut.SetWCS_VelocityValid(true);
   aDataOut.SetUpdateTime(aSimTime);

   for (i = 0; i < 3; ++i)
   {
      mLastFilteredState(i, 0)     = mFilteredLocation[i];
      mLastFilteredState(i + 3, 0) = mFilteredVelocity[i];
   }

   if (mDebugFlag)
   {
      OutputDebug(aSimTime, aDataIn, aDataOut);
   }
}

void AlphaBetaGammaFilter::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
{
   GetKinematicEstimate(aSimTime, aDataOut);
   const double* filteredLocation = aDataOut.GetLocationWCS().GetData();
   const double* filteredVelocity = aDataOut.GetVelocityWCS().GetData();

   // Save State Vector and the Update Time
   for (unsigned int i = 0; i < 3; ++i)
   {
      mLastFilteredState(i, 0)     = filteredLocation[i];
      mLastFilteredState(i + 3, 0) = filteredVelocity[i];
   }

   mLastUpdateTime = aSimTime;
}

void AlphaBetaGammaFilter::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
{
   double dt = aSimTime - mLastUpdateTime;

   UpdateStateTransitionMatrix(dt);

   UtMatrixd temp(6, 1);
   temp.Multiply(mStateTransitionMatrix, mLastFilteredState);

   double locationEstimate[3];
   double velocityEstimate[3];

   // Save to the arguments to return
   for (unsigned int i = 0; i < 3; ++i)
   {
      locationEstimate[i] = temp(i, 0);
      velocityEstimate[i] = temp(i + 3, 0);
   }

   aDataOut.SetLocationWCS(locationEstimate);
   aDataOut.SetVelocityWCS(velocityEstimate);
}

// virtual
bool AlphaBetaGammaFilter::IsStable() const
{
   return (mUpdateCount > 0);
}

void AlphaBetaGammaFilter::UpdateStateTransitionMatrix(double aDt)
{
   mStateTransitionMatrix.LoadIdentity();
   mStateTransitionMatrix(0, 0) = 1.0;
   mStateTransitionMatrix(1, 1) = 1.0;
   mStateTransitionMatrix(2, 2) = 1.0;
   mStateTransitionMatrix(3, 3) = 1.0;
   mStateTransitionMatrix(4, 4) = 1.0;
   mStateTransitionMatrix(5, 5) = 1.0;

   mStateTransitionMatrix(0, 3) = aDt;
   mStateTransitionMatrix(1, 4) = aDt;
   mStateTransitionMatrix(2, 5) = aDt;
}

void AlphaBetaGammaFilter::SetGains(double aAlpha, double aBeta, double aGamma)
{
   mAlpha = aAlpha;
   mBeta  = aBeta;
   mGamma = aGamma;
}

} // namespace TrackingFilters
