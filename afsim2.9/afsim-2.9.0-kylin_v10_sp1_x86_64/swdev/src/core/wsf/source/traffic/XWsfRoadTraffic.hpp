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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef XWSFROADTRAFFIC_HPP
#define XWSFROADTRAFFIC_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class UtInput;
#include "UtCallbackHolder.hpp"
#include "UtRandom.hpp"
class WsfMover;
class WsfPlatform;
class WsfRoadMover;
#include "WsfRoute.hpp"
class WsfRouteNetwork;
#include "WsfSimulationExtension.hpp"
#include "XWsfRouteNetwork.hpp"

//! XWsfRoadTraffic provides a basic background traffic capability.  Based on the provided input
//! file a specified number of ground vehicles are created and placed randomly on the specified
//! road network (a vehicle density can also be specified.  Weighted regions can be specified
//! so that vehicles populate those regions with a higher density.  Vehicle type, velocity,
//! travel time, and quantity can be specified.  In  addition, vehicles can travel offroad at the
//! end of their route or runtime and pause for a given amount of time.  The initial number of
//! vehicles is always maintained, so as a vehicle is removed from the simulation another one is added.
class WSF_EXPORT XWsfRoadTraffic : public WsfSimulationExtension
{
public:
   struct Data;

   XWsfRoadTraffic(const Data& aData)
      : mData(aData)
   {
   }

   ~XWsfRoadTraffic() override;

   bool Initialize() override;

   void AdvanceTime(double aSimTime);

   using WsfSimulationExtension::GetScenario;

   struct SGroundTraffic
   {
      int          mIdentifier        = 0;       //! Unique identifier for ground vehicle
      WsfStringId  mVehicleTypeId     = nullptr; //! Vehicle type
      bool         mAlive             = false;   //! Is this vehicle alive?
      bool         mVehicleOffRoad    = false;   //! Is this vehicle now off-road?
      bool         mStationary        = false;   //! Is this vehicle stationary?
      int          mNetwork           = -1;      //! Index value of network vehicle is traveling on
      double       mRouteDist         = 0.0;     //! The length of the path (meters)
      double       mSpeed             = 0.0;     //! Speed (meters/sec)
      double       mTravelTime        = 0.0;     //! Amount of time vehicle is moving along road (sec)
      double       mStartTime         = 0.0;     //! Time vehicle starts moving (sec)
      double       mDeadTime          = 0.0;     //! Time vehicle stops moving and is removed (sec)
      double       mOffRoadTime       = 0.0;     //! Time vehicle starts moving off the road (sec)
      WsfPlatform* mRoadPlatform      = nullptr;
      size_t       mRoadPlatformIndex = 0;
      bool         mConvoyVehicle     = false; //! Was this vehicle defined as a convoy vehicle?
   };

   //! A WeightedRegionInput object is created for each of the weighted regions
   //! defined in the input file. A list of WeighedRegionInput objects are stored in the
   //! NetworkInput object that they were defined under.
   struct WeightedRegionInput
   {
      double mLatitude    = 0.0;
      double mLongitude   = 0.0;
      double mInnerRadius = 0.0;
      double mOuterRadius = 0.0;
      double mInnerWeight = 0.0;
      double mOuterWeight = 0.0;
   };

   //! A VehicleInput object is created for each of the vehicles defined in
   //! the input file.  A list of VehicleInput objects are stored in the NetworkInput
   //! object that they were defined under.
   class VehicleInput
   {
   public:
      VehicleInput() = default;
      VehicleInput(WsfStringId aTypeId)
         : mTypeId(aTypeId)
      {
      }

      WsfStringId mTypeId     = nullptr; //! String Id of the type of the vehicle
      int         mCount      = 0;       //! The number of vehicles that should be created of this type.
      double      mFraction   = 0.0;     //! The percentage of this vehicle type that should exist in the simulation.
      double      mMeanSpeed  = 0.0;     //! Mean velocity in m/s
      double      mSigmaSpeed = 0.0;     //! Sigma velocity in m/s
   };

   //! A ConvoyInput object is created for each of the convoys defined in the input file.
   //! A list of ConvoyInput objects are stored in the NetworkInput object that they
   //! are defined under.
   struct ConvoyInput
   {
      double                    mStartLatitude      = 0.0;
      double                    mStartLongitude     = 0.0;
      double                    mEndLatitude        = 0.0;
      double                    mEndLongitude       = 0.0;
      double                    mSpacing            = 0.0;
      double                    mSpeed              = 0.0;
      double                    mPauseTime          = 0.0;
      bool                      mUseClosestWaypoint = false;
      std::vector<VehicleInput> mVehicleInput;
   };

   //! WeightedRouteSegment represents the concept of a weighted edge in a graph.
   //! It includes the two endpoints and their respective weights as well as the edge's
   //! weight. The NetworkInput class includes a list of WeightedRouteSegments that define
   //! the network graph.
   class WeightedRouteSegment
   {
   public:
      WeightedRouteSegment() = default;
      WeightedRouteSegment(WsfRoute* aRoutePtr, const WsfWaypoint& aNode1, const WsfWaypoint& aNode2)
         : mRoutePtr(aRoutePtr)
         , mNode1(aNode1)
         , mNode2(aNode2)
      {
      }

      WsfRoute*   mRoutePtr = nullptr;
      WsfWaypoint mNode1;
      WsfWaypoint mNode2;
      double      mNode1Weight      = 0.0;
      double      mNode2Weight      = 0.0;
      double      mSegmentWeight    = 0.0;
      double      mCumulativeWeight = 0.0;

      //! Equal if the provided weight is enclosed by this segment's boundary weights.
      bool operator==(double aWeight) const
      {
         return ((aWeight >= mCumulativeWeight) && (aWeight < (mCumulativeWeight + mSegmentWeight)));
      }

      bool operator<(const WeightedRouteSegment& aSegment) const { return (mSegmentWeight < aSegment.mSegmentWeight); }
   };

   //! A NetworkInput object is created for each network defined in the input file.  It holds
   //! data that is relevant to anything in its network, such as roads and vehicles.
   class RoadTrafficNetworkInput : public XWsfRouteNetwork::NetworkInput
   {
   public:
      RoadTrafficNetworkInput() = default;
      RoadTrafficNetworkInput(WsfStringId aNetworkId)
         : NetworkInput(aNetworkId)
      {
      }

      NetworkInput* Clone() const noexcept override { return new RoadTrafficNetworkInput{*this}; }

      void VerifyData(UtInput& aInput) override
      {
         NetworkInput::VerifyData(aInput);

         double                              sum = 0.0;
         std::vector<VehicleInput>::iterator vehItr;
         for (vehItr = mVehicleInput.begin(); vehItr != mVehicleInput.end(); ++vehItr)
         {
            sum += vehItr->mFraction;
         }
         if (fabs(1.0 - sum) > 10E-3)
         {
            throw UtInput::BadValue(aInput, "XWsfRoadTraffic the sum of all the vehicle 'fraction' values must be 1.0");
         }
      }

      double                           mMinDistOffRoad  = 0.0; //! Minimum distance vehicle travels off road (meters)
      double                           mMaxDistOffRoad  = 0.0; //! Maximum distance vehicle travels off road (meters)
      double                           mPauseTime       = 0.0; //! Pause time after completing travel (seconds)
      double                           mMaxSpeed        = 0.0; //! Maximum speed of any vehicle (m/s)
      double                           mMeanTravelTime  = 0.0; //! Mean travel time of vehicles (s)
      double                           mSigmaTravelTime = 0.0; //! Sigma travel time of vehicles (s)
      double                           mTotalDist       = 0.0; //! The sum of the lengths of all the roads (meters)
      std::vector<VehicleInput>        mVehicleInput;        //! Data for each of the vehicle types on this road network
      std::vector<WeightedRegionInput> mWeightedRegionInput; //! Data for each of the weighted regions in this road network.

      //! A list of weighted segments used to place bodies on the road
      //! network, with higher densities favoring larger edge weights.
      std::vector<WeightedRouteSegment> mWeightedRouteSegments;

      std::vector<ConvoyInput> mConvoyInput; //! Data for each of the convoys in this road network.

   protected:
      RoadTrafficNetworkInput(const RoadTrafficNetworkInput& aSrc) = default;
   };

   enum EndOfPath
   {
      cRESPAWN,
      cREVERSE_DIRECTION
   };

   //! Holds the data for XWsfRoadTraffic populated during the ProcessInput phase
   struct Data : public XWsfRouteNetwork
   {
      bool      mProcessedInput  = false;
      bool      mDebug           = false;
      bool      mVerbose         = false;
      EndOfPath mEndOfPathOption = cRESPAWN;
      // Vector of vehicles.
      std::vector<SGroundTraffic> mTraffic;
      double                      mNextUpdateTime = 0.0; // Simulation time when the next update
                                                         // (platform add, delete, pause, etc) is required.
      int mNextVehicleIdentifier = 0;
      int mNumVehiclesActive     = 0;
      int mMaxVehiclesActive     = 0;

      //! Get the route networks
      //! @return the route networks
      Networks& GetNetworks() { return mNetworks; }
      //! Get the route networks
      //! @return the route networks
      const Networks& GetNetworks() const { return mNetworks; }

      using XWsfRouteNetwork::ProcessCommand;
   };

protected:
   //! Copy constructor and assignment operator are declared but not defined in order to prevent use.
   XWsfRoadTraffic(const XWsfRoadTraffic& aSrc) = delete;
   XWsfRoadTraffic& operator=(const XWsfRoadTraffic& aRhs) = delete;

private:
   void BuildWeightedRouteSegmentList(WsfRouteNetwork& aRouteNetwork, RoadTrafficNetworkInput& aNetwork);

   double CalculateNodeWeight(const WeightedRegionInput& aWeightedRegion, const WsfWaypoint& aNode);

   void AddPlatformToSimulation(double aSimTime, int aVehicleNumber);

   int AllocateVehicle(WsfStringId aVehicleTypeId);

   bool GetRandomWaypoint(const RoadTrafficNetworkInput& aNetwork,
                          WsfWaypoint&                   aWaypoint,
                          WsfWaypoint&                   aRouteSegEndWpt1,
                          WsfWaypoint&                   aRouteSegEndWpt2);

   bool CreateRandomPath(const RoadTrafficNetworkInput& aNetwork, WsfRoute& aPath);

   double CreateRandomPathForVehicle(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber);

   void GoOffRoad(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber);

   void ClearVehicleList();

   bool CreateVehicle(const RoadTrafficNetworkInput& aNetworkInput, double aCreateTime, int& aVehicleNumber);

   bool CreateConvoyVehicle(WsfStringId aVehicleType, int& aVehicleNumber);

   void DeletePlatformFromSimulation(double aSimTime, int aVehicleNumber);

   void ReverseVehicleRoute(int aVehicleNumber, double aSimTime);

   bool ValidateVehicleTypes(RoadTrafficNetworkInput& aNetwork);

   WsfMover* GetMover(WsfPlatform* aMoverPtr);

   //! Data used by the extension (stored in the corresponding ScenarioExtension)
   //! @see Data
   Data             mData;
   UtCallbackHolder mCallbacks;
};

#endif
