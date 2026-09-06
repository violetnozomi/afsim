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

#ifndef WSFORBITALINTEGRATOR_HPP
#define WSFORBITALINTEGRATOR_HPP

#include "UtCalendar.hpp"
#include "UtOrbitalState.hpp"
#include "UtVec3.hpp"

class WsfIntegratingPropagator;
#include "WsfObject.hpp"
class WsfOrbitalDynamics;

class WsfOrbitalIntegrator : public WsfObject
{
public:
   WsfOrbitalIntegrator() = default;

   WsfOrbitalIntegrator(const WsfOrbitalIntegrator& aOther)
      : WsfObject(aOther)
      , mPropagatorPtr{nullptr}
   {
   }

   ~WsfOrbitalIntegrator() override = default;

   WsfOrbitalIntegrator& operator=(const WsfOrbitalIntegrator& aOther) = delete;

   WsfOrbitalIntegrator* Clone() const override = 0;

   const char* GetScriptClassName() const override { return "WsfOrbitalIntegrator"; }

   virtual ut::OrbitalState AdvanceToTime(const WsfOrbitalDynamics& aDynamics,
                                          const UtCalendar&         aFinalTime,
                                          const ut::OrbitalState&   aInitialState) = 0;

   virtual std::string GetIntegratorType() const = 0;

   void SetPropagator(const WsfIntegratingPropagator* aPropagatorPtr) { mPropagatorPtr = aPropagatorPtr; }

protected:
   const WsfIntegratingPropagator* mPropagatorPtr{nullptr};
};

#endif // WSFORBITALINTEGRATOR_HPP
