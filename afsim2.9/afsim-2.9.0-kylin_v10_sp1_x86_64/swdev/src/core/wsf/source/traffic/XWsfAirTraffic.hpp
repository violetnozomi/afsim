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

#ifndef XWSFAIRTRAFFIC_HPP
#define XWSFAIRTRAFFIC_HPP

#include "wsf_export.h"

#include <limits>
#include <map>
#include <string>
#include <vector>

class UtInput;
#include "WsfCallback.hpp"
#include "WsfEvent.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
class WsfRoute;
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
class WsfWaypoint;
#include "WsfSimulationExtension.hpp"

class WSF_EXPORT XWsfAirTrafficData
{
public:
   // The remaining classes describe data used by this class.

   class Destination
   {
   public:
      Destination()
         : mAirbaseNameId(nullptr)
         , mFraction(1.0)
      {
      }

      WsfStringId mAirbaseNameId; // String Id of the airbase name

      double mLatitude;  // The latitude of the local destination
      double mLongitude; // The longitude of the local destination
      double mRadius;

      double mFraction; // Fraction of departures going to this destination
   };

   typedef std::vector<Destination> Destinations;

   class Aircraft
   {
   public:
      Aircraft()
         : mAircraftTypeId(nullptr)
         , mFraction(1.0)
      {
      }
      WsfStringId mAircraftTypeId; // String Id of the aircraft type
      double      mFraction;       // Fraction of departures using this aircraft type
   };

   class Runway
   {
   public:
      Runway()
         : mBegLat(0.0)
         , mBegLon(0.0)
         , mBegHeight(0.0F)
         , mEndLat(0.0)
         , mEndLon(0.0)
         , mEndHeight(0.0F)
      {
      }

      double mMidLat;
      double mMidLon;
      double mBegLat;
      double mBegLon;
      float  mBegHeight;
      double mEndLat;
      double mEndLon;
      float  mEndHeight;
      double mHeading;
      double mLength;
      double mLatFAF;
      double mLonFAF;
      float  mAltFAF;
      double mLatIAF;
      double mLonIAF;
      float  mAltIAF;
      double mLatRightIAF;
      double mLonRightIAF;
      double mLatLeftIAF;
      double mLonLeftIAF;
   };

   class Airbase
   {
   public:
      Airbase()
         : mNameId(nullptr)
         , mLat(0.0)
         , mLon(0.0)
         , mDepartureInterval(300.0)
         , mDeactivationTime(std::numeric_limits<double>::max())
         , mRunway()
         , mDestinations()
         , mAircrafts()
         , mNextDepartureTime(0.0)
         , mFlightNumber(0)
         , mIsActive(true)
      {
      }

      WsfStringId           mNameId;            // String Id of the airbase name
      double                mLat;               // Latitude of the airbase
      double                mLon;               // Longitude of the airbase
      double                mDepartureInterval; // Time (secs) between departures
      double                mDeactivationTime;  // Simulation time when airbase is to be deactivated
      Runway                mRunway;
      Destinations          mDestinations;
      Destinations          mLocalDestinations;
      std::vector<Aircraft> mAircrafts;
      double                mNextDepartureTime; // Sim time (secs) of next departure
      unsigned int          mFlightNumber;      // Serial counter of flights from this airbase
      bool                  mIsActive;
   };

   class AircraftType
   {
   public:
      AircraftType()
         : mTypeId(nullptr)
         , mLocal(false)
         , mMinimumCruiseAltitude(0.0F)
         , mMaximumCruiseAltitude(0.0F)
         , mMeanCruiseSpeed(0.0F)
         , mSigmaCruiseSpeed(0.0F)
         , mMaximumOperatingRange(0.0F)
         , mMinimumRunwayLength(0.0F)
         , mLoiterTime(0.0)
         , mSigmaLoiterTime(0.0)
         , mDefaultLoiterTime(1800.0)
      {
      }
      WsfStringId              mTypeId;
      bool                     mLocal;
      float                    mMinimumCruiseAltitude;
      float                    mMaximumCruiseAltitude;
      float                    mMeanCruiseSpeed;
      float                    mSigmaCruiseSpeed;
      float                    mMaximumOperatingRange;
      float                    mMinimumRunwayLength;
      double                   mLoiterTime;
      double                   mSigmaLoiterTime;
      double                   mDefaultLoiterTime;
      std::vector<WsfStringId> mRouteIds;
   };

   class Flight
   {
   public:
      Flight()
         : mPlatformIndex(0)
         , mOriginationId(nullptr)
         , mDestinationId(nullptr)
         , mAircraftTypeId(nullptr)
         , mCruiseAltitude(0.0)
         , mCruiseSpeed(0.0)
         , mStatus(nullptr)
         , mDiversionPending(false)
      {
      }
      size_t      mPlatformIndex;
      WsfStringId mOriginationId;
      WsfStringId mDestinationId;
      WsfStringId mAircraftTypeId;
      double      mCruiseAltitude;
      double      mCruiseSpeed;
      WsfStringId mStatus;
      bool        mDiversionPending;
   };

   typedef std::map<WsfStringId, Airbase>      Airbases;
   typedef std::map<WsfStringId, AircraftType> AircraftTypes;

   XWsfAirTrafficData()
      : mEveryoneLandTime(std::numeric_limits<double>::max())
   {
   }

   bool ProcessInput(UtInput& aInput);
   void ProcessAirbaseBlock(UtInput& aInput);
   void ProcessAircraftTypeBlock(UtInput& aInput);

   static double NormalizeHeading(double aHeading);
   bool          IsRequested() const;

   double        mEveryoneLandTime;
   Airbases      mAirbases;
   AircraftTypes mAircraftTypes;
};

class WSF_EXPORT XWsfAirTraffic : public WsfSimulationExtension, public XWsfAirTrafficData
{
public:
   XWsfAirTraffic(WsfSimulation& aSimulation, XWsfAirTrafficData& aData);
   ~XWsfAirTraffic() override;

   bool Initialize() override;

   void UpdateFlightStatus(double aSimTime, WsfPlatform* aPlatformPtr, WsfStringId& aFlightStatus);

   // void ProcessDepartureEvent(int    aAirbaseId,
   //                            double aSimTime);

   double ProcessEvent(double aSimTime);

   void ProcessDivertFlights(double aSimtime);

private:
   // Copy constructed declared but not defined to prevent use.
   XWsfAirTraffic(const XWsfAirTraffic& aSrc);

   // Assignment operator declared but not defined to prevent use.
   XWsfAirTraffic& operator=(const XWsfAirTraffic& aRhs) = delete;

public:
   // An air traffic event is scheduled for the next time a takeoff
   // or airbase deactivation is to occur.

   class AirTrafficEvent : public WsfEvent
   {
   public:
      AirTrafficEvent(double aSimTime, XWsfAirTraffic* aAirTrafficPtr)
         : WsfEvent(aSimTime)
         , mAirTrafficPtr(aAirTrafficPtr)
      {
      }

      EventDisposition Execute() override
      {
         double           nextEventTime = mAirTrafficPtr->ProcessEvent(GetTime());
         EventDisposition disposition   = cDELETE;
         if (nextEventTime >= 0.0)
         {
            SetTime(nextEventTime);
            disposition = cRESCHEDULE;
         }
         return disposition;
      }

   private:
      XWsfAirTraffic* mAirTrafficPtr;
   };

   class UpdateFlightStatusCallback : public WsfCallback
   {
   public:
      UpdateFlightStatusCallback(XWsfAirTraffic* aAirTrafficPtr, WsfStringId& aFlightStatus)
         : WsfCallback()
         , mAirTrafficPtr(aAirTrafficPtr)
         , mFlightStatus(aFlightStatus)
      {
      }

      WsfCallback* Clone() const override { return new UpdateFlightStatusCallback(*this); }

      bool Execute(double aSimTime, WsfPlatform* aPlatformPtr) override
      {
         mAirTrafficPtr->UpdateFlightStatus(aSimTime, aPlatformPtr, mFlightStatus);
         return true;
      }

   private:
      XWsfAirTraffic* mAirTrafficPtr;
      WsfStringId     mFlightStatus;
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
               moverPtr->GoToLabel(GetTime(), "XWSF_AIR_TRAFFIC_GO_HOME");
            }
         }
         return cDELETE;
      }

   private:
      WsfStringId mPlatformId;
   };

private: // Revert back to 'private' (For Sun Workshop compiler)
   void AddCallback(WsfPlatform* aPlatformPtr, WsfStringId& aCallbackName);

   void AddToSimulation(const Aircraft&    aAircraft,
                        bool               aLocalAircraft,
                        Airbase&           aAirbase,
                        const Destination& aDestination,
                        double             aSimTime);

   double ComputeHeadingChange(double aCurrentHeading, double aTargetHeading) const;

   void ComputeRunwayParameters(Runway& aRunway);

   //! Adds details to the notes of aMessageStream
   void DivertFlight(double aSimTime, Flight& aFlight);

   unsigned int FindAirbase(WsfStringId aAirbaseNameId) const;

   void GenerateAscentPath(const Airbase& aOrigination,
                           const Airbase& aDestination,
                           float          aCruiseAlt,
                           float          aCruiseSpeed,
                           WsfRoute&      aRoute);

   void GenerateAscentPath(const Airbase& aAirbase, float aCruiseSpeed, WsfRoute& aRoute);

   void GenerateLoiterPath(const std::vector<WsfStringId>& aRouteIds,
                           const Destination&              aDestination,
                           float                           aCruiseAlt,
                           float                           aCruiseSpeed,
                           WsfRoute&                       aRoute);

   void GenerateDescentPath(WsfRoute&      aRoute,
                            double         aCurrentLat,
                            double         aCurrentLon,
                            double         aCurrentAlt,
                            double         aCurrentSpeed,
                            const Airbase& aDestination);

   void GenerateDiversionPath(Flight& aFlight);

   bool GetRandomDestination(const Airbase& aAirbase, const Destinations& aDestinations, Destination& aDestination);

   bool GetRandomAircraft(const Airbase& aAirbase, const Airbase& aDestAirbase, Aircraft& aAircraft);

   bool GetRandomLocalAircraft(const Airbase& aAirbase, Aircraft& aAircraft);

   bool DrawLocalOrRemote(const Airbase& aAirbase);

   void GetRandomWaypoint(const Destination& aDestination, WsfWaypoint& aWaypoint);

   double GetRandomLoiterTime(const AircraftType& aAircraftType);

   bool DoesAirbaseExist(WsfStringId aTypeId) const { return (mAirbases.find(aTypeId) != mAirbases.end()); }
   bool DoesAircraftTypeExist(WsfStringId aTypeId) const
   {
      return (mAircraftTypes.find(aTypeId) != mAircraftTypes.end());
   }

   WsfStringId mTakeOff;
   WsfStringId mAscending;
   WsfStringId mCruising;
   WsfStringId mDescending;
   WsfStringId mTouchdown;
   WsfStringId mComplete;
   WsfStringId mEnterLoiter;
   WsfStringId mGoHome;


   std::vector<Flight> mFlights;

   int mEveryoneLand;
};

#endif
