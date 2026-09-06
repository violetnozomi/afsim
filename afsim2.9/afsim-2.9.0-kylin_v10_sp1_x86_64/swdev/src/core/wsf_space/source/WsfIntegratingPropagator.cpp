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

#include "WsfIntegratingPropagator.hpp"

#include <sstream>
#include <typeinfo>
#include <utility>

#include "UtCalendar.hpp"
#include "UtEarth.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtOrbitalPropagatorBase.hpp"
#include "UtTwoLineElement.hpp"
#include "UtVec3.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "WsfOrbitalIntegrator.hpp"
#include "WsfOrbitalIntegratorTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfRocketOrbitalManeuvering.hpp"
#include "WsfScenario.hpp"
#include "WsfSpaceMoverBase.hpp"

namespace // {anonymous}
{

class InitialStateECI_Input
{
public:
   InitialStateECI_Input() = default;

   bool ProcessInput(UtInput& aInput)
   {
      bool        ok{true};
      std::string command;
      aInput.GetCommand(command);
      if (command == "position")
      {
         aInput.ReadValueOfType(mPosition[0], UtInput::cLENGTH);
         aInput.ReadValueOfType(mPosition[1], UtInput::cLENGTH);
         aInput.ReadValueOfType(mPosition[2], UtInput::cLENGTH);
         mHasPosition = true;
      }
      else if (command == "velocity")
      {
         aInput.ReadValueOfType(mVelocity[0], UtInput::cSPEED);
         aInput.ReadValueOfType(mVelocity[1], UtInput::cSPEED);
         aInput.ReadValueOfType(mVelocity[2], UtInput::cSPEED);
         mHasVelocity = true;
      }
      else if (command == "epoch")
      {
         double epoch;
         aInput.ReadValue(epoch);
         mEpoch.SetEpoch(epoch);
         mHasEpoch = true;
      }
      else if (command == "epoch_date_time")
      {
         aInput.ReadValue(mEpoch);
         mHasEpoch = true;
      }
      else if (command == "j2000")
      {
         mInJ2000 = true;
      }
      else
      {
         ok = false;
      }
      return ok;
   }

   UtVec3d GetPosition() const
   {
      if (mInJ2000)
      {
         UtECI_Conversion conv{mEpoch, ut::EarthEGM96()}; // earth-only
         conv.SetLocationJ2000(mPosition);
         return conv.GetLocationECI();
      }
      else
      {
         return mPosition;
      }
   }
   UtVec3d GetVelocity() const
   {
      if (mInJ2000)
      {
         UtECI_Conversion conv{mEpoch, ut::EarthEGM96()}; // earth-only
         conv.SetVelocityJ2000(mVelocity);
         return conv.GetVelocityECI();
      }
      else
      {
         return mVelocity;
      }
   }
   const UtCalendar& GetEpoch() const { return mEpoch; }
   bool              IsValid() const { return mHasPosition && mHasVelocity && mHasEpoch; }

private:
   UtVec3d    mPosition{};
   UtVec3d    mVelocity{};
   UtCalendar mEpoch{};
   bool       mHasPosition{false};
   bool       mHasVelocity{false};
   bool       mHasEpoch{false};
   bool       mInJ2000{false};
};

class ConstantMassProvider : public WsfIntegratingPropagator::MassProvider
{
public:
   explicit ConstantMassProvider(double aMass)
      : mMass{aMass}
   {
   }
   ~ConstantMassProvider() override = default;

   double GetMass() const override { return mMass; }

   std::unique_ptr<MassProvider> Decay() const override
   {
      return std::unique_ptr<MassProvider>(new ConstantMassProvider{mMass});
   }

private:
   double mMass;
};

class PlatformMassProvider : public WsfIntegratingPropagator::MassProvider
{
public:
   explicit PlatformMassProvider(const WsfPlatform& aPlatform)
      : mPlatform(aPlatform)
   {
   }
   ~PlatformMassProvider() override = default;

   double GetMass() const override { return mPlatform.GetMass(); }

   std::unique_ptr<MassProvider> Decay() const override
   {
      return std::unique_ptr<MassProvider>(new ConstantMassProvider{GetMass()});
   }

private:
   // Note that we can be sure that the platform will continue to exist if this provider is
   // ever installed on a WsfIntegratingPropagator, because it will only be installed for the
   // propagator owned by the mover owned by the platform. So the reference only would be invalid
   // in the case that the platform is removed, in which case the propagator owning this would
   // also have already been destroyed.
   const WsfPlatform& mPlatform;
};

class RocketMassProvider : public WsfIntegratingPropagator::MassProvider
{
public:
   explicit RocketMassProvider(const WsfRocketOrbitalManeuvering& aRocket)
      : mRocket(aRocket)
   {
   }
   ~RocketMassProvider() override = default;

   double GetMass() const override { return mRocket.GetTotalMass(); }

   std::unique_ptr<MassProvider> Decay() const override
   {
      return std::unique_ptr<MassProvider>(new ConstantMassProvider{GetMass()});
   }

private:
   const WsfRocketOrbitalManeuvering& mRocket;
};

} // namespace

WsfIntegratingPropagator::WsfIntegratingPropagator(const WsfScenario& aScenario)
   : WsfNonClassicalOrbitalPropagator(ut::make_unique<ut::OrbitalState>(ut::EarthWGS84(),
                                                                        ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                                                        ut::OrbitalState::ReferenceFrame::cTRUE_OF_DATE))
   , mScenario(aScenario)
{
}

WsfIntegratingPropagator::WsfIntegratingPropagator(const WsfIntegratingPropagator& aOther)
   : WsfNonClassicalOrbitalPropagator(aOther)
   , mScenario(aOther.mScenario)
   , mPropagatedOrbitalState(aOther.mPropagatedOrbitalState)
   , mIntegratorPtr{aOther.mIntegratorPtr}
   , mDynamicsPtr{aOther.mDynamicsPtr}
   , mMassProvider{nullptr}
   , mAccelerationValid{false}
   , mKinematicInput{aOther.mKinematicInput}
{
   if (mIntegratorPtr)
   {
      mIntegratorPtr->SetPropagator(this);
   }
   if (aOther.mMassProvider)
   {
      mMassProvider = aOther.mMassProvider->Decay();
   }
}

WsfIntegratingPropagator::~WsfIntegratingPropagator() {}

WsfOrbitalPropagatorTypes::FactoryPtr WsfIntegratingPropagator::ObjectFactory(WsfScenario& aScenario)
{
   return [&aScenario](const std::string& aTypeName) -> std::unique_ptr<UtOrbitalPropagatorBase>
   {
      if (aTypeName == "WSF_INTEGRATING_PROPAGATOR")
      {
         return ut::make_unique<WsfIntegratingPropagator>(aScenario);
      }
      return nullptr;
   };
}

WsfIntegratingPropagator* WsfIntegratingPropagator::Clone() const
{
   return new WsfIntegratingPropagator{*this};
}

bool WsfIntegratingPropagator::Initialize(const UtCalendar& aInitialTime)
{
   bool ok{WsfNonClassicalOrbitalPropagator::Initialize(aInitialTime)};
   if (ok)
   {
      mPropagatedOrbitalState = GetInitialOrbitalState();
      mPropagatedOrbitalState.SetReferenceFrame(ut::OrbitalState::ReferenceFrame::cECI);

      if (mIntegratorPtr == nullptr)
      {
         ut::log::error() << "No integrator specified in integrating propagator.";
         ok = false;
      }

      if (mDynamicsPtr == nullptr)
      {
         ut::log::error() << "No dynamical model specified for integrating propagator.";
         ok = false;
      }

      if (mMassProvider == nullptr)
      {
         // This branch occurs when the mover is not attached to a platform. This only happens
         // when verifying using Astrolabe in Wizard. This sets a large value so that dynamics
         // that depend on the mass will have little effect on the propagation.
         mMassProvider = ut::make_unique<ConstantMassProvider>(1.0e14);
      }
   }

   return ok;
}

bool WsfIntegratingPropagator::InitializeDynamics(const WsfSimulation* aSimulationPtr, const UtCalendar& aInitialTime)
{
   bool ok = mDynamicsPtr->Initialize(aSimulationPtr, *this, aInitialTime);

   if (!mMassProvider || mMassProvider->GetMass() <= 0.0)
   {
      std::string platName{};
      auto*       moverPtr = GetSpaceMover();
      if (moverPtr)
      {
         platName = moverPtr->GetPlatform()->GetName();
      }
      auto msg = ut::log::error() << "Platform does not have a positive mass.";
      msg.AddNote() << "Platform: " << platName;
      ok = false;
   }

   if (ok)
   {
      UpdateTime(aInitialTime);
      Propagate(aInitialTime);
      PostPropagate();
   }

   return ok;
}

bool WsfIntegratingPropagator::ProcessInput(UtInput& aInput)
{
   bool retval{true};

   std::string command = aInput.GetCommand();
   if (command == "integrator")
   {
      UtInputBlock inputBlock(aInput);

      std::string type;
      aInput.ReadValue(type);
      WsfOrbitalIntegrator* integratorTypePtr = WsfOrbitalIntegratorTypes::Get(mScenario).Find(type);
      if (integratorTypePtr != nullptr)
      {
         auto integratorPtr = ut::clone(integratorTypePtr);
         SetOrbitalIntegrator(std::move(integratorPtr));
         inputBlock.ProcessInput(mIntegratorPtr.get());
      }
      else
      {
         std::stringstream msg{};
         msg << "Unknown integrator type '" << type << "'.";
         throw UtInput::BadValue{aInput, msg.str()};
      }
   }
   else if (command == "dynamics")
   {
      UtInputBlock inputBlock(aInput);
      mDynamicsPtr = ut::make_unique<WsfOrbitalDynamics>(mScenario);
      inputBlock.ProcessInput(mDynamicsPtr.get());
   }
   else if (command == "initial_state")
   {
      InitialStateECI_Input initialState{};
      UtInputBlock          block{aInput};

      block.ProcessInput(&initialState);

      if (initialState.IsValid())
      {
         mAccelerationValid = false;
         mKinematicInput    = true;
         ut::OrbitalState initialOrbitalState(initialState.GetEpoch(),
                                              GetInitialOrbitalState().GetCentralPoint(),
                                              ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                              ut::OrbitalState::ReferenceFrame::cECI,
                                              ut::OrbitalState::Vector(initialState.GetPosition(),
                                                                       initialState.GetVelocity()));
         SetInitialOrbitalState(initialOrbitalState);
      }
      else
      {
         throw UtInput::ExceptionBase(aInput, "Incomplete initial state definition: must specify position, velocity and epoch.");
      }
   }
   else
   {
      retval = WsfNonClassicalOrbitalPropagator::ProcessInput(aInput);
   }
   return retval;
}

void WsfIntegratingPropagator::SetSpaceMover(WsfSpaceMoverBase* aMoverPtr)
{
   if (aMoverPtr)
   {
      try
      {
         // If successful, this propagator is associated with a space mover that has a rocket maneuvering
         // model, or, this is a Clone used during Verify that has a rocket maneuvering model. In either
         // case, we want to use a RocketMassProvider.
         const WsfRocketOrbitalManeuvering& rocket =
            dynamic_cast<const WsfRocketOrbitalManeuvering&>(aMoverPtr->GetOrbitalManeuvering());
         mMassProvider = ut::make_unique<RocketMassProvider>(rocket);
      }
      catch (std::bad_cast&)
      {
         // This branch occurs both for both propagators owned by movers, and by managers. If it
         // is associated with a mover, we should be sure to set up a mass provider.
         WsfPlatform* platformPtr = aMoverPtr->GetPlatform();
         if (platformPtr)
         {
            // If the mover is attached to a platform, use a platform mass provider.
            mMassProvider = ut::make_unique<PlatformMassProvider>(*platformPtr);
         }
         else
         {
            // This branch occurs when the mover is not attached to a platform. This only happens
            // when verifying using Astrolabe in Wizard. This sets a large value so that dynamics
            // that depend on the mass will have little effect on the propagation.
            mMassProvider = ut::make_unique<ConstantMassProvider>(1.0e14);
         }
      }
   }
   wsf::space::OrbitalPropagator::SetSpaceMover(aMoverPtr);
}

double WsfIntegratingPropagator::GetDynamicalMass() const
{
   return mMassProvider->GetMass();
}

void WsfIntegratingPropagator::Propagate(const UtCalendar& aTime)
{
   auto&           osv            = mPropagatedOrbitalState.GetOrbitalStateVector();
   auto&           lastUpdateTime = mPropagatedOrbitalState.GetEpoch();
   AdvancingHandle handle(*this);

   if (!mAccelerationValid)
   {
      mPropagatedOrbitalState.SetAccelerationInertial(
         mDynamicsPtr->ComputeAcceleration(GetDynamicalMass(), lastUpdateTime, osv.GetLocation(), osv.GetVelocity()));
      mAccelerationValid = true;
   }

   auto propagatedState = mIntegratorPtr->AdvanceToTime(*mDynamicsPtr, aTime, mPropagatedOrbitalState);

   // NOTE: No need to update the current time, as that is handled in UpdateTime(), called in the base class.
   mPropagatedOrbitalState.Set(aTime, propagatedState.GetOrbitalStateVector());
   mPropagatedOrbitalState.SetAccelerationInertial(propagatedState.GetAccelerationInertial());
}

std::unique_ptr<WsfOrbitalDynamics> WsfIntegratingPropagator::GetOrbitalDynamicsClone() const
{
   std::unique_ptr<WsfOrbitalDynamics> retvalPtr{nullptr};
   if (mDynamicsPtr)
   {
      retvalPtr = ut::clone(mDynamicsPtr.get());
   }
   return retvalPtr;
}

bool WsfIntegratingPropagator::SetOrbitalDynamics(std::unique_ptr<WsfOrbitalDynamics> aDynamicsPtr)
{
   bool retval{false};
   if (!IsAdvancing())
   {
      bool isInitialized = (mDynamicsPtr != nullptr) && (mDynamicsPtr->GetPropagator() != nullptr);
      if (isInitialized)
      {
         UtCalendar constants{};
         mDynamicsPtr->GetTimeConstants(constants);
         const WsfSimulation* simPtr = mDynamicsPtr->GetSimulation();
         mDynamicsPtr                = std::move(aDynamicsPtr);
         mDynamicsPtr->Initialize(simPtr, *this, constants);
      }
      else
      {
         mDynamicsPtr = std::move(aDynamicsPtr);
      }
      retval = true;
   }
   return retval;
}

std::unique_ptr<WsfOrbitalIntegrator> WsfIntegratingPropagator::GetOrbitalIntegratorClone() const
{
   std::unique_ptr<WsfOrbitalIntegrator> retvalPtr{nullptr};
   if (mIntegratorPtr)
   {
      retvalPtr = ut::clone(mIntegratorPtr.get());
   }
   return retvalPtr;
}

bool WsfIntegratingPropagator::SetOrbitalIntegrator(std::unique_ptr<WsfOrbitalIntegrator> aIntegratorPtr)
{
   bool retval{false};
   if (!IsAdvancing())
   {
      mIntegratorPtr = std::move(aIntegratorPtr);
      mIntegratorPtr->SetPropagator(this);
      retval = true;
   }
   return retval;
}

void WsfIntegratingPropagator::UpdateOrbitalState()
{
   SetOrbitalState(mPropagatedOrbitalState);
}
