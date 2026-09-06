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

#ifndef WSFRUNGEKUTTAORBITALINTEGRATOR_HPP
#define WSFRUNGEKUTTAORBITALINTEGRATOR_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfIntegratingPropagator.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "WsfOrbitalIntegrator.hpp"

template<unsigned int Order, unsigned int StepCount, typename Integrator>
class WsfRungeKuttaOrbitalIntegrator : public WsfOrbitalIntegrator
{
public:
   WsfRungeKuttaOrbitalIntegrator()                                             = default;
   WsfRungeKuttaOrbitalIntegrator(const WsfRungeKuttaOrbitalIntegrator& aOther) = default;
   ~WsfRungeKuttaOrbitalIntegrator() override                                   = default;

   static constexpr unsigned int cORDER     = Order;
   static constexpr unsigned int cSTEPCOUNT = StepCount;

   enum class ErrorCriterion
   {
      cUNKNOWN_CRITERION,
      cL_INFINITY_NORM,
      cL_TWO_NORM
   };

   double       GetTolerance() const { return mTolerance; }
   void         SetTolerance(double aTolerance) { mTolerance = aTolerance; }
   unsigned int GetMaxAdjustmentAttempts() const { return mMaxAdjustmentAttempts; }
   void         SetMaxAdjustmentAttempts(unsigned int aMaxAdjustmentAttempts)
   {
      mMaxAdjustmentAttempts = aMaxAdjustmentAttempts;
   }
   double         GetMaxStepSize() const { return mMaxStepSize; }
   void           SetMaxStepSize(double aMaxStepSize) { mMaxStepSize = aMaxStepSize; }
   double         GetMinStepSize() const { return mMinStepSize; }
   void           SetMinStepSize(double aMinStepSize) { mMinStepSize = aMinStepSize; }
   ErrorCriterion GetErrorCriterion() const { return mErrorCriterion; }
   void           SetErrorCriterion(ErrorCriterion aErrorCriterion) { mErrorCriterion = aErrorCriterion; }
   double         GetInitialStepSize() const { return mInitialStepSize; }
   void           SetInitialStepSize(double aInitialStepSize) { mInitialStepSize = aInitialStepSize; }

   static std::string GetStringFromCriterion(ErrorCriterion aErrorCriterion)
   {
      std::string retval{};
      switch (aErrorCriterion)
      {
      case ErrorCriterion::cL_INFINITY_NORM:
         retval = "L_infinity";
         break;
      case ErrorCriterion::cL_TWO_NORM:
         retval = "L_2";
         break;
      case ErrorCriterion::cUNKNOWN_CRITERION: // Intentional fall-through
      default:
         retval = "<UNKNOWN>";
         break;
      }
      return retval;
   }

   static ErrorCriterion GetErrorCriterionFromString(const std::string& aErrorCriterion)
   {
      ErrorCriterion retval{ErrorCriterion::cUNKNOWN_CRITERION};
      if (aErrorCriterion == "L_infinity")
      {
         retval = ErrorCriterion::cL_INFINITY_NORM;
      }
      else if (aErrorCriterion == "L_2")
      {
         retval = ErrorCriterion::cL_TWO_NORM;
      }
      return retval;
   }

   bool ProcessInput(UtInput& aInput) override
   {
      bool        retval{true};
      std::string command = aInput.GetCommand();
      if (command == "tolerance")
      {
         aInput.ReadValue(mTolerance);
         aInput.ValueGreater(mTolerance, 0.0);
      }
      else if (command == "max_adjustment_attempts")
      {
         aInput.ReadValue(mMaxAdjustmentAttempts);
         aInput.ValueGreater(mMaxAdjustmentAttempts, 0u);
      }
      else if (command == "max_step_size")
      {
         aInput.ReadValue(mMaxStepSize);
         aInput.ValueGreater(mMaxStepSize, 0.0);
         if (mMaxStepSize < mMinStepSize)
         {
            throw UtInput::BadValue{aInput, "max_step_size must be larger than min_step_size."};
         }
      }
      else if (command == "min_step_size")
      {
         aInput.ReadValue(mMinStepSize);
         aInput.ValueGreaterOrEqual(mMinStepSize, 0.0);
         if (mMinStepSize > mMaxStepSize)
         {
            throw UtInput::BadValue{aInput, "min_step_size must be smaller than max_step_size."};
         }
      }
      else if (command == "error_criterion")
      {
         std::string criterion;
         aInput.ReadValue(criterion);
         mErrorCriterion = GetErrorCriterionFromString(criterion);
         if (mErrorCriterion == ErrorCriterion::cUNKNOWN_CRITERION)
         {
            throw UtInput::BadValue(aInput, "Unknown error criterion.");
         }
      }
      else if (command == "initial_step_size")
      {
         aInput.ReadValue(mInitialStepSize);
         aInput.ValueGreater(mInitialStepSize, 0.0);
      }
      else
      {
         retval = false;
      }
      return retval;
   }

   ut::OrbitalState AdvanceToTime(const WsfOrbitalDynamics& aDynamics,
                                  const UtCalendar&         aFinalTime,
                                  const ut::OrbitalState&   aInitialState) override
   {
      ut::OrbitalState retval{aInitialState};

      if (mStepSize < 0.0)
      {
         mStepSize = mInitialStepSize;
      }

      double       finalTime = aFinalTime.GetTimeSince(retval.GetEpoch());
      double       currentTime{0.0};
      bool         acceptStep{false};
      unsigned int attempts{0};

      // Detect reversal of propagation direction.
      if ((finalTime < 0.0 && mStepSize > 0.0) || (finalTime > 0.0 && mStepSize < 0.0))
      {
         mStepSize = -mStepSize;
      }

      while (fabs(currentTime) < fabs(finalTime))
      {
         if (fabs(mStepSize + currentTime) > fabs(finalTime))
         {
            mStepSize = finalTime - currentTime;
         }

         TakeStep(aDynamics, retval);
         double error = ComputeError(retval.GetOrbitalStateVector());
         if (error < mTolerance)
         {
            acceptStep = true;
         }
         else
         {
            ++attempts;
            acceptStep = false;
         }

         if (attempts > mMaxAdjustmentAttempts)
         {
            if (!mWarned)
            {
               auto warn = ut::log::warning() << "Unable to find acceptable step size.";
               warn.AddNote() << "Integration will proceed, but error will be outside tolerance.";
               warn.AddNote() << "Attempts made: " << attempts;
               warn.AddNote() << "Integrator type: " << Integrator::cTYPE;
               mWarned = true;
            }
            acceptStep = true;
         }

         if (acceptStep)
         {
            AdvanceState(retval);
            attempts = 0;
            currentTime += mStepSize;
         }

         AdjustTimeStep(error);
      }

      return retval;
   }

private:
   static double L_InfinityNorm(const UtVec3d& aVector)
   {
      return std::max(fabs(aVector[0]), std::max(fabs(aVector[1]), fabs(aVector[2])));
   }

   double ComputeError(const ut::OrbitalStateVector& aInitialOSV) const
   {
      double retval{};
      switch (mErrorCriterion)
      {
      case ErrorCriterion::cL_INFINITY_NORM:
         retval = std::max(L_InfinityNorm(mPosDiff), L_InfinityNorm(mVelDiff));
         break;
      case ErrorCriterion::cL_TWO_NORM:
      {
         UtVec3d stepPosDiff = mPredictedPosition - aInitialOSV.GetLocation();
         double  posMag2     = stepPosDiff.MagnitudeSquared();
         double  posError    = mPosDiff.MagnitudeSquared();
         if (posMag2 > 0.1)
         {
            posError /= posMag2;
         }
         posError = sqrt(posError);

         UtVec3d stepVelDiff = mPredictedVelocity - aInitialOSV.GetVelocity();
         double  velMag2     = stepVelDiff.MagnitudeSquared();
         double  velError    = mVelDiff.MagnitudeSquared();
         if (velMag2 > 0.1)
         {
            velError /= velMag2;
         }
         velError = sqrt(velError);

         retval = std::max(posError, velError);
      }
      break;
      default:
         throw std::runtime_error{"Invalid value for error criterion."};
         break;
      }
      return retval;
   }

   // Implementation Note:
   //   The use of UtVec3d::operator+= was avoided after it was determined experimentally to impose
   //   a noticeable performance cost.

   void TakeStep(const WsfOrbitalDynamics& aDynamics, const ut::OrbitalState& aCurrentState)
   {
      // 1) Compute the various RHS contributions.
      // This method is First Same as Last (FSAL), so the saved velocity and acceleration
      // give rhs[0].
      auto& osv        = aCurrentState.GetOrbitalStateVector();
      mRHS_Position[0] = osv.GetVelocity();
      mRHS_Velocity[0] = aCurrentState.GetAccelerationInertial();

      for (unsigned int i = 1; i < Integrator::cSTEPCOUNT; ++i)
      {
         // Form the state based on prior prediction steps
         mY_Position = osv.GetLocation();
         mY_Velocity = osv.GetVelocity();
         for (unsigned int j = 0; j < i; ++j)
         {
            double factor = mStepSize * Integrator::cAVALUES[i][j];

            mY_Position[0] += mRHS_Position[j][0] * factor;
            mY_Position[1] += mRHS_Position[j][1] * factor;
            mY_Position[2] += mRHS_Position[j][2] * factor;
            mY_Velocity[0] += mRHS_Velocity[j][0] * factor;
            mY_Velocity[1] += mRHS_Velocity[j][1] * factor;
            mY_Velocity[2] += mRHS_Velocity[j][2] * factor;
         }

         // Get the prediction time
         UtCalendar predTime{aCurrentState.GetEpoch()};
         predTime.AdvanceTimeBy(Integrator::cCVALUES[i] * mStepSize);

         // Compute the RHS at the predicted time and location
         mRHS_Position[i] = mY_Velocity;
         mRHS_Velocity[i] =
            aDynamics.ComputeAcceleration(mPropagatorPtr->GetDynamicalMass(), predTime, mY_Position, mY_Velocity);
      }

      // 2) Compute both predictions
      mPredictedPosition = osv.GetLocation();
      mPredictedVelocity = osv.GetVelocity();
      mPosDiff.Set(0.0);
      mVelDiff.Set(0.0);

      for (unsigned int i = 0; i < Integrator::cSTEPCOUNT; ++i)
      {
         double bFactor      = mStepSize * Integrator::cBVALUES[i];
         double bErrorFactor = mStepSize * Integrator::cERRORVALUES[i];

         mPredictedPosition[0] += mRHS_Position[i][0] * bFactor;
         mPredictedPosition[1] += mRHS_Position[i][1] * bFactor;
         mPredictedPosition[2] += mRHS_Position[i][2] * bFactor;
         mPredictedVelocity[0] += mRHS_Velocity[i][0] * bFactor;
         mPredictedVelocity[1] += mRHS_Velocity[i][1] * bFactor;
         mPredictedVelocity[2] += mRHS_Velocity[i][2] * bFactor;
         mPosDiff[0] += mRHS_Position[i][0] * bErrorFactor;
         mPosDiff[1] += mRHS_Position[i][1] * bErrorFactor;
         mPosDiff[2] += mRHS_Position[i][2] * bErrorFactor;
         mVelDiff[0] += mRHS_Velocity[i][0] * bErrorFactor;
         mVelDiff[1] += mRHS_Velocity[i][1] * bErrorFactor;
         mVelDiff[2] += mRHS_Velocity[i][2] * bErrorFactor;
      }
   }

   void AdjustTimeStep(double aError)
   {
      if (aError > mTolerance)
      {
         mStepSize *= 0.9 * pow(mTolerance / aError, 1.0 / (Integrator::cORDER - 1.0));
      }
      else
      {
         mStepSize *= 0.9 * pow(mTolerance / aError, 1.0 / Integrator::cORDER);
      }
      if (fabs(mStepSize) > mMaxStepSize)
      {
         if (mStepSize > 0.0)
         {
            mStepSize = mMaxStepSize;
         }
         else
         {
            mStepSize = -mMaxStepSize;
         }
      }
      if (fabs(mStepSize) < mMinStepSize)
      {
         if (!mWarned)
         {
            auto warn = ut::log::warning() << "Timestep limited by minimum step size.";
            warn.AddNote() << "Integration will proceed, but will be outside error tolerance";
            warn.AddNote() << "Adjusted timestep: " << mStepSize;
            warn.AddNote() << "Minimum timestep: " << mMinStepSize;
            warn.AddNote() << "Integrator: " << Integrator::cTYPE;
            mWarned = true;
         }
         if (mStepSize > 0.0)
         {
            mStepSize = mMinStepSize;
         }
         else
         {
            mStepSize = -mMinStepSize;
         }
      }
   }

   void AdvanceState(ut::OrbitalState& aCurrentState)
   {
      auto epoch = aCurrentState.GetEpoch();
      epoch.AdvanceTimeBy(mStepSize);
      aCurrentState.Set(epoch, ut::OrbitalStateVector{mPredictedPosition, mPredictedVelocity});
      aCurrentState.SetAccelerationInertial(mRHS_Velocity[Integrator::cSTEPCOUNT - 1]);
   }

   double                         mStepSize{-1.0};
   double                         mTolerance{1.0e-10};
   double                         mMaxStepSize{std::numeric_limits<double>::max()};
   double                         mMinStepSize{0.0};
   double                         mInitialStepSize{0.1};
   unsigned int                   mMaxAdjustmentAttempts{50};
   ErrorCriterion                 mErrorCriterion{ErrorCriterion::cL_TWO_NORM};
   std::array<UtVec3d, StepCount> mRHS_Position{};
   std::array<UtVec3d, StepCount> mRHS_Velocity{};
   UtVec3d                        mY_Position{};
   UtVec3d                        mY_Velocity{};
   UtVec3d                        mPredictedPosition{};
   UtVec3d                        mPredictedVelocity{};
   UtVec3d                        mPosDiff{};
   UtVec3d                        mVelDiff{};
   bool                           mWarned{false};
};

#endif // WSFRUNGEKUTTAORBITALINTEGRATOR_HPP
