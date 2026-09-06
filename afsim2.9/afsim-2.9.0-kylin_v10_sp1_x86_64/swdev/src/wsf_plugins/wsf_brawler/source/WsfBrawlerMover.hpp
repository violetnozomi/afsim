// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBRAWLERMOVER_HPP
#define WSFBRAWLERMOVER_HPP

#include "wsf_brawler_export.h"

#include "BrawlerMover.hpp"

class WsfBrawlerProcessor;
class WsfDraw;
#include "WsfFuel.hpp"
#include "WsfMover.hpp"
#include "WsfRoute.hpp"


//! This class defines a new type of mover that replicates the BRAWLER
//! AROTYP1 mover model. The inheritance from the base WsfMover provides
//! the interface for platforms to use the replicated BRAWLER mover
//! inherited from BrawlerMover.
class WSF_BRAWLER_EXPORT WsfBrawlerMover : public WsfMover, public BrawlerMover
{
public:
   WsfBrawlerMover(WsfScenario& aScenario);
   WsfBrawlerMover& operator=(const WsfBrawlerMover&) = delete;
   ~WsfBrawlerMover() override;

   //! Clone this object.
   //! @return a pointer to the new object.
   WsfMover* Clone() const override;

   void             SwapMoverInitiated(double aSimTime, WsfMover* aOldMoverPtr) override;
   bool             Initialize(double aSimTime) override;
   bool             ProcessInput(UtInput& aInput) override;
   void             Update(double aSimTime) override;
   WsfSpatialDomain GetSpatialDomain() const override;

   void TurnOn(double aSimTime) override;

   void DeleteMover() override;

   void UpdateFuel(double aTimeStep, double aInitialFlow, double aThrottle, double& aFuelQuantity, double& aVehicleMass) override;

   const char* GetScriptClassName() const override { return "WsfBrawlerMover"; }


   //! @name BRAWLER Flight methods.
   //! These methods provide the interface for external control of the mover.
   //! These are the WsfBrawlerMover implementations of the base class methods
   //! that provide the interface to fly BRAWLER maneuver alternatives.

   //@{
   bool FlyRates(const UtVec3d& aAngularRates,                // radians/sec
                 const UtVec3d& aLongitudinalAccel) override; // meters/sec^2

   bool FlyHeadingSpeedAltitude(double aHeading,            // deg
                                double aSpeed,              // meters/sec
                                double aAltitude,           // meters
                                double aMaxGees,            // meters/sec^2
                                double aMaxClimb) override; // meters/sec

   bool TurnInPlaneWithSpeed(const UtVec3d& aPlane,   // world coordinates (not body coordinates), meters
                             double         aGees,    // meters/sec^2
                             double         aSpeed) override; // meters/sec

   bool TurnInPlaneWithThrottle(const UtVec3d& aPlane,      // world coordinates (not body coordinates)
                                double         aGees,       // meters/sec^2
                                double         aThrottle) override; //[0-3]

   bool FlyVectorWithSpeed(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates) (magnitude does
                                                           // not matter, just direction)
                           double aMaxGees,                // meters/sec^2
                           double aSpeed) override;        // meters/sec

   bool FlyVectorWithThrottle(const UtVec3d& aVelocityVector, // world coordinates (not body coordinates) (magnitude
                                                              // does not matter, just direction)
                              double aMaxGees,                // meters/sec^2
                              double aThrottle) override;     //[0-3]
   //@}

   //! Projects the current state of the mover forward the specified time.
   //! This does not change the internal state of mover
   //! @param delta The time in seconds to project the mover forward.
   //! @param projected A UtEntity that is updated and returned with the forward projected state.
   bool ProjectForward(double delta, UtEntity& projected);

   /////////////////////////////////////////////////////////////////////////
   // WsfMover virtual methods
   /////////////////////////////////////////////////////////////////////////
   //! @name Navigation methods.
   //! These methods provide the interface for external control of the mover.
   //! These are the WsfBrawlerMover implementations of the base class methods.

   //@{
   const WsfRoute* GetRoute() const override { return &mRoute; }

   int RoutePointIndex() const override { return mCurrentRouteIndex; }

   bool GoToLocation(double aSimTime, double aLatitude, double aLongitude, double aAltitude) override;

   bool TurnToHeading(double                 aSimTime,
                      double                 aHeading,     // radians
                      double                 aRadialAccel, // meters/second^2
                      WsfPath::TurnDirection aTurnDirection) override;

   bool GoToSpeed(double aSimTime,
                  double aSpeed, // meters/sec
                  double aLinearAccel,
                  bool   aKeepRoute) override;

   bool GoToAltitude(double aSimTime,
                     double aAltitude, // meters
                     double aClimbDiveRate,
                     bool   aKeepRoute) override;

   bool TurnToRelativeHeading(double                 aSimTime,
                              double                 aHeadingChange,
                              double                 aRadialAccel,
                              WsfPath::TurnDirection aTurnDirection) override;

   // support route control
   bool ReturnToRoute(double aSimTime) override;
   // FollowRoute_1: FollowRoute(WsfRoute aRoute);
   // FollowRoute_2: FollowRoute(WsfRoute aRoute, string aRouteAction);
   // FollowRoute_3: FollowRoute(string aRouteName);
   // FollowRoute_4: FollowRoute(string aRouteName, string aRouteAction);
   // FollowRoute_5: FollowRoute(WsfRoute aRoute, int aIndex);
   // FollowRoute_6: FollowRoute(string aRouteName, int aIndex);
   // implementing UpdateRoute() will enable FollowRoute_1 & FollowRoute_3
   bool UpdateRoute(double aSimTime, const WsfRoute& aRoute) override;
   // also implementing GoToClosestPoint() & GoToLabel() will enable FollowRoute_2 & FollowRoute_4
   bool GoToClosestPoint(double aSimTime) override;
   bool GoToLabel(double aSimTime, WsfStringId aLabelId) override;
   // also implementing GoToWaypoint() will enable FollowRoute_5 & FollowRoute_6
   bool GoToWaypoint(double aSimTime, unsigned int aDestination) override;
   // bool SetRoute(double          aSimTime,
   //                       const WsfRoute& aRoute) override;

   // Return the brawler computed throttle position
   double GetThrottlePosition() override;

   bool Slice(double aDesiredHeading, double aDesiredAltitude, double aDesiredMach);

   bool Prlvl(double aDesiredHeading, double aDesiredMach);

   bool Prlvlg(double aDesiredHeading, double aDesiredMach, double aDesiredgees);

   void SetMaxThrottle(double aThrottle);

   //@}
   /////////////////////////////////////////////////////////////////////////


protected:
   // calculates how much excess power to use for climb rate compared to what to use for acceleration
   // returns the appropriate "max climb rate", favors achieving speed first over climb
   double ReasonableClimbRate(double desiredSpeed);

   //! Copy constructor used by Clone()
   WsfBrawlerMover(const WsfBrawlerMover& aSrc);

   bool     mDrawProjection;        //!< Flag indicating if visual aid of projection should be drawn
   WsfDraw* mDrawPtr;               //!< Pointer to a wsfDraw object to allow drawing of projection
   UtEntity mDebugEntityProjection; //!< A UtEntity for storing projection state

   WsfFuel* mFuelPtr; //!< Pointer to a fuel object on the platform

   WsfRoute mRoute;             //!< The route the mover will follow
   bool     mFlyingRoute;       //!< Flag indicating the mover is following its defined route
   int      mCurrentRouteIndex; //!< Index of the current spot in the route
};

#endif
