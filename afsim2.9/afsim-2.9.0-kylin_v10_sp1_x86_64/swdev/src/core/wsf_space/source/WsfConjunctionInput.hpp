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

#ifndef WSFCONJUNCTIONINPUT_HPP
#define WSFCONJUNCTIONINPUT_HPP

#include <string>

#include "UtCalendar.hpp"
class UtInput;
class UtOrbitalPropagatorBase;
namespace ut
{
class OrbitalState;
}
#include "UtVec3.hpp"

class WsfSimulation;

//! A class to setup a satellite so that it will suffer a conjunction at a given time with a given target.
class WsfConjunctionInput
{
public:
   WsfConjunctionInput();
   WsfConjunctionInput(const WsfConjunctionInput& aOther);

   WsfConjunctionInput& operator=(const WsfConjunctionInput& aOther) = default;

   bool ProcessInput(UtInput& aInput);

   WsfConjunctionInput* Clone() const;

   //! Indicate if a solution to generate the conjunction was found.
   bool SolutionFound() const { return mSolutionFound; }

   //! Return the initial location of the satellite.
   const UtVec3d& InitialLocationECI() const { return mInitialLocationECI; }

   //! Return the initial velocity of the satellite.
   //! This velocity is what the satellite will have if the conjunction solution is
   //! impossible for some reason (e.g. the orbit hits the Earth).
   const UtVec3d& InitialVelocityECI() const { return mInitialVelocityECI; }

   //! return the solution velocity of the satellite.
   //! This velocity if what the satellite will have if the conjunction solution is
   //! successful.
   const UtVec3d& SolutionVelocityECI() const { return mSolutionVelocityECI; }

   bool Initialize(const UtCalendar& aTime, const ut::OrbitalState& aState);

   void ComputeSolution(const WsfSimulation&           aSimulation,
                        const UtCalendar&              aTime,
                        const UtOrbitalPropagatorBase& aInterceptorProp);

private:
   //! Enumerate the possible ways to provide initial position.
   enum class PositionInputType
   {
      cUNSPECIFIED,
      cLAT_LONG_ALT,
      cECI,
      cFROM_INITIAL
   };

   const UtOrbitalPropagatorBase* GetTargetPropagator(const WsfSimulation& aSimulation) const;

   std::string       mTarget;
   UtVec3d           mInitialLocationECI;
   UtVec3d           mInitialVelocityECI;
   UtVec3d           mSolutionVelocityECI;
   double            mTolerance{1.0e-9};
   double            mConjunctionTime;
   double            mLLA[3];
   PositionInputType mPosType;
   bool              mValidInput;
   bool              mSolutionFound;
};

#endif // WSFCONJUNCTIONINPUT_HPP
