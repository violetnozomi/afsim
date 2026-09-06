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

#ifndef WSFINTEGRATINGPROPAGATOR_HPP
#define WSFINTEGRATINGPROPAGATOR_HPP

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtMath.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"
#include "WsfNonClassicalOrbitalPropagator.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "WsfOrbitalIntegrator.hpp"
#include "WsfOrbitalPropagatorTypes.hpp"
class WsfScenario;
class WsfSimulation;
class WsfSpaceMoverBase;

#include "wsf_space_export.h"

class WsfIntegratingPropagator : public WsfNonClassicalOrbitalPropagator
{
public:
   explicit WsfIntegratingPropagator(const WsfScenario& aScenario);
   WsfIntegratingPropagator(const WsfIntegratingPropagator& aOther);
   WsfIntegratingPropagator(WsfIntegratingPropagator&& aOther) = delete;
   ~WsfIntegratingPropagator() override;

   WsfIntegratingPropagator& operator=(const WsfIntegratingPropagator& aOther) = delete;
   WsfIntegratingPropagator& operator=(WsfIntegratingPropagator&& aOther) = delete;

   static WsfOrbitalPropagatorTypes::FactoryPtr ObjectFactory(WsfScenario& aScenario);

   WsfIntegratingPropagator* Clone() const override;
   bool                      Initialize(const UtCalendar& aInitialTime) override;
   bool                      InitializeDynamics(const WsfSimulation* aSimulationPtr, const UtCalendar& aInitialTime);
   bool                      ProcessInput(UtInput& aInput) override;
   void                      SetSpaceMover(WsfSpaceMoverBase* aMoverPtr) override;

   double GetDynamicalMass() const;

   std::unique_ptr<WsfOrbitalDynamics>   GetOrbitalDynamicsClone() const;
   bool                                  SetOrbitalDynamics(std::unique_ptr<WsfOrbitalDynamics> aDynamicsPtr);
   std::unique_ptr<WsfOrbitalIntegrator> GetOrbitalIntegratorClone() const;
   bool                                  SetOrbitalIntegrator(std::unique_ptr<WsfOrbitalIntegrator> aIntegratorPtr);

   class MassProvider
   {
   public:
      virtual ~MassProvider() = default;

      virtual double                        GetMass() const = 0;
      virtual std::unique_ptr<MassProvider> Decay() const   = 0;
   };

protected:
   void Propagate(const UtCalendar& aTime) override;
   void UpdateOrbitalState() override;
   bool HyperbolicPropagationAllowed() const override { return true; }

private:
   class AdvancingHandle
   {
   public:
      explicit AdvancingHandle(WsfIntegratingPropagator& aProp)
         : mProp(aProp)
      {
         aProp.mAdvancing = true;
      }

      ~AdvancingHandle() { mProp.mAdvancing = false; }

   private:
      WsfIntegratingPropagator& mProp;
   };

   bool IsAdvancing() const { return mAdvancing; }

   const WsfScenario&                   mScenario;
   ut::OrbitalState                     mPropagatedOrbitalState;
   UtCloneablePtr<WsfOrbitalIntegrator> mIntegratorPtr{nullptr};
   UtCloneablePtr<WsfOrbitalDynamics>   mDynamicsPtr{nullptr};
   std::unique_ptr<MassProvider>        mMassProvider{nullptr};
   bool                                 mAccelerationValid{false};
   bool                                 mKinematicInput{false};
   bool                                 mAdvancing{false};
};

#endif // WSFINTEGRATINGPROPAGATOR_HPP
