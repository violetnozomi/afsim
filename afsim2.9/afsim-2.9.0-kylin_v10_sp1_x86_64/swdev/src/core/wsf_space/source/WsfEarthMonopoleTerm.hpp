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

#ifndef WSFEARTHMONOPOLETERM_HPP
#define WSFEARTHMONOPOLETERM_HPP

#include "UtEarth.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
class UtInput;

class WsfEarthMonopoleTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfEarthMonopoleTerm()                                   = default;
   WsfEarthMonopoleTerm(const WsfEarthMonopoleTerm& aOther) = default;
   ~WsfEarthMonopoleTerm() override                         = default;

   bool ProcessInput(UtInput& aInput) override;

   WsfEarthMonopoleTerm* Clone() const override { return new WsfEarthMonopoleTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfEarthMonopoleTerm"; }

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   double GetGravitationalParameter() const { return mGravitationalParameter; }
   void   SetGravitationalParameter(double aMu) { mGravitationalParameter = aMu; }

   constexpr static const char* cTYPE{"earth_monopole"};

private:
   double mGravitationalParameter{UtEarth::cGRAVITATIONAL_PARAMETER};
};

#endif // WSFEARTHMONOPOLETERM_HPP
