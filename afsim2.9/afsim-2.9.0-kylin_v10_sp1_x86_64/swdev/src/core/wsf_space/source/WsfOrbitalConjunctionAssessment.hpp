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

#ifndef WSFORBITALCONJUNCTIONASSESSMENT_HPP
#define WSFORBITALCONJUNCTIONASSESSMENT_HPP

#include "UtCalendar.hpp"
class UtCovariance;
#include "UtException.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtSpline.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"

//! The data recorded when a possible conjunction is predicted.
struct WsfOrbitalConjunctionRecord
{
   std::string mPrimary;          //! A string identifier for the primary object.
   std::string mSecondary;        //! A string identifier for the secondary object.
   double      mMinTime;          //! The time of closest approach.
   double      mStartTime;        //! The start time of the encounter.
   double      mEndTime;          //! The end time of the encounter.
   double      mMaxProbability;   //! The worst-case probability estimate for a conjunction to occur.
   double      mMissDistance;     //! The predicted miss distance between the two objects.
   double      mRelativeVelocity; //! The relative velocity at the time of closest approach.
};

//! An object to search for possible conjunctions between two spacecraft.
class WsfOrbitalConjunctionAssessment
{
public:
   //! Specifies the options to use when performing the analysis.
   struct Options
   {
      double mCutRadius;       //! The distance cut in the periapsis-apoapsis fast cut.
      double mStepRadians;     //! The step size in degrees to take during prediction.
      double mExclusionFactor; //! The scaled size of the covariance ellipsoid defining the
      //!   exclusion zone.
      double mInitialSearchTime;   //! The initial search time.
      double mFinalSearchTime;     //! The time over which to search for close approaches.
      double mDefaultObjectRadius; //! The default object radius.
      double mDefaultVariance;     //! The default variance.
   };

   //! This indicates if a conjunction has been found.
   enum class Status
   {
      cNO_CONJUNCTION = 0,
      cCONJUNCTION_FOUND,
      cREACHED_FINAL_TIME
   };

   WsfOrbitalConjunctionAssessment(WsfLocalTrack&                 aPrimary,
                                   double                         aPrimarySize,
                                   WsfLocalTrack&                 aSecondary,
                                   double                         aSecondarySize,
                                   const Options&                 aOptions,
                                   const UtOrbitalPropagatorBase* aPropPtr);

   //! Find the next possible conjunction.
   //! This will search for the next conjunction for the two objects. After each call
   //! to the routine, the state of the assessment will advance to the next conjunction
   //! (if one is found) or the end of the interval if no further conjunctions are found.
   //! If the end of the interval is reached, this will return Status::cREACHED_FINAL_TIME.
   //! If a conjunction is found, this will return Status::cCONJUNCTION_FOUND.
   //! In the event a conjunction is found, CurrentConjunction() can be used to
   //! retrieve the encounter data.
   //! @returns - Status::cCONJUNCTION_FOUND if a conjunction was found before the end
   //!            of the search interval; Status::cREACHED_FINAL_TIME otherwise.
   Status FindNext();

   //! Returns if the assessment has reached the end of the evaluation interval.
   bool ReachedFinalTime() const { return mStatus == Status::cREACHED_FINAL_TIME; }

   //! Returns the current conjunction.
   WsfOrbitalConjunctionRecord CurrentConjunction() const;

   //! Return the default variance set when this object was constructed.
   double DefaultVariance() const { return mOptions.mDefaultVariance; }

   //! Return the default object radius set when this object was constructed.
   double DefaultObjectRadius() const { return mOptions.mDefaultObjectRadius; }

private:
   //! Collection of kinematic data used during assessment.
   struct Kinematics
   {
      UtVec3d mPosition;
      UtVec3d mVelocity;
      UtVec3d mAcceleration;
   };

   //! The state of the two objects at a given time.
   struct State
   {
      double  DistanceFunction() const;
      double  DistanceFunctionPrime() const;
      double  DistanceFunctionPrimePrime() const;
      UtVec3d RelativePosition() const;
      UtVec3d RelativeVelocity() const;
      UtVec3d RelativeAcceleration() const;

      double     mTime;
      Kinematics mPrimary;
      Kinematics mSecondary;
   };

   class Encounter
   {
   public:
      Encounter();

      //! Reset the encounter to contain sentinel values indicating no solution.
      void Reset();

      //! Compute the details of the close encounter.
      void Compute(const State&        aCurr,
                   const State&        aNext,
                   double              aCombinedRadius,
                   const UtCovariance& aCombinedCovariance,
                   double              aScaleFactor);

      //! Return if this encounter represents a close encounter.
      bool HasCloseEncounter() const { return mRecord.mStartTime < mRecord.mEndTime; }

      //! Return the encounter record.
      WsfOrbitalConjunctionRecord GetRecord() const { return mRecord; }

   private:
      void          ComputeProbability(const UtInterpolatingPolynomial<UtVec3d, 5>& aPath, double aCombinedRadius);
      void          ComputeEncounterTimes(const UtInterpolatingPolynomial<UtVec3d, 5>& aPath,
                                          double                                       aCurrTime,
                                          double                                       aNextTime,
                                          double                                       aMinTime,
                                          UtCovariance&                                aCombinedCovariance);
      static double RecomputeMinTime(const UtInterpolatingPolynomial<UtVec3d, 5>& aFunction,
                                     double                                       aLowRange,
                                     double                                       aHighRange);

      //! This will contract the given limit toward the fixed point.
      //! This method takes any callable object taking a double and returning a double which evaluates
      //! to a negative value at @p aFixed, and a positive value at @aLimit. This function will return
      //! a value of the parameter to the provided function that is closer to @p aFixed, but which still
      //! gives a positive value when supplied as argument to @p aFunction. There is no ordering
      //! requirement for @p aFixed and @p aLimit. If @p aFunction evaluated at @p aLimit and @p aFixed
      //! give the same sign, then this function will return @p aLimit.
      //! \param aFunction - A callable object taking a double and returning a double.
      //! \param aFixed    - The fixed parameter to the function.
      //! \param aLimit    - The limiting parameter to the function.
      //! \returns         - A new limiting value that is closer to @p aFixed but for which @p aFunction
      //!                    still evaluates to a positive value.
      template<typename Callable>
      static double ContractTowardRoot(Callable aFunction, double aFixed, double aLimit)
      {
         if (aFunction(aFixed) >= 0.0)
         {
            return aLimit;
         }
         double delta = (aLimit - aFixed) / 2.0;
         double xTest = aFixed + delta;
         while (aFunction(xTest) > 0.0)
         {
            delta /= 2.0;
            xTest = aFixed + delta;
         }
         return xTest + 2.0 * delta;
      }

      WsfOrbitalConjunctionRecord mRecord;
   };

   //! An encapsulation of the representation of known information about the tracked object.
   class Object
   {
   public:
      Object(WsfLocalTrack& aTrack, double aSize, const UtOrbitalPropagatorBase* aPropPtr);

      //! Estimate the time to traverse the given angle.
      double EstimateTimeStep(double aTime, double aRadians);

      //! Return the periapsis of the object's predicted orbit.
      double Periapsis() const;

      //! Return the apoapsis of the object's predicted orbit.
      double Apoapsis() const;

      //! Return the object's size.
      double Size() const;

      //! Return the covariance of the object at the given time.
      UtCovariance Covariance(double aTime) const;

      //! Return the predicted kinematic state of the object for the given time.
      Kinematics Propagate(double aTime);

      //! Set the conjunction assessment object to which this is related.
      void SetConjunctionAssessment(const WsfOrbitalConjunctionAssessment* aConjPtr);

      //! Return a text identifier for the represented object.
      std::string GetName() const;

   private:
      double OrbitRadiusSquared(double aTime);
      double Eccentricity() const;
      double SemiMajorAxis() const;
      double MeanMotion() const;
      void   UpdatePropagator(double aTime);

      WsfLocalTrack&                           mTrack;        //! The track representing the object.
      UtCalendar                               mSimStartTime; //! The simulation's start time (for propagation).
      double                                   mSize;         //! The size of the object.
      std::unique_ptr<UtOrbitalPropagatorBase> mPropPtr;      //! A propagator used for state prediction.
      const WsfOrbitalConjunctionAssessment*   mConjPtr;      //! This object's assessment.
   };

   bool   FastFilter();
   void   PredictNextState();
   double FindMinimum();
   double ComputeNextTime();

   Object mPrimary;   //! The primary object.
   Object mSecondary; //! The secondary object.

   Options mOptions; //! The assessment options.

   State mCurrent; //! The current encounter state.
   State mNext;    //! The next encounter state.

   Status    mStatus;    //! The current status of the computation.
   Encounter mEncounter; //! The current encounter object.
};

#endif // WSFORBITALCONJUNCTIONASSESSMENT_HPP
