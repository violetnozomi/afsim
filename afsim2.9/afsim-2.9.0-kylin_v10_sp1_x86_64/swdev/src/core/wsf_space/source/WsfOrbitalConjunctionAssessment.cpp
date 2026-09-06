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

#include "WsfOrbitalConjunctionAssessment.hpp"

#include <algorithm>
#include <memory>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtGoldenSectionSearch.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfDateTime.hpp"
#include "WsfFilter.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackId.hpp"
#include "WsfTrackManager.hpp"

WsfOrbitalConjunctionAssessment::WsfOrbitalConjunctionAssessment(WsfLocalTrack&                 aPrimary,
                                                                 double                         aPrimarySize,
                                                                 WsfLocalTrack&                 aSecondary,
                                                                 double                         aSecondarySize,
                                                                 const Options&                 aOptions,
                                                                 const UtOrbitalPropagatorBase* aPropPtr)
   : mPrimary(aPrimary, aPrimarySize, aPropPtr)
   , mSecondary(aSecondary, aSecondarySize, aPropPtr)
   , mOptions(aOptions)
   , mStatus{Status::cNO_CONJUNCTION}
   , mEncounter{}
{
   mPrimary.SetConjunctionAssessment(this);
   mSecondary.SetConjunctionAssessment(this);

   mNext.mTime      = mOptions.mInitialSearchTime;
   mNext.mPrimary   = mPrimary.Propagate(mNext.mTime);
   mNext.mSecondary = mSecondary.Propagate(mNext.mTime);
   mCurrent         = mNext;

   if (!FastFilter())
   {
      mNext.mTime = mOptions.mFinalSearchTime;
      mStatus     = Status::cREACHED_FINAL_TIME;
   }
}

WsfOrbitalConjunctionAssessment::Status WsfOrbitalConjunctionAssessment::FindNext()
{
   mStatus = Status::cNO_CONJUNCTION;
   mEncounter.Reset();

   while (mStatus == Status::cNO_CONJUNCTION)
   {
      if (mNext.mTime < mOptions.mFinalSearchTime)
      {
         mCurrent = mNext;

         PredictNextState();
         double tMin = FindMinimum();
         if (tMin <= mNext.mTime)
         {
            double       combinedRadius     = mPrimary.Size() + mSecondary.Size();
            UtCovariance combinedCovariance = mPrimary.Covariance(tMin) + mSecondary.Covariance(tMin);
            mEncounter.Compute(mCurrent, mNext, combinedRadius, combinedCovariance, mOptions.mExclusionFactor);
            if (mEncounter.HasCloseEncounter())
            {
               mStatus = Status::cCONJUNCTION_FOUND;
            }
         }
      }
      else
      {
         mStatus = Status::cREACHED_FINAL_TIME;
      }
   }

   return mStatus;
}

WsfOrbitalConjunctionRecord WsfOrbitalConjunctionAssessment::CurrentConjunction() const
{
   WsfOrbitalConjunctionRecord retval = mEncounter.GetRecord();
   retval.mPrimary                    = mPrimary.GetName();
   retval.mSecondary                  = mSecondary.GetName();
   return retval;
}

WsfOrbitalConjunctionAssessment::Object::Object(WsfLocalTrack& aTrack, double aSize, const UtOrbitalPropagatorBase* aPropPtr)
   : mTrack(aTrack)
   , mSize{aSize}
   , mPropPtr(aPropPtr->Clone())
   , mConjPtr{nullptr}
{
   mSimStartTime = aTrack.GetTrackManager()->GetPlatform()->GetSimulation()->GetDateTime().GetStartDateAndTime();
   UtCalendar targetTime(mSimStartTime);
   targetTime.AdvanceTimeBy(mTrack.GetUpdateTime());

   UtECI_Conversion conversion(targetTime, aPropPtr->GetCentralPoint());

   double locTargetWCS[3];
   mTrack.GetLocationWCS(locTargetWCS);
   conversion.SetLocationWCS(locTargetWCS);
   UtVec3d locTargetECI = conversion.GetLocationECI();

   double velTargetWCS[3];
   mTrack.GetVelocityWCS(velTargetWCS);
   conversion.SetVelocityWCS(velTargetWCS);
   UtVec3d velTargetECI = conversion.GetVelocityECI();

   ut::OrbitalState initialState = mPropPtr->GetInitialOrbitalState();
   initialState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);
   initialState.Set(targetTime, ut::OrbitalState::Vector(locTargetECI, velTargetECI));
   mPropPtr->Initialize(initialState);
   mPropPtr->Update(targetTime);
}

double WsfOrbitalConjunctionAssessment::Object::EstimateTimeStep(double aTime, double aRadians)
{
   // This uses the instantaneous time derivative of the anomaly to estimate. This is a simple
   // Euler step.
   double denom = MeanMotion() * pow(SemiMajorAxis(), 2.0) * sqrt(1.0 - pow(Eccentricity(), 2.0));
   return OrbitRadiusSquared(aTime) * aRadians / denom;
}

double WsfOrbitalConjunctionAssessment::Object::Periapsis() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalElements().GetPeriapsisRadius();
}

double WsfOrbitalConjunctionAssessment::Object::Apoapsis() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalElements().GetApoapsisRadius();
}

double WsfOrbitalConjunctionAssessment::Object::Size() const
{
   double retval = mSize;
   if (retval <= 0.0)
   {
      retval = mConjPtr->DefaultObjectRadius();
   }
   return retval;
}

UtCovariance WsfOrbitalConjunctionAssessment::Object::Covariance(double aTime) const
{
   // NOTE: This should return a 6x6 state covariance for the full kinematic state of the
   // object. We are not interested in the velocity components of this covariance. We will
   // project out the velocity components when we use this covariance to form the ellipsoidal
   // function.

   UtCovariance retval{};
   WsfFilter*   filterPtr  = mTrack.GetFilter();
   bool         useDefault = true;
   if (filterPtr != nullptr)
   {
      if (filterPtr->GetPredictedStateCovariance(aTime, retval))
      {
         useDefault = false;
      }
   }

   if (useDefault)
   {
      // The default is a spherical error with variance given by the input.
      double var = mConjPtr->DefaultVariance();
      retval     = UtCovariance(6, 6);
      retval.FillDiagonal(var * var);
   }

   return retval;
}

WsfOrbitalConjunctionAssessment::Kinematics WsfOrbitalConjunctionAssessment::Object::Propagate(double aTime)
{
   UpdatePropagator(aTime);
   Kinematics               retval;
   ut::OrbitalState::Vector osvInertial = mPropPtr->GetOrbitalState().GetOrbitalStateVectorInertial();
   retval.mPosition                     = osvInertial.GetLocation();
   retval.mVelocity                     = osvInertial.GetVelocity();
   retval.mAcceleration                 = mPropPtr->GetOrbitalState().GetAccelerationInertial();
   return retval;
}

void WsfOrbitalConjunctionAssessment::Object::SetConjunctionAssessment(const WsfOrbitalConjunctionAssessment* aConjPtr)
{
   mConjPtr = aConjPtr;
}

std::string WsfOrbitalConjunctionAssessment::Object::GetName() const
{
   // Revisit this. This will give the 'Truth' name when it exists; this 'cheats'.
   WsfStringId        name = mTrack.GetTargetName();
   std::ostringstream ss{};
   if (!name.IsNull())
   {
      ss << name.GetString();
   }
   else
   {
      ss << "TRACK:" << mTrack.GetTrackId();
   }
   return ss.str();
}

double WsfOrbitalConjunctionAssessment::Object::OrbitRadiusSquared(double aTime)
{
   UpdatePropagator(aTime);
   UtVec3d loc = mPropPtr->GetOrbitalState().GetOrbitalStateVector().GetLocation();
   return loc.MagnitudeSquared();
}

double WsfOrbitalConjunctionAssessment::Object::Eccentricity() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalElements().GetEccentricity();
}

double WsfOrbitalConjunctionAssessment::Object::SemiMajorAxis() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalElements().GetSemiMajorAxis();
}

double WsfOrbitalConjunctionAssessment::Object::MeanMotion() const
{
   return mPropPtr->GetOrbitalState().GetOrbitalElements().GetMeanMotion();
}

void WsfOrbitalConjunctionAssessment::Object::UpdatePropagator(double aTime)
{
   UtCalendar targetTime(mSimStartTime);
   targetTime.AdvanceTimeBy(aTime);
   mPropPtr->Update(targetTime);
}

bool WsfOrbitalConjunctionAssessment::FastFilter()
{
   double rPeriMax = std::max(mPrimary.Periapsis(), mSecondary.Periapsis());
   double rApoMin  = std::min(mPrimary.Apoapsis(), mSecondary.Apoapsis());
   return rPeriMax - rApoMin <= mOptions.mCutRadius;
}

void WsfOrbitalConjunctionAssessment::PredictNextState()
{
   mNext.mTime += ComputeNextTime();
   mNext.mPrimary   = mPrimary.Propagate(mNext.mTime);
   mNext.mSecondary = mSecondary.Propagate(mNext.mTime);

   while (mNext.RelativePosition().Magnitude() < 4.0 * mOptions.mCutRadius)
   {
      mNext.mTime += 60.0;
      mNext.mPrimary   = mPrimary.Propagate(mNext.mTime);
      mNext.mSecondary = mSecondary.Propagate(mNext.mTime);
   }
}

double WsfOrbitalConjunctionAssessment::FindMinimum()
{
   double dInit       = mCurrent.DistanceFunction();
   double dDotInit    = mCurrent.DistanceFunctionPrime();
   double dDotDotInit = mCurrent.DistanceFunctionPrimePrime();
   double dFini       = mNext.DistanceFunction();
   double dDotFini    = mNext.DistanceFunctionPrime();
   double dDotDotFini = mNext.DistanceFunctionPrimePrime();

   // We interpolate the derivative of the distance function (the squared separation)
   // and look for zeros. These would then be the locations of minima or maxima of
   // the distance function. So we then check if the second derivative of the
   // distance function (the first derivative of the interpolant) is positive.
   auto f =
      UtQuinticSpline::TwoPoint(mCurrent.mTime, dInit, dDotInit, dDotDotInit, mNext.mTime, dFini, dDotFini, dDotDotFini);
   auto fPrime = f.Derivative();

   auto   zeros  = fPrime.Zeros(mCurrent.mTime, mNext.mTime);
   double retval = std::numeric_limits<double>::max();
   if (!zeros.empty())
   {
      auto fPrimePrime = fPrime.Derivative();
      for (double time : zeros)
      {
         if (fPrimePrime(time) > 0.0)
         {
            retval = time;
         }
      }
   }

   return retval;
}

double WsfOrbitalConjunctionAssessment::State::DistanceFunction() const
{
   return RelativePosition().DotProduct(RelativePosition());
}

double WsfOrbitalConjunctionAssessment::State::DistanceFunctionPrime() const
{
   return 2.0 * RelativeVelocity().DotProduct(RelativePosition());
}

double WsfOrbitalConjunctionAssessment::State::DistanceFunctionPrimePrime() const
{
   return (2.0 * RelativeVelocity().MagnitudeSquared() + 2.0 * RelativeAcceleration().DotProduct(RelativePosition()));
}

UtVec3d WsfOrbitalConjunctionAssessment::State::RelativePosition() const
{
   return mSecondary.mPosition - mPrimary.mPosition;
}

UtVec3d WsfOrbitalConjunctionAssessment::State::RelativeVelocity() const
{
   return mSecondary.mVelocity - mPrimary.mVelocity;
}

UtVec3d WsfOrbitalConjunctionAssessment::State::RelativeAcceleration() const
{
   return mSecondary.mAcceleration - mPrimary.mAcceleration;
}

WsfOrbitalConjunctionAssessment::Encounter::Encounter()
   : mRecord{std::string{}, std::string{}, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0}
{
}

void WsfOrbitalConjunctionAssessment::Encounter::Reset()
{
   mRecord.mMinTime          = -1.0;
   mRecord.mStartTime        = -1.0;
   mRecord.mEndTime          = -1.0;
   mRecord.mMaxProbability   = -1.0;
   mRecord.mMissDistance     = -1.0;
   mRecord.mRelativeVelocity = -1.0;
}

void WsfOrbitalConjunctionAssessment::Encounter::Compute(const State&        aCurr,
                                                         const State&        aNext,
                                                         double              aCombinedRadius,
                                                         const UtCovariance& aCombinedCovariance,
                                                         double              aScaleFactor)
{
   auto path                 = UtQuinticSpline::TwoPoint(aCurr.mTime,
                                         aCurr.RelativePosition(),
                                         aCurr.RelativeVelocity(),
                                         aCurr.RelativeAcceleration(),
                                         aNext.mTime,
                                         aNext.RelativePosition(),
                                         aNext.RelativeVelocity(),
                                         aNext.RelativeAcceleration());
   mRecord.mMinTime          = RecomputeMinTime(path, aCurr.mTime, aNext.mTime);
   mRecord.mMissDistance     = path(mRecord.mMinTime).Magnitude();
   mRecord.mRelativeVelocity = path.Derivative()(mRecord.mMinTime).Magnitude();

   ComputeProbability(path, aCombinedRadius);

   UtCovariance scaledCovariance(aCombinedCovariance);
   scaledCovariance = scaledCovariance * aScaleFactor * aScaleFactor;
   ComputeEncounterTimes(path, aCurr.mTime, aNext.mTime, mRecord.mMinTime, scaledCovariance);
}

void WsfOrbitalConjunctionAssessment::Encounter::ComputeProbability(const UtInterpolatingPolynomial<UtVec3d, 5>& aPath,
                                                                    double aCombinedRadius)
{
   double rScaled = aCombinedRadius / mRecord.mMissDistance;
   if (rScaled < 1.0)
   {
      // See Vallado (11-56) p. 932.
      double sqrtarg = -log((1.0 - rScaled) / (1.0 + rScaled));
      double sfactor = sqrt(sqrtarg);
      double erfarg  = sfactor / (2.0 * sqrt(rScaled));
      mRecord.mMaxProbability =
         0.5 * (UtMath::ErrorFunction((rScaled + 1.0) * erfarg) + UtMath::ErrorFunction((rScaled - 1.0) * erfarg));
   }
   else
   {
      // The miss distance is smaller than the object sizes; a conjunction is likely.
      mRecord.mMaxProbability = 1.0;
   }
}

void WsfOrbitalConjunctionAssessment::Encounter::ComputeEncounterTimes(const UtInterpolatingPolynomial<UtVec3d, 5>& aPath,
                                                                       double        aCurrTime,
                                                                       double        aNextTime,
                                                                       double        aMinTime,
                                                                       UtCovariance& aCombinedCovariance)
{
   double covarX = aCombinedCovariance(0, 0);
   double covarY = aCombinedCovariance(1, 1);
   double covarZ = aCombinedCovariance(2, 2);

   // The ellipsoidal function helps determine the encounter times.
   aCombinedCovariance.Invert(); // The inverse of the combined covariance matrix gives the ellipsoid
   auto ellipsoidalFunction = [&aPath, &aCombinedCovariance](double aTime) -> double
   {
      UtVec3d separation = aPath(aTime);

      // NOTE that this actually projects out the velocity components. sepData is initialized
      // to zero in the next line, and the following Get() will only pull the first three
      // components.
      double sepData[6]{};
      separation.Get(sepData);
      UtMatrixd sepVec(6);
      sepVec.CopyFrom(sepData);
      UtMatrixd sepVecTrans{};
      sepVecTrans.Transpose(sepVec);

      return (sepVecTrans * aCombinedCovariance * sepVec)(0, 0) - 1.0;
   };

   // Reduce the range of times over which we are interpolating the ellipsoidal function.
   aCurrTime = ContractTowardRoot(ellipsoidalFunction, mRecord.mMinTime, aCurrTime);
   aNextTime = ContractTowardRoot(ellipsoidalFunction, mRecord.mMinTime, aNextTime);

   // Find the middle interpolation times.
   double thirdTime;
   double fourthTime;
   if (mRecord.mMinTime > (aCurrTime + aNextTime) / 2.0)
   {
      thirdTime  = (mRecord.mMinTime + aCurrTime) / 2.0;
      fourthTime = mRecord.mMinTime;
   }
   else
   {
      thirdTime  = mRecord.mMinTime;
      fourthTime = (mRecord.mMinTime + aNextTime) / 2.0;
   }

   auto ellipsoidInterp = UtCubicSpline::FourPoint(aCurrTime,
                                                   ellipsoidalFunction(aCurrTime),
                                                   thirdTime,
                                                   ellipsoidalFunction(thirdTime),
                                                   fourthTime,
                                                   ellipsoidalFunction(fourthTime),
                                                   aNextTime,
                                                   ellipsoidalFunction(aNextTime));
   auto zeros           = ellipsoidInterp.Zeros(aCurrTime, aNextTime);
   switch (zeros.size())
   {
   case 0:
      // The interpolant does not intersect the ellipsoid, even though there
      // is a minimum in the relative separation of the two objects. This means
      // that either the minimum is not inside the error ellipsoid, or the entire
      // length of the interpolant is inside the ellipsoid (i.e., the platforms
      // are on nearly the same orbit).
      {
         UtVec3d minSeparation = aPath(aMinTime);
         if (minSeparation[0] * minSeparation[0] <= covarX && minSeparation[1] * minSeparation[1] <= covarY &&
             minSeparation[2] * minSeparation[2] <= covarZ)
         {
            mRecord.mStartTime = aCurrTime;
            mRecord.mEndTime   = aNextTime;
         }
         else
         {
            mRecord.mStartTime = std::numeric_limits<double>::max();
            mRecord.mEndTime   = std::numeric_limits<double>::min();
         }
      }
      break;
   case 1:
      // Only a single zero in the range indicates that the timestep was just lucky
      // (or unlucky) enough to have an endpoint close to the time of conjunction.
      // We take the approach that we use the one root to generate the other by
      // reflection across the minimum time. Note that this assumes that the path enters
      // the ellipsoid before leaving it, as is reasonable.
      if (zeros[0] - mRecord.mMinTime > 0)
      {
         mRecord.mEndTime   = zeros[0];
         mRecord.mStartTime = mRecord.mMinTime - (zeros[0] - mRecord.mMinTime);
      }
      else
      {
         mRecord.mStartTime = zeros[0];
         mRecord.mEndTime   = mRecord.mMinTime - (zeros[0] - mRecord.mMinTime);
      }
      break;
   case 2:
      for (double root : zeros)
      {
         double deriv = ellipsoidInterp.Derivative()(root);
         if (deriv > 0.0)
         {
            mRecord.mEndTime = root;
         }
         else // deriv < 0.0
         {
            mRecord.mStartTime = root;
         }
      }
      break;
   case 3:
      // If there are three intersections with the ellipsoid, some assumption of this
      // analysis is surely broken.
      assert(0 && "Impossible: More than two intersections.");
      break;
   default:
      assert(0 && "Impossible: More than three roots in a cubic.");
      break;
   }
}

double WsfOrbitalConjunctionAssessment::Encounter::RecomputeMinTime(const UtInterpolatingPolynomial<UtVec3d, 5>& aFunction,
                                                                    double aLowRange,
                                                                    double aHighRange)
{
   auto func = [&aFunction](double aTime) -> double { return aFunction(aTime).Magnitude(); };

   return UtGoldenSectionSearch(func, aLowRange, aHighRange, 1.0e-6);
}

double WsfOrbitalConjunctionAssessment::ComputeNextTime()
{
   double tNextPrimary   = mPrimary.EstimateTimeStep(mCurrent.mTime, mOptions.mStepRadians);
   double tNextSecondary = mSecondary.EstimateTimeStep(mCurrent.mTime, mOptions.mStepRadians);
   return std::min(mOptions.mFinalSearchTime, std::min(tNextPrimary, tNextSecondary));
}
