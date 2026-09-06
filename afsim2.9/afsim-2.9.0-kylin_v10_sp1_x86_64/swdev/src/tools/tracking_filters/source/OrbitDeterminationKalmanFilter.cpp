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

#include "OrbitDeterminationKalmanFilter.hpp"

#include <cmath>

#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLambertProblem.hpp"
#include "UtLog.hpp"
#include "UtMat3.hpp"
#include "UtMeasurementData.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalElements.hpp"
#include "UtOrbitalPropagator.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"

namespace
{
constexpr auto cNUM_SIGMA_POINTS = 12;
constexpr auto cNUM_STATES       = 6;
} // namespace

namespace TrackingFilters
{

OrbitDeterminationKalmanFilter::OrbitDeterminationKalmanFilter()
   : KalmanFilter()
   , mStateCovarianceDecomposition(6, 6)
   , mCovarianceSigmaPoints(6, 12)
   , mCovarianceSigmas(6, 12)
   , mCovarianceSigmasTranspose(12, 6)
   , mSigmaPointsValid(false)
   , mStatePropagatorPtr(nullptr)
{
   mNoiseModel = UtCovariance::cCONST_ACC;
}

OrbitDeterminationKalmanFilter::OrbitDeterminationKalmanFilter(const OrbitDeterminationKalmanFilter& aSrc)
   : KalmanFilter(aSrc)
   , mIsStable(aSrc.mIsStable)
   , mEpochSimTime(aSrc.mEpochSimTime)
   , mStartSimTime(aSrc.mStartSimTime)
   , mLastEpochSimTime(aSrc.mLastEpochSimTime)
   , mECI_ConversionPtr(aSrc.mECI_ConversionPtr)
   , mStateCovarianceDecomposition(aSrc.mStateCovarianceDecomposition)
   , mCovarianceSigmaPoints(aSrc.mCovarianceSigmaPoints)
   , mCovarianceSigmas(aSrc.mCovarianceSigmas)
   , mCovarianceSigmasTranspose(aSrc.mCovarianceSigmasTranspose)
   , mSigmaPointsValid(aSrc.mSigmaPointsValid)
   , mStatePropagatorPtr(aSrc.mStatePropagatorPtr)
   , mSigmaPointPropagators(aSrc.mSigmaPointPropagators)
{
   UtMat3d::Set(mTargetWCS_ToECI_Transform, aSrc.mTargetWCS_ToECI_Transform);
}

//! Base class' initialization routine, included for compatibility.
//! Because this method provides the least information about propagation and orbital state of the target,
//! it is preferred to use the other Initialize() routines instead.
bool TrackingFilters::OrbitDeterminationKalmanFilter::Initialize(double            aSimTime,
                                                                 const UtCalendar& aEpoch,
                                                                 const UtMatrixd*  aInitialStatePtr)
{
   // The filter interface mandates calling Reset to provide the initial state.
   KalmanFilter::Reset(aSimTime, aInitialStatePtr);
   mEpochSimTime     = aEpoch;
   mStartSimTime     = aEpoch;
   mLastEpochSimTime = mEpochSimTime;
   bool initialized  = true;

   if (aSimTime > 0.0)
   {
      mStartSimTime.AdvanceTimeBy(-aSimTime);
   }
   mECI_ConversionPtr = ut::make_unique<UtECI_Conversion>(mStartSimTime, ut::EarthWGS84()); // Only valid for earth WGS-84.
   mECI_ConversionPtr->SetTime(aSimTime);
   if (mInitialStatePtr != nullptr)
   {
      // convert initial state to ECI
      UtVec3d locWCS;
      mInitialStatePtr->CopyTo(locWCS.GetData(), 3, 0);
      UtVec3d locECI;
      mECI_ConversionPtr->SetLocationWCS(locWCS);
      mECI_ConversionPtr->GetWCS_ToECI_Transform(mTargetWCS_ToECI_Transform);
      UtMat3d::Transform(locECI.GetData(), mTargetWCS_ToECI_Transform, locWCS.GetData());
      mInitialStatePtr->CopyFrom(locECI.GetData(), 3, 0);
      UtVec3d velWCS;
      mInitialStatePtr->CopyTo(velWCS.GetData(), 3, 3);
      mECI_ConversionPtr->SetVelocityWCS(velWCS);
      UtVec3d velECI = mECI_ConversionPtr->ConvertVelocityWCS_ToECI(locWCS, velWCS);
      mInitialStatePtr->CopyFrom(velECI.GetData(), 3, 3);

      // Initialize the propagator with that state.
      initialized = InitializePropagators(ut::OrbitalState(mEpochSimTime,
                                                           ut::EarthWGS84(),
                                                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                           ut::OrbitalState::ReferenceFrame::cECI,
                                                           ut::OrbitalState::Vector(locECI, velECI)));
      InitializeFilter(aSimTime);
   }

   return initialized;
}

void OrbitDeterminationKalmanFilter::SetStatePropagator(std::unique_ptr<UtOrbitalPropagatorBase> aStatePropagatorPtr)
{
   mStatePropagatorPtr = std::move(aStatePropagatorPtr);
}

// override
bool TrackingFilters::OrbitDeterminationKalmanFilter::GetPredictedStateCovariance(double aSimTime, UtCovariance& aCovariance)
{
   UpdateTime(aSimTime);
   bool success = false;
   if (IsStable())
   {
      success   = true;
      double dt = aSimTime - mLastUpdateTime;
      if (dt > 0.0)
      {
         ComputePredictedStateCovariance(dt);
      }
      aCovariance = mPredictedStateCovariance;

      // For this filter, predicted state is in ECI; we need to convert to WCS.
      // Frame                                                      B     A.
      UtCovariance::ConvertFromFrameA_ToFrameB(aCovariance, mTargetWCS_ToECI_Transform);
   }
   return success;
}

bool OrbitDeterminationKalmanFilter::IsStable() const
{
   return mIsStable || KalmanFilter::IsStable();
}

bool OrbitDeterminationKalmanFilter::HyperbolicOrbitAllowed() const
{
   return mStatePropagatorPtr->HyperbolicPropagationAllowed();
}

// override
void TrackingFilters::OrbitDeterminationKalmanFilter::Update(double                   aSimTime,
                                                             const UtMeasurementData& aDataIn,
                                                             UtMeasurementData&       aDataOut)
{
   UpdateTime(aSimTime);
   KalmanFilter::Update(aSimTime, aDataIn, aDataOut);
   if (mPropagatorsInitialized)
   {
      mIsStable = UpdateSigmaPoints(mEpochSimTime); // provides basis for computing predicted state and covariance
   }
}

// override
void TrackingFilters::OrbitDeterminationKalmanFilter::NoDetectUpdate(double aSimTime, UtMeasurementData& aDataOut)
{
   // For this filter, the result should be the same as GetKinematicEstimate
   // (the only difference would be that we temporarily save a filtered state, but that state is not directly accessible).
   GetKinematicEstimate(aSimTime, aDataOut);
}

// override
void TrackingFilters::OrbitDeterminationKalmanFilter::GetKinematicEstimate(double aSimTime, UtMeasurementData& aDataOut)
{
   // Update the State Transition Matrix
   double dt = aSimTime - mLastUpdateTime;

   // The original simulation times should be saved and restored later,
   // as this is not really an update.
   UtCalendar lastEpochSimTime = mLastEpochSimTime;
   UtCalendar epochSimTime     = mEpochSimTime;
   mECI_ConversionPtr->SetTime(aSimTime);

   if (dt > 0.0)
   {
      UpdateTime(aSimTime);
   }

   ComputePredictedState(dt);
   double locationEstimateECI[3];
   double velocityEstimateECI[3];
   for (int i = 0; i < 3; ++i)
   {
      locationEstimateECI[i] = mPredictedState(i);
      velocityEstimateECI[i] = mPredictedState(i + 3);
   }

   // Estimates are in ECI; need to convert to WCS
   double locWCS[3];
   mECI_ConversionPtr->ConvertECI_ToWCS(locationEstimateECI, locWCS);
   aDataOut.SetLocationWCS(locWCS);

   if (IsStable())
   {
      double velWCS[3];
      mECI_ConversionPtr->ConvertVelocityECI_ToWCS(locationEstimateECI, velocityEstimateECI, velWCS);
      aDataOut.SetVelocityWCS(velWCS);
   }

   UtCovariance predictedStateCovarianceWCS;
   mECI_ConversionPtr->GetWCS_ToECI_Transform(mTargetWCS_ToECI_Transform);
   if (GetPredictedStateCovariance(aSimTime, predictedStateCovarianceWCS)) // TODO optimize with ComputePredictedState, above.
   {
      predictedStateCovarianceWCS.CopyTrackData(aDataOut);
      aDataOut.SetStateCovariance(aSimTime, predictedStateCovarianceWCS);
   }

   if (dt > 0.0)
   {
      // Restore times to their original values
      mEpochSimTime     = epochSimTime;
      mLastEpochSimTime = lastEpochSimTime;
      mECI_ConversionPtr->SetTime(mLastUpdateTime);
   }
}

const UtMatrixd* TrackingFilters::OrbitDeterminationKalmanFilter::GetResidualCovarianceMatrix()
{
   return &mWCS_ResidualCovariance;
}

const UtCovariance* TrackingFilters::OrbitDeterminationKalmanFilter::GetResidualCovariance()
{
   return &mWCS_ResidualCovariance;
}

// virtual
Filter* OrbitDeterminationKalmanFilter::Clone() const
{
   return new OrbitDeterminationKalmanFilter(*this);
}

bool TrackingFilters::OrbitDeterminationKalmanFilter::ProcessInput(UtInput& aInput)
{
   std::string command = aInput.GetCommand();
   if (command == "process_noise_model")
   {
      ut::log::error() << "Setting the process noise model for orbit determination Kalman filter is not allowed.";
      throw(UtInput::OutOfContext(aInput));
   }
   return KalmanFilter::ProcessInput(aInput);
}

bool OrbitDeterminationKalmanFilter::Initialize(double aSimTime, const ut::OrbitalState& aState)
{
   bool ok = InitializePropagators(aState);

   if (ok)
   {
      ut::OrbitalState::Vector osvInertial = aState.GetOrbitalStateVectorInertial();
      UtVec3d                  locInertial = osvInertial.GetLocation();
      UtVec3d                  velInertial = osvInertial.GetVelocity();
      UtMatrixd                initialState(6, 1);
      for (auto xyz = 0; xyz < 3; ++xyz)
      {
         initialState(xyz)     = locInertial[xyz];
         initialState(xyz + 3) = velInertial[xyz];
      }
      KalmanFilter::Reset(aSimTime, &initialState);

      mEpochSimTime     = aState.GetEpoch();
      mStartSimTime     = aState.GetEpoch();
      mLastEpochSimTime = mEpochSimTime;

      if (aSimTime > 0.0)
      {
         mStartSimTime.AdvanceTimeBy(-aSimTime);
      }
      mECI_ConversionPtr = ut::make_unique<UtECI_Conversion>(mStartSimTime, aState.GetCentralBody());
      mECI_ConversionPtr->SetTime(0.0);

      InitializeFilter(aSimTime);
   }

   return ok;
}

bool OrbitDeterminationKalmanFilter::InitializePropagators(const ut::OrbitalState& aInitialState)
{
   // Initialize the propagators with that state.
   if (mStatePropagatorPtr == nullptr)
   {
      mStatePropagatorPtr = ut::make_unique<UtOrbitalPropagator>();
   }
   mPropagatorsInitialized = mStatePropagatorPtr->Initialize(aInitialState);
   mSigmaPointPropagators.clear();
   mSigmaPointPropagators.resize(cNUM_SIGMA_POINTS, ut::clone(*mStatePropagatorPtr));
   for (auto& prop : mSigmaPointPropagators)
   {
      mPropagatorsInitialized &= prop->Initialize(aInitialState);
   }
   return mPropagatorsInitialized;
}

// protected override
void TrackingFilters::OrbitDeterminationKalmanFilter::UpdateMeasurementInputs(double                   aSimTime,
                                                                              const UtMeasurementData& aDataIn)
{
   KalmanFilter::UpdateMeasurementInputs(aSimTime, aDataIn);

   mECI_ConversionPtr->GetWCS_ToECI_Transform(mTargetWCS_ToECI_Transform);

   // convert measurement and measurement covariance to ECI
   double locWCS[3];
   mMeasurement.CopyTo(locWCS, 3, 0);
   double locECI[3];
   UtMat3d::Transform(locECI, mTargetWCS_ToECI_Transform, locWCS);
   mMeasurement.CopyFrom(locECI, 3, 0);

   // Frame                                                                     B      A
   UtCovariance::ConvertFromFrameB_ToFrameA(mMeasurementNoiseCovariance, mTargetWCS_ToECI_Transform);

   if (!mInitialized) // No initial data were provided at initialization.
   {
      InitializeFilter(aSimTime);
   }
}

// protected override
void TrackingFilters::OrbitDeterminationKalmanFilter::UpdateFilteredOutputs(double aSimTime, UtMeasurementData& aDataOut)
{
   double locECI[3];
   mFilteredState.CopyTo(locECI, 3);
   UtMat3d::InverseTransform(mWCS_Location.GetData(), mTargetWCS_ToECI_Transform, locECI);
   aDataOut.SetLocationWCS(mWCS_Location.GetData());
   aDataOut.SetUpdateTime(aSimTime);

   if (IsStable())
   {
      double velECI[3];
      mFilteredState.CopyTo(velECI, 3, 3);

      if (!mPropagatorsInitialized)
      {
         InitializePropagators(ut::OrbitalState(mEpochSimTime,
                                                ut::EarthWGS84(),
                                                ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                ut::OrbitalState::ReferenceFrame::cECI,
                                                ut::OrbitalState::Vector(locECI, velECI)));
      }

      double* filteredVelocity = mWCS_Velocity.GetData();
      mECI_ConversionPtr->ConvertVelocityECI_ToWCS(locECI, velECI, filteredVelocity);
      aDataOut.SetVelocityWCS(mWCS_Velocity.GetData());

      mWCS_StateCovariance = mStateCovariance;
      UtCovariance::ConvertFromFrameA_ToFrameB(mWCS_StateCovariance, mTargetWCS_ToECI_Transform);
      mWCS_StateCovariance.CopyTrackData(aDataOut);

      mWCS_MeasurementCovariance = mMeasurementNoiseCovariance;
      UtCovariance::ConvertFromFrameA_ToFrameB(mWCS_MeasurementCovariance, mTargetWCS_ToECI_Transform);
      mWCS_MeasurementCovariance.CopyTrackData(aDataOut);

      mWCS_ResidualCovariance = mResidualCovariance;
      UtCovariance::ConvertFromFrameA_ToFrameB(mWCS_ResidualCovariance, mTargetWCS_ToECI_Transform);
      mWCS_ResidualCovariance.CopyTrackData(aDataOut);

      aDataOut.SetStateCovariance(mLastUpdateTime, mWCS_StateCovariance);
      aDataOut.SetMeasurementCovariance(mLastUpdateTime, mWCS_MeasurementCovariance);
   }
}

// override
//! Compute the transition matrix consistent with orbital motion (ballistic kinematics;
//! See Vallado, equation 10-46).
//!@param aDt The change in time since the last filter update.
//!@note This filter often does not work well when using this linear update form
//! (corresponding with the standard linear Kalman filter); measurements must be precise,
//! and aDt should not be too large (more than order 10 seconds).  The filter only uses this form
//! when sigma points can't be computed (probably because of very imprecise measurements).
void OrbitDeterminationKalmanFilter::ComputeTransitionMatrix(double aDt)
{
   double r0 = sqrt(mFilteredState(0) * mFilteredState(0) + mFilteredState(1) * mFilteredState(1) +
                    mFilteredState(2) * mFilteredState(2));
   double lowerLeft3x3_DiagonalElement =
      -mECI_ConversionPtr->GetCentralBody().GetGravitationalParameter() * aDt / pow(r0, 3);
   double upperLeft3x3_DiagonalElement =
      1.0 - mECI_ConversionPtr->GetCentralBody().GetGravitationalParameter() * aDt * aDt / (2.0 * pow(r0, 3));

   mStateTransitionMatrix(0, 0) = upperLeft3x3_DiagonalElement;
   mStateTransitionMatrix(1, 1) = upperLeft3x3_DiagonalElement;
   mStateTransitionMatrix(2, 2) = upperLeft3x3_DiagonalElement;

   mStateTransitionMatrix(0, 3) = aDt;
   mStateTransitionMatrix(1, 4) = aDt;
   mStateTransitionMatrix(2, 5) = aDt;

   mStateTransitionMatrix(3, 0) = lowerLeft3x3_DiagonalElement;
   mStateTransitionMatrix(4, 1) = lowerLeft3x3_DiagonalElement;
   mStateTransitionMatrix(5, 2) = lowerLeft3x3_DiagonalElement;

   mStateTransitionMatrix(3, 3) = 1.0;
   mStateTransitionMatrix(4, 4) = 1.0;
   mStateTransitionMatrix(5, 5) = 1.0;
}

// protected override
//! Same as base class' method, but orients to / from ECI instead of WCS.
void TrackingFilters::OrbitDeterminationKalmanFilter::ComputeProcessNoiseCovariance(double aDt)
{
   if (IsStable())
   {
      mProcessNoiseCovariance.ComputeProcessNoise(aDt, mTargetECI_ToECS_Transform, mNoiseModel);
   }
   else
   {
      mProcessNoiseCovariance.ComputeProcessNoise(aDt, mTargetECI_ToNED_Transform, mNoiseModel);
   }
}

// protected override
void TrackingFilters::OrbitDeterminationKalmanFilter::ComputeCoordinateTransforms()
{
   assert(mPass >= 1);
   mECI_ConversionPtr->GetWCS_ToECI_Transform(mTargetWCS_ToECI_Transform);
   double locECI[3];
   double locWCS[3];
   mPredictedState.CopyTo(locECI, 3, 0);
   mECI_ConversionPtr->ConvertECI_ToWCS(locECI, locWCS);
   UtCovariance::ComputeWCS_ToNED_Transform(locWCS, mTargetWCS_ToNED_Transform);
   double targetECI_ToWCS_Transform[3][3];
   UtMat3d::Transpose(targetECI_ToWCS_Transform, mTargetWCS_ToECI_Transform);
   UtMat3d::Multiply(mTargetECI_ToNED_Transform, mTargetWCS_ToNED_Transform, targetECI_ToWCS_Transform);

   if (IsStable()) // We will have a valid velocity estimate.
   {
      double velECI[3];
      double velWCS[3];
      mPredictedState.CopyTo(velECI, 3, 3);
      mECI_ConversionPtr->ConvertVelocityECI_ToWCS(locECI, velECI, velWCS);
      UtCovariance::ComputeWCS_ToECS_Transform(velWCS, mTargetWCS_ToNED_Transform, mTargetWCS_ToECS_Transform);
      UtMat3d::Multiply(mTargetECI_ToECS_Transform, mTargetWCS_ToECS_Transform, targetECI_ToWCS_Transform);
   }
}

// protected override
//! If the filter is stable, update mPredcitedState using the mean of the sigma points propagated forward in time.
//! Otherwise, if an initial state was provided during the call to Reset, propagate the last filtered state forward.
//! using an orbital propagator.  Finally, if an initial state was not provided, or if the state does not provide a
//! valid orbit, perform a linear update by calling the base class' method.
void TrackingFilters::OrbitDeterminationKalmanFilter::ComputePredictedState(double aDt)
{
   if (mSigmaPointsValid) // The sigma points were successfully computed on the last Update.
   {
      PropagateSigmaPoints(mEpochSimTime);
      ComputePredictedFromSigmaPoints();
   }
   else if (ComputePredictedFromPropagatedState())
   {
   }
   else
   {
      ComputeTransitionMatrix(aDt); // Needed to set the state transition matrix.
      KalmanFilter::ComputePredictedState(aDt);
   }
}

// protected override
//! If the filter is stable, update mPredictedStateCovariance by propagating the sigma points forward and
//! reconstituting the covariance at the new locations.  Otherwise, use the linear state transition matrix
//! and transpose to provide an update using the base class' method.
//!@note The resulting covariance is referenced to ECI, not WCS / ECEF.
void TrackingFilters::OrbitDeterminationKalmanFilter::ComputePredictedStateCovariance(double aDt)
{
   if (mSigmaPointsValid)
   {
      PropagateSigmaPoints(mEpochSimTime);
      ComputePredictedFromSigmaPoints();
      ComputeCoordinateTransforms();
      ComputeProcessNoiseCovariance(aDt);
      mPredictedStateCovariance += mProcessNoiseCovariance;
   }
   else
   {
      UpdateTransitionMatrix(aDt); // Needed to set the state transition matrix and transpose.
      KalmanFilter::ComputePredictedStateCovariance(aDt);
   }
}

// private
void TrackingFilters::OrbitDeterminationKalmanFilter::UpdateTime(double aSimTime)
{
   mLastEpochSimTime = mEpochSimTime;
   mEpochSimTime     = mStartSimTime;
   mEpochSimTime.AdvanceTimeBy(aSimTime);
   mECI_ConversionPtr->SetTime(aSimTime);
}

// private
//! Update the UKF sigma points to be propagated from the state covariance matrix
//! (which would have just been computed in UpdateFilter() ).
//! This algorithm follows Vallado,
//! Fundamentals of Astrodynamics and Applications, 4th ed., p. 797-798.
//! @param aEpoch The epoch to which the sigma points are to be re-initialized.
bool TrackingFilters::OrbitDeterminationKalmanFilter::UpdateSigmaPoints(const UtCalendar& aEpoch)
{
   bool sigmaPointsUpdated = false;

   // Compute the Cholesky decomposition of the state covariance matrix.
   bool success;
   mStateCovarianceDecomposition.CholeskyDecomposition(mStateCovariance, success);

   if (success)
   {
      // Multiply by the square root of 6 (eq. 10-36).
      mStateCovarianceDecomposition.Multiply(mStateCovarianceDecomposition, sqrt(6.0));

      // Add the Filtered state to the (+ and -) offset sigma points computed above.
      // This results in 12 sigma point locations in state phase space.
      for (auto i = 0; i < 6; ++i)
      {
         auto j = i * 2;
         for (auto stateNum = 0; stateNum < cNUM_STATES; ++stateNum)
         {
            mCovarianceSigmaPoints(stateNum, j) = mStateCovarianceDecomposition(stateNum, i) + mFilteredState(stateNum);
            mCovarianceSigmaPoints(stateNum, j + 1) =
               -mStateCovarianceDecomposition(stateNum, i) + mFilteredState(stateNum);
         }
      }

      // The sigma points are to be transformed into orbital elements for forward propagation.
      std::vector<ut::OrbitalState> iosVec;
      sigmaPointsUpdated = true;

      for (auto sigmaPointNum = 0; sigmaPointNum < cNUM_SIGMA_POINTS; ++sigmaPointNum)
      {
         UtVec3d locECI;
         UtVec3d velECI;
         for (auto xyz = 0; xyz < 3; ++xyz)
         {
            locECI[xyz] = mCovarianceSigmaPoints(xyz, sigmaPointNum);
            velECI[xyz] = mCovarianceSigmaPoints(xyz + 3, sigmaPointNum);
         }

         iosVec.emplace_back(aEpoch,
                             mECI_ConversionPtr->GetCentralBody(),
                             ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                             ut::OrbitalState::ReferenceFrame::cECI,
                             ut::OrbitalState::Vector(locECI, velECI));
      }

      if (sigmaPointsUpdated)
      {
         // Initialize the 12 orbital propagators.
         mSigmaPointsValid = true; // Keep this value true if only set once (allow coasting)
         for (auto sigmaPointNum = 0; sigmaPointNum < cNUM_SIGMA_POINTS; ++sigmaPointNum)
         {
            if (!mSigmaPointPropagators[sigmaPointNum]->Initialize(iosVec[sigmaPointNum]))
            {
               ut::log::warning()
                  << "Orbit Determination Kalman Filter: Could not initialize sigma point from orbital state.";
               sigmaPointsUpdated = false;
               break;
            }
         }
      }
   }
   else
   {
      ut::log::warning() << "Orbit Determination Kalman Filter: Could not compute the Cholesky decomposition of state "
                            "covariance matrix.";
   }
   return sigmaPointsUpdated;
}

// private
//! Propagate the sigma points to the given epoch and save the result for processing by ComputePredictedFromSimgaPoints().
//! @param aEpoch The epoch to which the points should be propagated.
void TrackingFilters::OrbitDeterminationKalmanFilter::PropagateSigmaPoints(const UtCalendar& aEpoch)
{
   for (auto sigmaPointNum = 0; sigmaPointNum < cNUM_SIGMA_POINTS; ++sigmaPointNum)
   {
      mSigmaPointPropagators[sigmaPointNum]->Update(aEpoch);
      ut::OrbitalState::Vector osvInertial =
         mSigmaPointPropagators[sigmaPointNum]->GetOrbitalState().GetOrbitalStateVectorInertial();
      UtVec3d locECI = osvInertial.GetLocation();
      UtVec3d velECI = osvInertial.GetVelocity();

      for (auto xyz = 0; xyz < 3; ++xyz)
      {
         mCovarianceSigmaPoints(xyz, sigmaPointNum)     = locECI[xyz];
         mCovarianceSigmaPoints(xyz + 3, sigmaPointNum) = velECI[xyz];
      }
   }
}

// private
//! Compute the predicted state and predicted covariance based upon the sigma points propagated in PropagateSigmaPoints().
//! The process noise covariance is not computed and must be added to the predicted state covariance in another step.
void TrackingFilters::OrbitDeterminationKalmanFilter::ComputePredictedFromSigmaPoints()
{
   mPredictedState.Fill(0.0);

   // Find the mean of the covariance points.  This becomes the predicted state.
   for (auto sigmaPointNum = 0; sigmaPointNum < cNUM_SIGMA_POINTS; ++sigmaPointNum)
   {
      for (auto stateNum = 0; stateNum < cNUM_STATES; ++stateNum)
      {
         mPredictedState(stateNum) += mCovarianceSigmaPoints(stateNum, sigmaPointNum);
      }
   }

   mPredictedState.Divide(mPredictedState, 12.0);
   mCovarianceSigmas = mCovarianceSigmaPoints;

   // The covariance is computed using the sigma points and subtracting the mean.
   for (auto sigmaPointNum = 0; sigmaPointNum < cNUM_SIGMA_POINTS; ++sigmaPointNum)
   {
      for (auto stateNum = 0; stateNum < cNUM_STATES; ++stateNum)
      {
         mCovarianceSigmas(stateNum, sigmaPointNum) -= mPredictedState(stateNum);
      }
   }

   mCovarianceSigmasTranspose.Transpose(mCovarianceSigmas);
   mPredictedStateCovariance.Multiply(mCovarianceSigmas, mCovarianceSigmasTranspose);
   mPredictedStateCovariance.Divide(mPredictedStateCovariance, 12.0);
   mPredictedStateCovariance.Symmetrize();

   // Re-initialize the internal propagator with the predicted state.
   UtVec3d locECI;
   UtVec3d velECI;
   for (auto xyz = 0; xyz < 3; ++xyz)
   {
      locECI[xyz] = mPredictedState(xyz);
      velECI[xyz] = mPredictedState(xyz + 3);
   }

   ut::OrbitalState initialState = mStatePropagatorPtr->GetInitialOrbitalState();
   initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
   initialState.Set(mEpochSimTime, ut::OrbitalState::Vector(locECI, velECI));
   mStatePropagatorPtr->Initialize(initialState);
}

// private
//! If an initial state was provided (including velocity) it is used to propagate the state forward until
//! the filter stabilizes (currently after three passes).
bool TrackingFilters::OrbitDeterminationKalmanFilter::ComputePredictedFromPropagatedState()
{
   if (mPropagatorsInitialized)
   {
      mStatePropagatorPtr->Update(mEpochSimTime);
      ut::OrbitalState::Vector osvInertial = mStatePropagatorPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
      for (auto xyz = 0; xyz < 3; ++xyz)
      {
         mPredictedState(xyz)     = osvInertial.GetLocation()[xyz];
         mPredictedState(xyz + 3) = osvInertial.GetVelocity()[xyz];
      }
      return true;
   }
   return false;
}

} // namespace TrackingFilters
