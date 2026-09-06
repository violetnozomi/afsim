// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef XWSFSEATRAFFIC_HPP
#define XWSFSEATRAFFIC_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

#include "UtCallbackHolder.hpp"
class UtInput;
#include "UtMath.hpp"
#include "WsfEvent.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfRoute.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationExtension.hpp"
#include "WsfStringId.hpp"
class WsfWaypoint;
class XWsfSeaTrafficExtension;

//------------------------------------------------------------------------
//! XWsfSeaTraffic provides the ability to create sea based background
//! traffic.  It uses an input syntax defined by the sea_traffic ...
//! end_sea_traffic input block, which is described in the WSF User
//! Input Guide.
//------------------------------------------------------------------------

class WSF_EXPORT XWsfSeaTraffic : public WsfSimulationExtension
{
public:
   friend class XWsfSeaTrafficExtension;
   class SeaRegion;
   class SeaPort;
   class SeaLane;
   class Vehicle;

   XWsfSeaTraffic();
   XWsfSeaTraffic(const XWsfSeaTraffic& aSrc, WsfSimulation* aSimPtr);
   virtual XWsfSeaTraffic* Clone() const;

   ~XWsfSeaTraffic() override;

   virtual bool ProcessInput(UtInput& aInput);
   bool         Initialize() override;

   void HandleDeparture(WsfStringId aPortId, double aSimTime);

   void HandleEnterLoiter(WsfPlatform* aPlatformPtr, double aSimTime);

   void GetSeaLaneIds(WsfStringId aPortId, std::vector<WsfStringId>& aLaneIds);

   //! Returns true if the specified port exits.
   //! aNameId: The string id of the port name.
   bool DoesPortExist(WsfStringId aNameId) const { return mSeaPorts.find(aNameId) != mSeaPorts.end(); }

   //! Returns true if the specified lane exits.
   //! aNameId: The string id of the lane name.
   bool DoesLaneExist(WsfStringId aNameId) const { return mSeaLanes.find(aNameId) != mSeaLanes.end(); }

   //! Returns true if the specified departure vehicle exits.
   //! aTypeId: The string id of the vehicle type.
   bool DoesDepVehicleExist(WsfStringId aTypeId) const { return mDepVehicles.find(aTypeId) != mDepVehicles.end(); }

   //! Returns true if the specified local vehicle exits.
   //! aTypeId: The string id of the vehicle type.
   bool DoesLocVehicleExist(WsfStringId aTypeId) const { return mLocVehicles.find(aTypeId) != mLocVehicles.end(); }

   //! Returns a pointer to the specified port.
   //! aNameId: The string id of the port name.
   SeaPort* GetSeaPort(WsfStringId aNameId)
   {
      return mSeaPorts.find(aNameId) != mSeaPorts.end() ? &mSeaPorts[aNameId] : nullptr;
   }

   //! Returns a pointer to the specified lane.
   //! aNameId: The string id of the lane name.
   SeaLane* GetSeaLane(WsfStringId aNameId)
   {
      return mSeaLanes.find(aNameId) != mSeaLanes.end() ? &mSeaLanes[aNameId] : nullptr;
   }

   //! Returns a pointer to the specified departure vehicle.
   //! aTypeId: The string id of the vehicle type.
   Vehicle* GetDepVehicle(WsfStringId aTypeId)
   {
      return mDepVehicles.find(aTypeId) != mDepVehicles.end() ? &mDepVehicles[aTypeId] : nullptr;
   }

   //! Returns a pointer to the specified local vehicle.
   //! aTypeId: The string id of the vehicle type.
   Vehicle* GetLocVehicle(WsfStringId aTypeId)
   {
      return mLocVehicles.find(aTypeId) != mLocVehicles.end() ? &mLocVehicles[aTypeId] : nullptr;
   }

   static const double cEPSILON;

   //! Nested Class
   //! Defines a geographical position and a radius.
   class SeaRegion
   {
   public:
      SeaRegion()
         : mLat(0.0)
         , mLon(0.0)
         , mRadius(0.0)
         , mFraction(0.0)
      {
      }

      SeaRegion(double aLat, double aLon, double aRadius, double aFraction = 0.0)
         : mLat(aLat)
         , mLon(aLon)
         , mRadius(aRadius)
         , mFraction(aFraction)
      {
      }

      ~SeaRegion() {}

      bool ProcessInput(UtInput& aInput);

      double mLat;
      double mLon;
      double mRadius;
      double mFraction;
   };

   //! Nested Class
   //! Defines a sea port by a position and optionally a path that leads
   //! to the port location. Ports contains a list of sea lanes that
   //! connect the port with other ports.
   class SeaPort
   {
   public:
      SeaPort()
         : mNameId()
      {
         // provided for map[]
      }

      SeaPort(WsfStringId aNameId)
         : mNameId(aNameId)
         , mDepartureInterval(0.0)
         , mDepVehiclesInterval(0.0)
         , mLocVehiclesInterval(0.0)
         , mUseAllLanes(false)
      {
      }

      ~SeaPort() {}

      bool ProcessInput(UtInput& aInput);
      bool Initialize(XWsfSeaTraffic& aSeaTraffic);

      bool operator==(const SeaPort& aRhs) const { return mNameId == aRhs.mNameId; }

      WsfStringId              mNameId;
      SeaRegion                mPosition;
      std::vector<SeaRegion>   mPortRoute;
      std::vector<SeaRegion>   mLocalTrafficRegions;
      double                   mDepartureInterval;
      double                   mDepVehiclesInterval;
      double                   mLocVehiclesInterval;
      bool                     mUseAllLanes;
      std::vector<WsfStringId> mSeaLaneIds;
      std::vector<double>      mDstPortWeights;
      std::vector<WsfStringId> mAirportIds;
   };

   //! Nested Class
   //! Defines a sea lane by two ports, which define the endpoints
   //! and optionally an intermediate path connecting the two endpoints.
   class SeaLane
   {
   public:
      SeaLane()
      {
         // provided for map[]
      }
      SeaLane(WsfStringId aNameId)
         : mNameId(aNameId)
         , mPortCount(0)
      {
         mPortId[0]          = nullptr;
         mPortId[1]          = nullptr;
         mPortIgnoreRoute[0] = false;
         mPortIgnoreRoute[1] = false;
      }

      ~SeaLane() {}

      bool ProcessInput(UtInput& aInput);
      bool Initialize(XWsfSeaTraffic& aSeaTraffic);

      //! Returns the destination port's string id.
      //! aPortId: The origination port's string id.
      WsfStringId GetDestinationPort(WsfStringId aPortId) const
      {
         return (aPortId == mPortId[0]) ? mPortId[1] : mPortId[0];
      }

      //! Returns true if we should ignore the port route.
      //! aPortId: The port's string id.
      bool IgnorePortRoute(WsfStringId aPortId) const
      {
         return (aPortId == mPortId[0]) ? mPortIgnoreRoute[0] : mPortIgnoreRoute[1];
      }

      bool operator==(const SeaLane& aRhs) const { return mNameId == aRhs.mNameId; }

      WsfStringId            mNameId;
      int                    mPortCount;
      WsfStringId            mPortId[2];
      bool                   mPortIgnoreRoute[2];
      std::vector<SeaRegion> mLaneRoute;
   };

   //! Nested Class
   //! Defines a type of Vehicle.
   class Vehicle
   {
   public:
      Vehicle()
      {
         // provided for map[]
      }
      Vehicle(WsfStringId aTypeId)
         : mTypeId(aTypeId)
         , mCount(0)
         , mFraction(0.0)
         , mLoiterTime(0.0)
         , mDefaultLoiterTime(1800.0)
         , // 1/2 hour
         mSigmaLoiterTime(0.0)
         , mSpeed(0.0)
         , mDefaultSpeed(10.0 * UtMath::cFT_PER_NM / UtMath::cFT_PER_M)
         , // 10.0 knots
         mSigmaSpeed(0.0)
      {
      }

      ~Vehicle() {}

      bool ProcessInput(UtInput& aInput);
      bool Initialize(XWsfSeaTraffic& aSeaTraffic);

      bool operator==(const Vehicle& aRhs) const { return mTypeId == aRhs.mTypeId; }

      WsfStringId mTypeId;
      int         mCount;
      double      mFraction;
      double      mLoiterTime;
      double      mDefaultLoiterTime;
      double      mSigmaLoiterTime;
      double      mSpeed;
      double      mDefaultSpeed;
      double      mSigmaSpeed;

      std::vector<WsfStringId> mRouteIds;
   };

   //! Nested class
   //! Define an event class that handles a departure from a port.
   class DepartureEvent : public WsfEvent
   {
   public:
      DepartureEvent(XWsfSeaTraffic* aSeaTrafficPtr, WsfStringId aPortId, double aSimTime)
         : WsfEvent(aSimTime)
         , mSeaTrafficPtr(aSeaTrafficPtr)
         , mPortId(aPortId)
      {
      }

      EventDisposition Execute() override
      {
         mSeaTrafficPtr->HandleDeparture(mPortId, GetTime());
         return cDELETE;
      }

   private:
      XWsfSeaTraffic* mSeaTrafficPtr;
      WsfStringId     mPortId;
   };

   //! Nested class
   //! Define an event class that handles exiting from a loiter pattern by sending
   //! the platform to the waypoint labeled as home.
   class ExitLoiterEvent : public WsfEvent
   {
   public:
      ExitLoiterEvent(WsfStringId aPlatformId, double aSimTime)
         : WsfEvent(aSimTime)
         , mPlatformId(aPlatformId)
      {
      }

      EventDisposition Execute() override
      {
         WsfPlatform* platformPtr = GetSimulation()->GetPlatformByName(mPlatformId);
         if (platformPtr != nullptr)
         {
            WsfMover* moverPtr = platformPtr->GetMover();
            if (moverPtr != nullptr)
            {
               moverPtr->GoToLabel(GetTime(), WsfStringId("XWSF_SEA_TRAFFIC_HOME_LABEL"));
            }
         }
         return cDELETE;
      }

   private:
      WsfStringId mPlatformId;
   };

private:
   typedef std::map<WsfStringId, SeaPort> Ports;
   typedef std::map<WsfStringId, SeaLane> Lanes;
   typedef std::map<WsfStringId, Vehicle> Vehicles;

   bool ProcessSeaTraffic(UtInput& aInput);

   void ProcessDepartureTraffic(UtInput& aInput);

   void ProcessLocalTraffic(UtInput& aInput);

   void GenerateInitialEntities();

   bool GenerateDeparture(SeaPort& aPort, bool aLocalDeparture, Vehicle& aVehicle, double aDistanceAlongRoute, double aSimTime);

   bool GenerateEntity(Vehicle& aVehicle, WsfRoute& aRoute, double aSimTime);

   void UpdateRoute(double aDistanceAlongRoute, WsfRoute& aRoute);

   Vehicle* GetRandomVehicle(Vehicles& aVehicles);

   SeaLane* GetRandomLane(SeaPort& aPort);

   void GetRandomRoute(SeaLane&  aLane,
                       SeaPort&  aSrcPort,
                       SeaPort&  aDstPort,
                       double    aSpeed,
                       WsfRoute& aRoute,
                       bool      aActualRoute = false);

   void GetRandomRoute(SeaPort& aPort, SeaRegion& aRegion, Vehicle& aVehicle, double aSpeed, WsfRoute& aRoute);

   void GetRandomWaypoint(SeaRegion& aRegion, WsfWaypoint& aWaypoint, bool aActualWaypoint = false);

   void GetRandomRegion(SeaPort& aPort, SeaRegion& aRegion);

   double GetRandomSpeed(Vehicle& aVehicle);

   double GetRandomLoiterTime(Vehicle& aVehicle);

   double CalcMaxTravelTime(SeaPort& aPort, Vehicles& aVehicles);

   void WriteScenarioEditorFile();

   // Disable copying and assignment.
   XWsfSeaTraffic(const XWsfSeaTraffic& aSrc);
   XWsfSeaTraffic& operator=(const XWsfSeaTraffic& aRhs);

   bool mProcessedInput;

   double mDepVehiclesFraction;
   double mLocVehiclesFraction;

   Ports    mSeaPorts;
   Lanes    mSeaLanes;
   Vehicles mDepVehicles;
   Vehicles mLocVehicles;
};

#endif
