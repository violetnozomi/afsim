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

#ifndef WSFROCKETORBITALMANEUVERING_HPP
#define WSFROCKETORBITALMANEUVERING_HPP

#include "wsf_space_export.h"

#include "WsfOrbitalManeuvering.hpp"

//! An extension of the base class for orbital maneuvering, utilizing the rocket equation.
class WSF_SPACE_EXPORT WsfRocketOrbitalManeuvering : public WsfOrbitalManeuvering
{
public:
   WsfRocketOrbitalManeuvering() = default;
   WsfRocketOrbitalManeuvering(const WsfRocketOrbitalManeuvering& aSrc);
   WsfRocketOrbitalManeuvering& operator=(const WsfRocketOrbitalManeuvering&) = delete;
   ~WsfRocketOrbitalManeuvering() override;

   WsfOrbitalManeuvering* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(const UtCalendar& aEpoch) override;

   void SetPlatformAttributes(WsfPlatform* aPlatform) const override;

   //! @name Utility methods that are optionally supported in derived classes.
   //@{
   double GetRequiredDuration(double aDeltaV) const override;
   double GetRequiredDeltaV(double aDuration) const override;
   double GetAvailableDeltaV() const override;
   double GetAvailableDuration() const override;
   bool   ReduceAvailableDeltaV_By(double aDeltaV) override;
   bool   PerformStagingOperation() override;
   //@}

   const double GetTotalMass() const { return mRocket.GetTotalMass(); }

protected:
   // The stage class is meant to be a simplified version of the WsfGuidedMover::Stage class.
   // Todo constuct or initialize a stage from a WsfGuidedMover::Stage.
   class Stage
   {
   public:
      Stage();
      ~Stage() = default;

      bool Initialize();
      bool ProcessInput(UtInput& aInput);

      double GetDuration(double aTotalMass, double aDeltaV) const;
      double GetDeltaV(double aTotalMass, double aDuration) const;
      double GetAvailableDeltaV(double aTotalMass) const;
      double GetAvailableDuration() const { return mFuelMass / mBurnRate; }
      double GetEmptyMass() const { return mEmptyMass; }
      double GetFuelMass() const { return mFuelMass; }
      double GetTotalMass() const { return mTotalMass; }
      double GetBurnRate() const { return mBurnRate; }
      double GetThrust() const { return mThrust; }
      double GetSpecificImpulse() const;
      double GetExhaustVelocity() const { return mExhaustVelocity; }
      double Thrust(double aTotalMass, double aDuration, double& aDeltaV);

   protected:
      //! Mass of the THIS stage minus the mass of the fuel on THIS.
      //! @note This does NOT include the mass of the subsequent stages.
      double mEmptyMass;

      //! Mass of just the fuel on THIS stage.
      //! @note This does NOT include the mass of the subsequent stages.
      double mFuelMass;

      //! The total mass of the THIS stage (empty mass plus fuel mass).
      //! @note This does NOT include the mass of the subsequent stages.
      double mTotalMass;

      //! The rate of fuel expenditure (Kg/Sec); this can be provided or derived from the exhaust velocity.
      double mBurnRate;

      //! The thrust (N); this must be provided
      double mThrust;

      //! The exhaust velocity (m/s); this can be provided or derived from the burn rate.
      double mExhaustVelocity;
   };

   // A Rocket is a sequence of stages
   class Rocket
   {
   public:
      Rocket();
      Rocket(const Rocket& aSrc);
      Rocket& operator=(const Rocket&) = delete;
      ~Rocket()                        = default;

      bool Initialize();
      bool ProcessInput(UtInput& aInput);

      double Thrust(double aDuration, double& aDeltaV);
      double GetDuration(double aDeltaV) const;
      double GetDeltaV(double aDuration) const;
      double GetAvailableDeltaV() const;
      double GetAvailableDuration() const;
      double GetTotalMass() const;
      double GetFuelMass() const;
      double GetEmptyMass() const;
      double GetPayloadMass() const;
      bool   PerformStagingOperation();

   private:
      typedef std::vector<Stage> Stages;
      double                     GetTotalMass(const Stages::iterator aFromStage) const;

      // bool                       mPerformAutomaticStaging;
      Stages           mStages;
      Stages::iterator mCurrentStageIter;
   };

private:
   Rocket mRocket;
};

#endif
