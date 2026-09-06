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

#include "WsfConjunctionInput.hpp"

#include <cmath>
#include <memory>
#include <string>

#include "UtCalendar.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtOrbitalState.hpp"
#include "WsfMover.hpp"
#include "WsfOrbitalTargeting.hpp"
#include "WsfPlatform.hpp"
#include "WsfPropagatorTargetPoint.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMover.hpp"

WsfConjunctionInput::WsfConjunctionInput()
   : mTarget{}
   , mInitialLocationECI{}
   , mInitialVelocityECI{}
   , mSolutionVelocityECI{}
   , mConjunctionTime{0.0}
   , mLLA{0.0, 0.0, 400000.0}
   , mPosType{PositionInputType::cUNSPECIFIED}
   , mValidInput{false}
   , mSolutionFound{false}
{
}

WsfConjunctionInput::WsfConjunctionInput(const WsfConjunctionInput& aOther)
   : mTarget{aOther.mTarget}
   , mInitialLocationECI{aOther.mInitialLocationECI}
   , mInitialVelocityECI{aOther.mInitialVelocityECI}
   , mSolutionVelocityECI{aOther.mSolutionVelocityECI}
   , mTolerance{aOther.mTolerance}
   , mConjunctionTime{aOther.mConjunctionTime}
   , mLLA{aOther.mLLA[0], aOther.mLLA[1], aOther.mLLA[2]}
   , mPosType{aOther.mPosType}
   , mValidInput{aOther.mValidInput}
   , mSolutionFound{aOther.mSolutionFound}
{
}

//! Process the input file to setup the conjunction.
bool WsfConjunctionInput::ProcessInput(UtInput& aInput)
{
   bool targetInput{false};
   bool timeInput{false};

   UtInputBlock inputBlock(aInput);
   while (inputBlock.ReadCommand())
   {
      std::string comm = inputBlock.GetCommand();
      if (comm == "from_eci")
      {
         double locECI[3];
         aInput.ReadValueOfType(locECI[0], UtInput::cLENGTH);
         aInput.ReadValueOfType(locECI[1], UtInput::cLENGTH);
         aInput.ReadValueOfType(locECI[2], UtInput::cLENGTH);
         mInitialLocationECI.Set(locECI);
         mPosType = PositionInputType::cECI;
      }
      else if (comm == "from_lla")
      {
         aInput.ReadValueOfType(mLLA[0], UtInput::cLATITUDE);
         aInput.ReadValueOfType(mLLA[1], UtInput::cLONGITUDE);
         aInput.ReadValueOfType(mLLA[2], UtInput::cLENGTH);
         mPosType = PositionInputType::cLAT_LONG_ALT;
      }
      else if (comm == "from_initial")
      {
         mPosType = PositionInputType::cFROM_INITIAL;
      }
      else if (comm == "with_target")
      {
         aInput.ReadValue(mTarget);
         targetInput = true;
      }
      else if (comm == "at_time")
      {
         aInput.ReadValueOfType(mConjunctionTime, UtInput::cTIME);
         timeInput = true;
      }
      else if (comm == "tolerance")
      {
         aInput.ReadValue(mTolerance);
         aInput.ValueGreater(mTolerance, 0.0);
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   mValidInput = (mPosType != PositionInputType::cUNSPECIFIED) && targetInput && timeInput;

   return mValidInput;
}

//! Clone this object.
WsfConjunctionInput* WsfConjunctionInput::Clone() const
{
   return new WsfConjunctionInput(*this);
}

//! Initialize this object.
//! This method will set the initial position and velocity for the solution. These
//! kinematics will be used in the event that a conjunction solution is impossible.
//! Unless the position and velocity are taken from other initialization, this will
//! set the velocity to be the circular velocity for the specified position.
bool WsfConjunctionInput::Initialize(const UtCalendar& aTime, const ut::OrbitalState& aState)
{
   if (mValidInput)
   {
      if (mPosType == PositionInputType::cFROM_INITIAL)
      {
         ut::OrbitalState::Vector inertialStateVector = aState.GetOrbitalStateVectorInertial();
         mInitialLocationECI                          = inertialStateVector.GetLocation();
         mInitialVelocityECI                          = inertialStateVector.GetVelocity();
      }
      else
      {
         if (mPosType == PositionInputType::cLAT_LONG_ALT)
         {
            UtEntity sat;
            sat.SetECI_ReferenceEpoch(aTime);
            sat.SetTime(aTime.GetTime());
            sat.SetLocationLLA(mLLA[0], mLLA[1], mLLA[2]);
            sat.GetLocationECI(mInitialLocationECI.GetData());
         }

         double dist = mInitialLocationECI.Magnitude();
         double rxy =
            sqrt(mInitialLocationECI[0] * mInitialLocationECI[0] + mInitialLocationECI[1] * mInitialLocationECI[1]);
         double speed     = sqrt(aState.GetCentralBody().GetGravitationalParameter() / dist);
         double nedVel[3] = {speed * cos(UtMath::cPI_OVER_2),
                             speed * sin(UtMath::cPI_OVER_2) - aState.GetCentralBody().GetRotationRate() * rxy,
                             0.0};

         UtEntity sat;
         sat.SetECI_ReferenceEpoch(aTime);
         sat.SetTime(aTime.GetTime());
         sat.SetLocationECI(mInitialLocationECI.GetData());
         sat.SetVelocityNED(nedVel);
         sat.GetVelocityECI(mInitialVelocityECI.GetData());
      }

      mSolutionVelocityECI = mInitialVelocityECI;
   }

   return mValidInput;
}

//! Compute the solution for the initial velocity to provide the specified conjunction.
//! This will solve the targeting problem to produce the input conjunction. This sets the
//! solution velocity so that the spacecraft would move from the initial position to the
//! position of the target spacecraft at the given time.
void WsfConjunctionInput::ComputeSolution(const WsfSimulation&           aSimulation,
                                          const UtCalendar&              aTime,
                                          const UtOrbitalPropagatorBase& aInterceptorProp)
{
   auto targetPropTemplate = GetTargetPropagator(aSimulation);
   if (targetPropTemplate != nullptr)
   {
      auto targetProp = ut::clone(targetPropTemplate);
      targetProp->Initialize(aTime);
      wsf::space::PropagatorTargetPoint targetPoint{std::move(targetProp)};
      auto                              interceptorProp = ut::clone(aInterceptorProp);
      interceptorProp->Initialize(ut::OrbitalState(aTime,
                                                   interceptorProp->GetInitialOrbitalState().GetCentralBody(),
                                                   interceptorProp->GetInitialOrbitalState().GetCoordinateSystem(),
                                                   ut::OrbitalState::ReferenceFrame::cECI,
                                                   ut::OrbitalState::Vector(mInitialLocationECI, mInitialVelocityECI)));
      WsfOrbitalTargeting targ{aTime, *interceptorProp, targetPoint};
      targ.SetTolerance(mTolerance);
      UtVec3d deltaV;
      UtVec3d unused;
      mSolutionFound =
         targ.Solve(mConjunctionTime, deltaV, unused).Assess(aInterceptorProp.HyperbolicPropagationAllowed(), false);

      if (mSolutionFound)
      {
         mSolutionVelocityECI.Add(deltaV);
      }
   }
}

//! A utility routine to get the propagator for the given target.
const UtOrbitalPropagatorBase* WsfConjunctionInput::GetTargetPropagator(const WsfSimulation& aSimulation) const
{
   const UtOrbitalPropagatorBase* retval{nullptr};

   WsfPlatform* targetPlatform = aSimulation.GetPlatformByName(mTarget);
   if (targetPlatform)
   {
      WsfMover* mover = targetPlatform->GetMover();
      if (mover != nullptr && mover->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE)
      {
         WsfSpaceMoverBase* spaceMover = dynamic_cast<WsfSpaceMoverBase*>(mover);
         if (spaceMover)
         {
            retval = &spaceMover->GetPropagator();
         }
         else
         {
            auto logger = ut::log::warning() << "Platform does not have a space mover.";
            logger.AddNote() << "Platform: " << mTarget;
         }
      }
      else
      {
         auto logger = ut::log::warning() << "Platform is not in the SPACE domain.";
         logger.AddNote() << "Platform: " << mTarget;
      }
   }
   else
   {
      auto logger = ut::log::warning() << "Platform not found.";
      logger.AddNote() << "Platform: " << mTarget;
   }

   return retval;
}
