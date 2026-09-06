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

#include "WsfOrbitalDynamics.hpp"

#include <sstream>
#include <utility>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
#include "WsfOrbitalDynamicsTermTypes.hpp"

WsfOrbitalDynamics::WsfOrbitalDynamics(const WsfScenario& aScenario)
   : mScenario(aScenario)
{
}

WsfOrbitalDynamics::WsfOrbitalDynamics(const WsfOrbitalDynamics& aOther)
   : mScenario(aOther.mScenario)
   , mSimulationPtr{nullptr}
   , mTerms{aOther.mTerms}
   , mPropagatorPtr{nullptr}
{
}

WsfOrbitalDynamics* WsfOrbitalDynamics::Clone() const
{
   return new WsfOrbitalDynamics{*this};
}

bool WsfOrbitalDynamics::ProcessInput(UtInput& aInput)
{
   bool        retval{false};
   std::string command = aInput.GetCommand();
   if (command == "term")
   {
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);
      WsfOrbitalDynamicsTerm* termTypePtr = WsfOrbitalDynamicsTermTypes::Get(mScenario).Find(type);
      if (termTypePtr != nullptr)
      {
         auto termInstancePtr = ut::clone(termTypePtr);
         inputBlock.ProcessInput(termInstancePtr.get());
         AddTerm(std::move(termInstancePtr));
         retval = true;
      }
      else
      {
         std::stringstream msg{};
         msg << "Unknown orbital dynamics term '" << type << "'.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
   }
   return retval;
}

//! Compute the total acceleration for an object with the specified properties.
//!
//! This computes the total acceleration for all the terms represented by this
//! dynamical model. The accelerations are computed for an object of the given
//! @p aMass that occupies the given @p aPosition with the given @p aVelocity at
//! the given @p aTime. The input kinematics and the returned acceleration are
//! all given in the ECI frame.
//!
//! \param aMass     - The mass of the object subject to these dynamics.
//! \param aTime     - The time at which the acceleration is being computed.
//! \param aPosition - The position of the object subject to these dynamics.
//! \param aVelocity - The velocity of the object subject to these dynamics.
//! \returns         - The acceleration.
UtVec3d WsfOrbitalDynamics::ComputeAcceleration(double            aMass,
                                                const UtCalendar& aTime,
                                                const UtVec3d&    aPosition,
                                                const UtVec3d&    aVelocity) const
{
   UtVec3d retval{0.0, 0.0, 0.0};
   for (auto& term : mTerms)
   {
      retval += term->ComputeAcceleration(aMass, aTime, aPosition, aVelocity);
   }
   return retval;
}

bool WsfOrbitalDynamics::Initialize(const WsfSimulation*            aSimulationPtr,
                                    const WsfIntegratingPropagator& aPropagator,
                                    const UtCalendar&               aInitialTime)
{
   mPropagatorPtr = &aPropagator;
   mSimulationPtr = aSimulationPtr;
   mDeltaUT1      = aInitialTime.GetDeltaUT1();
   mDeltaAT       = aInitialTime.GetDeltaAT();
   bool ok{true};
   for (auto& term : mTerms)
   {
      ok = term->Initialize(*this) && ok;
   }
   return ok;
}

UtVec3d WsfOrbitalDynamics::GetWCS_PositionFromECI(const UtVec3d& aPositionECI) const
{
   return mConverter.ConvertECI_ToWCS(aPositionECI);
}

UtVec3d WsfOrbitalDynamics::GetECI_AccelerationFromWCS(const UtVec3d& aAccelerationWCS) const
{
   return mConverter.ConvertWCS_ToECI(aAccelerationWCS);
}

UtVec3d WsfOrbitalDynamics::GetWCS_VelocityFromECI(const UtVec3d& aPositionECI, const UtVec3d& aVelocityECI) const
{
   return mConverter.ConvertVelocityECI_ToWCS(aPositionECI, aVelocityECI);
}

UtLLAPos WsfOrbitalDynamics::GetLLA_FromWCS(const UtVec3d& aPositionWCS) const
{
   double lat;
   double lon;
   double alt;
   mConverter.GetCentralBody().ConvertWCS_ToLLA(aPositionWCS, lat, lon, alt);
   return UtLLAPos{lat, lon, alt};
}

void WsfOrbitalDynamics::UpdateConverterTime(const UtCalendar& aTime) const
{
   if (!mEpochSet)
   {
      mConverter.SetEpoch(aTime);
      mInitialEpoch = aTime;
      mEpochSet     = true;
   }
   else if (aTime != mConverter.GetEpoch())
   {
      mConverter.SetTime(aTime.GetTimeSince(mInitialEpoch));
   }
}

//! Add a term to this set of dynamics.
void WsfOrbitalDynamics::AddTerm(std::unique_ptr<WsfOrbitalDynamicsTerm> aTermPtr)
{
   mTerms.emplace_back(std::move(aTermPtr));
}

//! Get an orbital dynamics term.
//!
//! \param aIndex - the index of the term to retrieve.
//! \returns      - the term, or nullptr if @p aIndex is out of bounds.
const WsfOrbitalDynamicsTerm* WsfOrbitalDynamics::GetTerm(size_t aIndex) const
{
   const WsfOrbitalDynamicsTerm* retvalPtr{nullptr};
   if (aIndex < mTerms.size())
   {
      retvalPtr = mTerms[aIndex];
   }
   return retvalPtr;
}

//! Remove an orbital dynamics term.
//!
//! \param aIndex - the index of the term to remove.
//! \returns      - true if the term was successfully removed; false otherwise.
bool WsfOrbitalDynamics::RemoveTerm(size_t aIndex)
{
   if (aIndex < mTerms.size())
   {
      return false;
   }
   mTerms.erase(mTerms.begin() + aIndex);
   return true;
}

void WsfOrbitalDynamics::GetTimeConstants(UtCalendar& aTime) const
{
   aTime.SetDeltaUT1(mDeltaUT1);
   aTime.SetDeltaAT(mDeltaAT);
}
