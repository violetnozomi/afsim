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

#include "AlphaBetaFilter.hpp"

#include <fstream>
#include <iomanip>

#include "UtInput.hpp"
#include "UtMatrix.hpp"
#include "UtMeasurementData.hpp"
#include "UtVec3.hpp"

namespace TrackingFilters
{

AlphaBetaFilter::AlphaBetaFilter()
   : Filter()
   , mAlpha(0.0)
   , mBeta(0.0)
   , mUpdateCount(0)
   , mLastUpdateTime(0.0)
   , mStateTransitionMatrix(6, 6)
   , mLastFilteredState(6, 1)
   , mDebugFlag(false)
{
   UtVec3d::Set(mFilteredLocation, 0.0);
   UtVec3d::Set(mFilteredVelocity, 0.0);
   UtVec3d::Set(mPredictedLocation, 0.0);
   UtVec3d::Set(mPFiltVel, 0.0);
}

AlphaBetaFilter::AlphaBetaFilter(const AlphaBetaFilter& aSrc)
   : Filter(aSrc)
   , mAlpha(aSrc.mAlpha)
   , mBeta(aSrc.mBeta)
   , mUpdateCount(aSrc.mUpdateCount)
   , mLastUpdateTime(aSrc.mLastUpdateTime)
   , mStateTransitionMatrix(aSrc.mStateTransitionMatrix)
   , mLastFilteredState(aSrc.mLastFilteredState)
   , mDebugFlag(aSrc.mDebugFlag)
{
   UtVec3d::Set(mFilteredLocation, 0.0);
   UtVec3d::Set(mFilteredVelocity, 0.0);
   UtVec3d::Set(mPredictedLocation, 0.0);
   UtVec3d::Set(mPFiltVel, 0.0);
}

// virtual
Filter* AlphaBetaFilter::Clone() const
{
   return new AlphaBetaFilter(*this);
}

// virtual
bool AlphaBetaFilter::ProcessInput(UtInput& aInput)
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

void AlphaBetaFilter::InitializeDebug()
{
   std::ofstream f1;
   f1.open("AB_FILTER_DEBUG.out", std::ios::out);
   f1 << "% Time        MX             MY             MX             ";
   f1 << "PX             PY             PZ             ";
   f1 << "FX             FY             FZ             R_X            R_Y            R_Z" << std::endl;
   f1.close();
}

// virtual
void AlphaBetaFilter::Reset(double aSimTime, const UtMatrixd* aInitialStatePtr)
{
   Filter::Reset(aSimTime, aInitialStatePtr);
   mUpdateCount    = 0;
   mLastUpdateTime = aSimTime;
}

// virtual
void AlphaBetaFilter::Update(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
{
   unsigned int i;

   double        deltaTime        = aSimTime - mLastUpdateTime;
   const double* measuredLocation = aDataIn.GetLocationWCS().GetData();

   if (mUpdateCount == 0)
   {
      // Initialize filtered location with measurement data.
      UtVec3d::Set(mFilteredLocation, measuredLocation);
      UtVec3d::Set(mPredictedLocation, measuredLocation);
      UtVec3d::Set(mFilteredVelocity, 0.0);
      UtVec3d::Set(mPFiltVel, mFilteredVelocity);
      mLastUpdateTime = aSimTime;
      ++mUpdateCount;
   }
   else if (mUpdateCount == 1)
   {
      if (deltaTime > 0.0)
      {
         // Initialize filtered velocity vector
         //
         // Vf(n) = (Xm - Xf(n-1)) / T

         for (i = 0; i < 3; ++i)
         {
            mFilteredVelocity[i] = (measuredLocation[i] - mFilteredLocation[i]) / deltaTime;
         }

         // Update filtered position with measurement data

         UtVec3d::Set(mFilteredLocation, measuredLocation);
         UtVec3d::Set(mPredictedLocation, measuredLocation);

         UtVec3d::Set(mPFiltVel, mFilteredVelocity);

         mLastUpdateTime = aSimTime;
         ++mUpdateCount;
      }
   }
   else
   {
      if (deltaTime > 0.0)
      {
         // Compute the Predicted Target Position
         for (i = 0; i < 3; ++i)
         {
            mPredictedLocation[i] = mFilteredLocation[i] + (deltaTime * mFilteredVelocity[i]);
         }

         // Compute the residual between the Predicted and Measured Position
         double deltaLocation[3];
         UtVec3d::Subtract(deltaLocation, measuredLocation, mPredictedLocation);

         // Compute the filtered state vectors
         //
         // Xf(n) = Xp(n-1) + a * (Xm - Xp(n-1))
         // Vf(n) = Vf(n-1) + (b / T) * (Xm - Xp(n-1))

         for (i = 0; i < 3; ++i)
         {
            mFilteredLocation[i] = mPredictedLocation[i] + (mAlpha * deltaLocation[i]);
            mFilteredVelocity[i] = mFilteredVelocity[i] + ((mBeta / deltaTime) * deltaLocation[i]);
         }

         UtVec3d::Set(mPFiltVel, mFilteredVelocity);

         mLastUpdateTime = aSimTime;
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

// virtual
void AlphaBetaFilter::OutputDebug(double aSimTime, const UtMeasurementData& aDataIn, UtMeasurementData& aDataOut)
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
   f1.open("AB_FILTER_DEBUG.out", std::ios::out | std::ios::app);
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

// Virtual
void AlphaBetaFilter::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
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

void AlphaBetaFilter::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
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
bool AlphaBetaFilter::IsStable() const
{
   return (mUpdateCount > 0);
}

void AlphaBetaFilter::UpdateStateTransitionMatrix(double aDt)
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

void AlphaBetaFilter::SetGains(double aAlpha, double aBeta)
{
   mAlpha = aAlpha;
   mBeta  = aBeta;
}

} // namespace TrackingFilters
