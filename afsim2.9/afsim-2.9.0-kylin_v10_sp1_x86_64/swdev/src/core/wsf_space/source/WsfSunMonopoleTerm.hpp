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

#ifndef WSFSUNMONOPOLETERM_HPP
#define WSFSUNMONOPOLETERM_HPP

#include "UtCloneablePtr.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
class UtInput;
#include "UtSolarSystem.hpp"
#include "UtSun.hpp"
#include "WsfDE_FileHandle.hpp"

class WsfSunMonopoleTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfSunMonopoleTerm();
   WsfSunMonopoleTerm(const WsfSunMonopoleTerm& aOther) = default;
   ~WsfSunMonopoleTerm() override                       = default;

   WsfSunMonopoleTerm* Clone() const override { return new WsfSunMonopoleTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfSunMonopoleTerm"; }

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(const WsfOrbitalDynamics& aDynamics) override;

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   double GetGravitationalParameter() const { return mGravitationalParameter; }
   void   SetGravitationalParameter(double aMu) { mGravitationalParameter = aMu; }

   constexpr static const char* cTYPE{"sun_monopole"};

   UtVec3d GetSunLocationECI(const UtCalendar& aEpoch) const;

private:
   enum DataSource
   {
      cDEFAULT,
      cDE_FILE
   };

   double                              mGravitationalParameter{UtSun::cGRAVITATIONAL_PARAMETER};
   UtCloneablePtr<UtSolarSystem::Body> mEarthPtr{nullptr};
   DataSource                          mDataSource{cDEFAULT};
   std::string                         mDataFileName{};
   ut::CloneablePtr<WsfDE_FileHandle>  mDataFile{nullptr};
};

#endif // WSFSUNMONOPOLETERM_HPP
