// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALMANEUVERING_HPP
#define WSFORBITALMANEUVERING_HPP

#include "wsf_space_export.h"

#include "UtCalendar.hpp"
#include "UtVec3.hpp"
#include "WsfObject.hpp"
class WsfOrbitalManeuver;
class WsfPlatform;
class WsfScenario;

//! Base class for orbital maneuvering functions.  It serves both to execute a
//! WsfManeuverSequence and is used by the WsfSpaceMoverBase as well as in a
//! standalone role external to the simulation.  In this latter capacity
//! that the maneuver sequence can be verified using only a compatible
//! UtOrbitalPropagatorBase derived class (typically UtOrbitalPropagator).
class WSF_SPACE_EXPORT WsfOrbitalManeuvering : public WsfObject
{
public:
   WsfOrbitalManeuvering()                                  = default;
   WsfOrbitalManeuvering(const WsfOrbitalManeuvering& aSrc) = default;
   WsfOrbitalManeuvering& operator=(const WsfOrbitalManeuvering&) = delete;
   ~WsfOrbitalManeuvering() override                              = default;

   WsfOrbitalManeuvering* Clone() const override = 0;

   bool ProcessInput(UtInput& aInput) override;

   virtual bool Initialize(const UtCalendar& aEpoch);

   virtual void SetPlatformAttributes(WsfPlatform* aPlatform) const {};

   //! Constant value representing infinite delta-V; used to compare against values computed for maneuvers.
   static constexpr double cINFINITE_DELTA_V = std::numeric_limits<double>::max();

   //! Constant value representing infinite maneuver duration; used to compare against values computed for maneuvers.
   static constexpr double cINFINITE_DURATION = std::numeric_limits<double>::max();

   //! @name Utility methods that are optionally supported in derived classes.
   //@{
   virtual double GetAvailableDeltaV() const;
   virtual double GetAvailableDuration() const;
   virtual bool   ReduceAvailableDeltaV_By(double aDeltaV);
   virtual bool   Maneuver(const UtCalendar&         aEpoch,
                           const WsfOrbitalManeuver& aManeuver,
                           const UtVec3d&            aCommandedDeltaV,
                           UtVec3d&                  aActualDeltaV);
   virtual bool   PerformStagingOperation();

   //! Given a delta-v, return the required time to complete the maneuver.
   virtual double GetRequiredDuration(double aDeltaV) const = 0;

   //! Given a maneuver duration, return the necessary delta-v.
   virtual double GetRequiredDeltaV(double aDuration) const = 0;
   //@}

   const UtCalendar& GetUpdateTime() const { return mUpdateTime; }

   double GetManeuverDuration(const UtCalendar& aEpoch, const WsfOrbitalManeuver& aManeuver, double aDeltaV) const;

protected:
   bool       mDebug{false}; // debug flag
   UtCalendar mUpdateTime{};
};

#endif
