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

#ifndef UTORBITALPROPAGATORBASE_HPP
#define UTORBITALPROPAGATORBASE_HPP

#include "ut_export.h"

#include <vector>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtCloneablePtr.hpp"
#include "UtEclipse.hpp"
#include "UtMemory.hpp"
class UtInput;
#include "UtLambertProblem.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"

//! UtOrbitalPropagatorBase is an interface for orbital propagators.  It contains a list of orbital elements,
//! location, velocity, and acceleration.  Child classes must implement the Propagate() method
//! that updates the attributes for location, velocity, acceleration, and orbital elements.
//! The initial orbital elements object must be provided to the constructor; doing so provides
//! the flexibility to utilize derived orbital element types in other propagators
//! (the object need not be initialized, and it does not need to be configured if we utilize ProcessInput instead).
//! Clients should call the Update() method while running.
class UT_EXPORT UtOrbitalPropagatorBase
{
public:
   //! Default constructor; orbital state to be provided at initialization.
   UtOrbitalPropagatorBase();
   explicit UtOrbitalPropagatorBase(std::unique_ptr<ut::OrbitalState> aInitialOrbitalStatePtr);
   UtOrbitalPropagatorBase(const UtOrbitalPropagatorBase&) = default;
   UtOrbitalPropagatorBase& operator=(const UtOrbitalPropagatorBase& Rhs)   = default;
   bool                     operator==(const UtOrbitalPropagatorBase& aRhs) = delete;
   virtual ~UtOrbitalPropagatorBase()                                       = default;

   virtual bool ProcessInput(UtInput& aInput);
   virtual bool Initialize(const UtCalendar& aEpoch);
   bool         Initialize(const ut::OrbitalState& aInitialState);


   bool InitializeOrbitalState(const UtCalendar& aEpoch);

   void               Update(const UtCalendar& aTime);
   UT_DEPRECATED void Update(double aTimePastEpoch);

   virtual UtOrbitalPropagatorBase* Clone() const = 0;

   //! @name Eclipse Methods (not valid for heliocentric)
   //@{
   // TODO Eclipse methods should be generic for the central body
   // (earth-sun, moon-earth).  Currently they are Earth-centric.
   UtEclipse::EclipsedState GetEclipsedState() const;
   bool                     GetEclipseTimes(const UtCalendar& aTime, double& aTimeToEntry, double& aTimeToExit) const;
   bool                     GetEclipseTimes(double& aTimeToEntry, double& aTimeToExit) const;
   bool GetEclipseAnomalies(const UtCalendar& aTime, double& aEntryAnomaly, double& aExitAnomaly) const;
   bool GetEclipseAnomalies(double& aEntryAnomaly, double& aExitAnomaly) const;
   //@}

   //! @name methods related to the initial and/or propagated orbital states.
   //@{
   //! Return the propagated orbital state.
   const ut::OrbitalState& GetOrbitalState() const;

   //! Return the initial orbital state.
   const ut::OrbitalState& GetInitialOrbitalState() const { return *mInitialOrbitalStatePtr; }

   virtual bool SetInitialOrbitalState(const ut::OrbitalState& aOrbitalState);

   //! Return whether hyperbolic propagation (including the limiting parabolic case)
   //! is allowed by this propagator.
   virtual bool HyperbolicPropagationAllowed() const = 0;

   //! Return the current central body.
   const ut::CentralPoint& GetCentralPoint() const { return mInitialOrbitalStatePtr->GetCentralPoint(); }
   const ut::CentralBody&  GetCentralBody() const;

   //! Set the initial central body.
   //! @note This is needed to set the polar offsets properly, if used.
   //! This should be called prior to initialize().
   UT_DEPRECATED void SetInitialCentralBody(const ut::CentralBody& aCentralBody)
   {
      SetInitialCentralPoint(aCentralBody);
   }
   void SetInitialCentralPoint(const ut::CentralPoint& aCentralPoint);
   //@}

   //! Return the current absolute time.
   //!@note That this returns a reference, which will change if Update() is later called;
   //! be sure to copy the current time if you don't want it to change!
   const UtCalendar& GetCurrentTime() const { return mCurrentTime; }

   //! Return the time of the previous update of this propagator.
   const UtCalendar& GetLastUpdateTime() const { return mLastUpdateTime; }

   //! Return the current time past satellite epoch.
   //! @note This time will generally not be the same as the start time of the simulation.
   double GetTimePastEpoch() const { return mTimePastEpoch; }

   //! @name Utility Methods, optionally supported in derived classes.
   //@{
   virtual double GetTimeToPeriapsisPassage(unsigned aOrbitNum = 0) const;
   virtual double GetTimeToApoapsisPassage(unsigned aOrbitNum = 0) const;
   virtual double GetTimeToAscendingNodePassage(unsigned aOrbitNum = 0) const;
   virtual double GetTimeSinceAscendingNodePassage(unsigned aOrbitNum) const;
   virtual double GetTimeSinceDescendingNodePassage(unsigned aOrbitNum) const;
   virtual double GetTimeToDescendingNodePassage(unsigned aOrbitNum = 0) const;
   virtual bool   GetTimeToAscendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber = 0) const;
   virtual bool   GetTimeToDescendingRadius(double aRadius, double& aTime, unsigned aOrbitNumber = 0) const;
   virtual double GetTimeToNorthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const;
   virtual double GetTimeToSouthernIntersection(double aTargetRAAN, double aTargetInclination, unsigned aOrbitNum = 0) const;
   //@}

   //! Return a propagator that can be used to compute maneuvering operations.
   //! Some propagators might not be able to simply compute the required changes in kinematics for
   //! maneuvers. This method allows a propagator to provide an alternate propagator that is used
   //! to compute these changes. The simplest example of this is to return an instance of a
   //! UtOrbitalPropagator that has the same orbital elements as this propagator has osculating
   //! elements.
   //! @returns - a propagator that can be used for determining changes in velocity for maneuvers.
   virtual std::unique_ptr<UtOrbitalPropagatorBase> GetManeuveringPropagator() const;

   //! @name Methods with default implementations to catch special cases used in maneuvering.
   //! These methods may be re-implemented to reflect differing tolerances among propagators.
   //@{
   virtual bool OrbitIsCircular() const;
   virtual bool OrbitIsEquatorial() const;
   virtual bool OrbitIsPrograde() const;
   //@}

   // @name Descriptor / Designator methods (see UtOrbitalElements).
   //@{
   const std::string& GetDescriptor() const;
   const std::string& GetDesignator() const;
   //@}

   // TODO evaluate whether this is the best place for this method (new class?)
   virtual UtLambertProblem::Result SolveLambertProblem(const ut::CentralBody& aCentralBody,
                                                        const UtVec3d&         aLocationInertial_1,
                                                        const UtVec3d&         aLocationInertial_2,
                                                        double                 aDt,
                                                        bool                   aShortWay,
                                                        bool                   aAllowHitsEarth,
                                                        double                 aConvergenceTolerance = 1.0e-12) const;

   // TODO rename or remove
   void SetDebugOutputOE(bool aDebugOutputOE) { mDebugOutputOE = aDebugOutputOE; }

   friend class Perturbation;

   //! Base class for implementation of orbital perturbations not covered by
   //! setting the orbital elements directly.
   //! The implied method to perturb the orbit is through use of moments in the
   //! orbital elements, but another method could also be implemented.
   class Perturbation
   {
   public:
      Perturbation()                         = default;
      Perturbation(const Perturbation& aSrc) = default;
      virtual ~Perturbation()                = default;
      virtual bool Initialize(UtOrbitalPropagatorBase& aPropagator)
      {
         mPropagatorPtr = &aPropagator;
         return true;
      }
      virtual Perturbation* Clone() const                             = 0;
      virtual bool          AddPerturbation(ut::OrbitalState& aState) = 0;

      UtOrbitalPropagatorBase&       GetPropagator() { return *mPropagatorPtr; }
      const UtOrbitalPropagatorBase& GetPropagator() const { return *mPropagatorPtr; }

   protected:
      UtOrbitalPropagatorBase* mPropagatorPtr{nullptr};
   };

   void SetPerturbation(std::unique_ptr<Perturbation> aPerturbationPtr);
   void PrintDebug(double aTimePastEpoch);

   //! Return the perturbation object associated with this propagator, or nullptr if none exists.
   Perturbation* GetPerturbation() const { return mPerturbationPtr; }

protected:
   //! Update the propagated state (typically a ut::OrbitalState object).
   //! Implement this method in child classes to perform actual satellite propagation.
   virtual void Propagate(const UtCalendar& aTime) = 0;

   //! Provide a valid output orbital state.
   //!@note Providing the override for this method typically involves
   //! calling UtOrbitalPropagatorBase::SetOrbitalState with an argument
   //! of the derived propagator's internally propagated state (see UtOrbitalPropagator).
   virtual void UpdateOrbitalState() = 0;

   virtual void PostPropagate();

   virtual bool InitializeP(const UtCalendar& aEpoch);

   virtual void UpdateTime(const UtCalendar& aTime);

   void SetOrbitalState(const ut::OrbitalState& aState);


private:
   static bool EvaluateDebugInput(UtInput& aInput);

   void UpdateTime(double aTimePastEpoch); // deprecated version of UpdateTime, above

   ut::CloneablePtr<ut::OrbitalState> mInitialOrbitalStatePtr; //! The orbital state at the initial epoch.
   ut::CloneablePtr<ut::OrbitalState> mOrbitalStatePtr;        //! The orbital state that is reported out.
   ut::CloneablePtr<Perturbation>     mPerturbationPtr;

   double     mTimePastEpoch{0.0}; //! The time in seconds since the initial epoch time.
   UtCalendar mCurrentTime; //! The current time to which the propagator has updated.  The propagated elements are valid at this time.
   UtCalendar mLastUpdateTime; //! The previously updated time.

   bool mDebugOutputOE{false};
   bool mDebugOutputSTK{false};
   bool mDebugOutputWSF{false};
   bool mDebugOutputXYZ{false};
};

#endif
