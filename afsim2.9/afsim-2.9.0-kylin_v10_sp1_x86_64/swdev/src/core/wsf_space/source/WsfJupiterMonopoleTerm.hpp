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

#ifndef WSFJUPITERMONOPOLETERM_HPP
#define WSFJUPITERMONOPOLETERM_HPP

#include "UtCloneablePtr.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
class UtInput;
#include "UtJupiter.hpp"
#include "WsfDE_FileHandle.hpp"

class WsfJupiterMonopoleTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfJupiterMonopoleTerm();
   WsfJupiterMonopoleTerm(const WsfJupiterMonopoleTerm& aOther) = default;
   ~WsfJupiterMonopoleTerm() override                           = default;

   WsfJupiterMonopoleTerm* Clone() const override { return new WsfJupiterMonopoleTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfJupiterMonopoleTerm"; }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(const WsfOrbitalDynamics& aDynamics) override;

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   double GetGravitationalParameter() const { return mGravitationalParameter; }
   void   SetGravitationalParameter(double aMu) { mGravitationalParameter = aMu; }

   constexpr static const char* cTYPE{"jupiter_monopole"};

   UtVec3d GetJupiterLocationECI(const UtCalendar& aEpoch) const;

private:
   enum DataSource
   {
      cDEFAULT,
      cDE_FILE
   };

   ut::Jupiter                        mJupiter{};
   double                             mGravitationalParameter;
   DataSource                         mDataSource{cDEFAULT};
   std::string                        mDataFileName{};
   ut::CloneablePtr<WsfDE_FileHandle> mDataFile{nullptr};
};

#endif // WSFJUPITERMONOPOLETERM_HPP
