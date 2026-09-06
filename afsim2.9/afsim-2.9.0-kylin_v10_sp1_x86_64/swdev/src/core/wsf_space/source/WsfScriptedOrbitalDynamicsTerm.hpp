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

#ifndef WSFSCRIPTEDORBITALDYNAMICSTERM_HPP
#define WSFSCRIPTEDORBITALDYNAMICSTERM_HPP

#include <string>

#include "WsfOrbitalDynamicsTerm.hpp"

class UtCalendar;
class UtInput;
class UtScript;

class WsfScriptedOrbitalDynamicsTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfScriptedOrbitalDynamicsTerm() = default;

   WsfScriptedOrbitalDynamicsTerm(const WsfScriptedOrbitalDynamicsTerm& aOther)
      : WsfOrbitalDynamicsTerm(aOther)
      , mScriptName{aOther.mScriptName}
      , mScriptPtr{nullptr}
   {
   }

   ~WsfScriptedOrbitalDynamicsTerm() override = default;

   WsfScriptedOrbitalDynamicsTerm& operator=(const WsfScriptedOrbitalDynamicsTerm& aOther) = delete;

   bool ProcessInput(UtInput& aInput) override;

   WsfScriptedOrbitalDynamicsTerm* Clone() const override { return new WsfScriptedOrbitalDynamicsTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfScriptedDynamicsTerm"; }

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   bool Initialize(const WsfOrbitalDynamics& aDynamics) override;

   const std::string& GetScriptName() const { return mScriptName; }
   void               SetScriptName(const std::string& aName) { mScriptName = aName; }

   constexpr static const char* cTYPE{"scripted"};

private:
   bool FindScript();

   std::string       mScriptName{};
   mutable UtScript* mScriptPtr{nullptr};
};

#endif // WSFSCRIPTEDORBITALDYNAMICSTERM_HPP
