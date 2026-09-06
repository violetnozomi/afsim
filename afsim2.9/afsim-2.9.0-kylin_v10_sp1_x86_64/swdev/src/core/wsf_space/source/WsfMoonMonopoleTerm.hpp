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

#ifndef WSFMOONMONOPOLETERM_HPP
#define WSFMOONMONOPOLETERM_HPP

#include <memory>

#include "WsfOrbitalDynamicsTerm.hpp"

class UtInput;
#include "UtCloneablePtr.hpp"
#include "UtMoon.hpp"
#include "UtSpline.hpp"
#include "UtVec3.hpp"
#include "WsfDE_FileHandle.hpp"

class WsfMoonMonopoleTerm : public WsfOrbitalDynamicsTerm
{
public:
   WsfMoonMonopoleTerm()                                  = default;
   WsfMoonMonopoleTerm(const WsfMoonMonopoleTerm& aOther) = default;
   ~WsfMoonMonopoleTerm() override                        = default;

   WsfMoonMonopoleTerm* Clone() const override { return new WsfMoonMonopoleTerm{*this}; }

   const char* GetScriptClassName() const override { return "WsfMoonMonopoleTerm"; }

   bool ProcessInput(UtInput& aInput) override;

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   bool Initialize(const WsfOrbitalDynamics& aDynamics) override;

   double GetGravitationalParameter() const { return mGravitationalParameter; }
   void   SetGravitationalParameter(double aMu) { mGravitationalParameter = aMu; }

   double GetInterpolationInterval() const { return mMoonPosition.GetInterval(); }
   void   SetInterpolationInterval(double aInterval) { mMoonPosition.SetInterval(aInterval); }

   constexpr static const char* cTYPE{"moon_monopole"};

   UtVec3d GetMoonPositionECI(const UtCalendar& aTime) const;

private:
   enum DataSource
   {
      cDEFAULT,
      cDE_FILE
   };

   class MoonPositionHelper
   {
   public:
      MoonPositionHelper();

      double GetInterval() const { return mInterval; }
      void   SetInterval(double aInterval) { mInterval = aInterval; }

      const UtCalendar& GetBaseTime() const { return mBaseTime; }
      void              SetBaseTime(const UtCalendar& aBaseTime) { mBaseTime = aBaseTime; }

      UtVec3d GetPosition(const UtCalendar& aTime) const;

   private:
      void   ComputeSpline() const;
      double RedoSpline(const UtCalendar& aTime) const;

      double                                        mInterval{600.0};
      mutable UtCalendar                            mBaseTime{};
      mutable UtVec3d                               mF0{};
      mutable UtVec3d                               mF1{};
      mutable UtVec3d                               mF2{};
      mutable UtVec3d                               mF3{};
      mutable UtInterpolatingPolynomial<UtVec3d, 3> mSpline{{0.0, 0.0, 0.0, 0.0}, 0.0};
   };

   double                             mGravitationalParameter{UtMoon::cGRAVITATIONAL_PARAMETER};
   DataSource                         mDataSource{cDEFAULT};
   MoonPositionHelper                 mMoonPosition{};
   std::string                        mDataFileName{};
   ut::CloneablePtr<WsfDE_FileHandle> mDataFile{nullptr};
};

#endif // WSFMOONMONOPOLETERM_HPP
