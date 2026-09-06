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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTORBITALPROPAGATOR_HPP
#define UTORBITALPROPAGATOR_HPP

#include "ut_export.h"

#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
class UtTwoLineElement;

//! UtOrbitalPropagator is a simple, mean anomaly propagator.
//! It advances a body in an elliptical orbit according to the
//! given epoch (see UtCalendar).
class UT_EXPORT UtOrbitalPropagator : public UtOrbitalPropagatorBase
{
public:
   UtOrbitalPropagator() = default;
   explicit UtOrbitalPropagator(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr);
   UtOrbitalPropagator(const UtOrbitalPropagator& aSrc) = default;
   UtOrbitalPropagator& operator                        =(const UtOrbitalPropagator& aRhs);
   ~UtOrbitalPropagator() override                      = default;

   UtOrbitalPropagator* Clone() const override { return new UtOrbitalPropagator(*this); }
   bool                 Initialize(const UtCalendar& aTime) override;
   using UtOrbitalPropagatorBase::Initialize;

   void SetPrecession(bool aSetPrecession);

   //! Set the angular tolerance to be used for this propagator.  This is the difference between the
   //! approximated true anomaly and the computed eccentric anomaly.  The lower the tolerance the more iterations
   //! it will take to achieve.
   //!@note The default is 1.0e-8.
   void SetTolerance(double aTolerance) { mTolerance = aTolerance; }

   //! Set the maximum number of iterations to achieve the tolerance (see above) between the approximated true anomaly
   //! and the computed eccentric anomaly.  If the maximum number of iterations is set too low, the tolerance will not
   //! be achieved.
   //!@note: The default value is 1000 iterations.
   void SetMaxIterations(unsigned aMaxIterations) { mMaxIterations = aMaxIterations; }

   //! Set the linear tolerance to be used for this propagator.  The computed satellite location will be precise
   //! to within the limit specified.
   //!@note If set this parameter is used to set the angular tolerance.
   void SetLinearTolerance(double aLinearTolerance) { mLinearTolerance = aLinearTolerance; }

   //! @name Utility Methods, optionally supported in derived classes.
   //@{
   double GetTimeToPeriapsisPassage(unsigned aOrbitNum = 0) const override;
   double GetTimeToApoapsisPassage(unsigned aOrbitNum = 0) const override;
   double GetTimeToAscendingNodePassage(unsigned aOrbitNum = 0) const override;
   double GetTimeSinceAscendingNodePassage(unsigned aOrbitNum) const override;
   double GetTimeSinceDescendingNodePassage(unsigned aOrbitNum) const override;
   double GetTimeToDescendingNodePassage(unsigned aOrbitNum = 0) const override;
   bool   GetTimeToAscendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber = 0) const override;
   bool   GetTimeToDescendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber = 0) const override;
   double GetTimeToNorthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const override;
   double GetTimeToSouthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const override;
   //@}

   UtLambertProblem::Result SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                const UtVec3d&         aLocationInertial_1,
                                                const UtVec3d&         aLocationInertial_2,
                                                double                 aDt,
                                                bool                   aShortWay,
                                                bool                   aAllowHitsEarth,
                                                double                 aConvergenceTolerance = 1.0e-12) const override;

   static bool GetMeanAnomaliesOfIntersection(const UtOrbitalElements& aInitialOE,
                                              double                   aFinalRAAN,
                                              double                   aFinalInclination,
                                              double&                  aMA1,
                                              double&                  aMA2);

protected:
   void Propagate(const UtCalendar& aTime) override; // Propagate in this step, then use accessors to get data.

   void UpdateOrbitalState() override;

   bool HyperbolicPropagationAllowed() const override { return false; }

   //! Return the orbital elements referenced to the true-of-date equinox.
   const UtOrbitalElements& GetOrbitalElements() const { return GetOrbitalState().GetOrbitalElementsTOD(); }

   //! Perturbation that models the first order gravity term for oblate earth.
   class J2_Perturbation : public Perturbation
   {
   public:
      J2_Perturbation()                            = default;
      J2_Perturbation(const J2_Perturbation& aSrc) = default;
      ~J2_Perturbation() override                  = default;

      Perturbation* Clone() const override { return new J2_Perturbation(*this); }
      bool          Initialize(UtOrbitalPropagatorBase& aPropagator) override;
      bool          AddPerturbation(ut::OrbitalState& aState) override;

   private:
      void ComputeJ2Perturbation(const ut::OrbitalState& aState);
      void AdvanceOrbitalElements(ut::OrbitalState& aState);

      bool   mInitialized{false};
      double mLastUpdateTime{0.0};
      double mRAAN_Dot{0.0};
      double mArgPeriapsisDot{0.0};
      double mMeanAnomalyDot{0.0};
   };

private:
   void PropagateTrueAnomalyP();

   ut::CloneablePtr<ut::OrbitalState> mPropagatedOrbitalStatePtr{
      nullptr}; //! The orbital state that is propagated forward in time

   double   mAngularMomentum{0.0};
   double   mSpeed{0.0};
   double   mLastTrueAnomaly{0.0};
   double   mRadius{0.0}; // distance from focus to satellite
   double   mTolerance{1.0E-8};
   double   mLinearTolerance{0.0};
   unsigned mMaxIterations{1000};
   double   mStartMeanAnomaly{0.0};
   double   mCosTrueAnomaly{0.0};
   double   mSinTrueAnomaly{0.0};
   double   mSMA_Times_1_Minus_e_Squared{0.0};
};

#endif
