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
#include "WsfModule.hpp"
#include "XWsfRouteNetwork.hpp"

class WSF_EXPORT XWsfRoadTrafficData
{
public:
   // The following classes must be public to prevent a error with the Sun Workshop compiler.
   class WeightedRouteSegment;


   //! A WeightedRegionInput object is created for each of the weighted regions
   //! defined in the input file. A list of WeighedRegionInput objects are stored in the
   //! NetworkInput object that they were defined under.
   class WeightedRegionInput
   {
   public:
      WeightedRegionInput()
         : mLatitude(0.0)
         , mLongitude(0.0)
         , mInnerRadius(0.0)
         , mOuterRadius(0.0)
         , mInnerWeight(0.0)
         , mOuterWeight(0.0)
      {
      }

      double mLatitude;
      double mLongitude;
      double mInnerRadius;
      double mOuterRadius;
      double mInnerWeight;
      double mOuterWeight;
   };

   //! A VehicleInput object is created for each of the vehicles defined in
   //! the input file.  A list of VehicleInput objects are stored in the NetworkInput
   //! object that they were defined under.
   class VehicleInput
   {
   public:
      VehicleInput(int aTypeId)
         : mTypeId(aTypeId)
         , mCount(0)
         , mFraction(0.0)
         , mMeanSpeed(0.0)
         , mSigmaSpeed(0.0)
      {
      }

      int    mTypeId;     // String Id of the type of the vehicle
      int    mCount;      // The number of vehicles that should be created of this type.
      double mFraction;   // The percentage of this vehicle type that should exist in the simulation.
      double mMeanSpeed;  // Mean velocity in m/s
      double mSigmaSpeed; // Sigma velocity in m/s
   };

   //! A ConvoyInput object is created for each of the convoys defined in the input file.
   //! A list of ConvoyInput objects are stored in the NetworkInput object that they
   //! are defined under.
   class ConvoyInput
   {
   public:
      ConvoyInput()
         : mStartLatitude(0.0)
         , mStartLongitude(0.0)
         , mEndLatitude(0.0)
         , mEndLongitude(0.0)
         , mSpacing(0.0)
         , mSpeed(0.0)
         , mPauseTime(0.0)
         , mUseClosestWaypoint(false)
      {
      }

      double                    mStartLatitude;
      double                    mStartLongitude;
      double                    mEndLatitude;
      double                    mEndLongitude;
      double                    mSpacing;
      double                    mSpeed;
      double                    mPauseTime;
      bool                      mUseClosestWaypoint;
      std::vector<VehicleInput> mVehicleInput;
   };

   //! A NetworkInput object is created for each network defined in the input file.  It holds
   //! data that is relevant to anything in its network, such as roads and vehicles.
   class RoadTrafficNetworkInput : public XWsfRouteNetwork::NetworkInput
   {
   public:
      RoadTrafficNetworkInput(int aNetworkId)
         : NetworkInput(aNetworkId)
         , mMinDistOffRoad(0.0)
         , mMaxDistOffRoad(0.0)
         , mPauseTime(0.0)
         , mMaxSpeed(0.0)
         , mMeanTravelTime(0.0)
         , mSigmaTravelTime(0.0)
         , mTotalDist(0.0)
      {
      }

      virtual void VerifyData(UtInput& aInput)
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

      double                           mMinDistOffRoad;      // Minimum distance vehicle travels off road (meters)
      double                           mMaxDistOffRoad;      // Maximum distance vehicle travels off road (meters)
      double                           mPauseTime;           // Pause time after completing travel (seconds)
      double                           mMaxSpeed;            // Maximum speed of any vehicle (m/s)
      double                           mMeanTravelTime;      // Mean travel time of vehicles (s)
      double                           mSigmaTravelTime;     // Sigma travel time of vehicles (s)
      double                           mTotalDist;           // The sum of the lengths of all the roads (meters)
      std::vector<VehicleInput>        mVehicleInput;        // data for each of the vehicle types on this road network
      std::vector<WeightedRegionInput> mWeightedRegionInput; // data for each of the weighted regions in this road network.
      std::vector<WeightedRouteSegment> mWeightedRouteSegments; // a list of weighted segments used to place bodies on
                                                                // the road network, with higher densities favouring
                                                                // larger edge weights.
      std::vector<ConvoyInput> mConvoyInput;                    // data for each of the convoys in this road network.
   };

   //! WeightedRouteSegment represents the concept of a weighted edge in a graph.
   //! It includes the two endpoints and their respective weights as well as the edge's
   //! weight. The NetworkInput class includes a list of WeightedRouteSegments that define
   //! the network graph.
   class WeightedRouteSegment
   {
   public:
      WeightedRouteSegment(WsfRoute* aRoutePtr, const WsfWaypoint& aNode1, const WsfWaypoint& aNode2)
         : mRoutePtr(aRoutePtr)
         , mNode1(aNode1)
         , mNode2(aNode2)
         , mNode1Weight(0.0)
         , mNode2Weight(0.0)
         , mSegmentWeight(0.0)
         , mCumulativeWeight(0.0)
      {
      }

      WeightedRouteSegment()
         : mRoutePtr(0)
         , mNode1Weight(0.0)
         , mNode2Weight(0.0)
         , mSegmentWeight(0.0)
         , mCumulativeWeight(0.0)
      {
      }

      WsfRoute*   mRoutePtr;
      WsfWaypoint mNode1;
      WsfWaypoint mNode2;
      double      mNode1Weight;
      double      mNode2Weight;
      double      mSegmentWeight;
      double      mCumulativeWeight;

      //! Equal if the prvided weight is enclosed by this segment's boundary weights.
      bool operator==(double aWeight) const
      {
         return ((aWeight >= mCumulativeWeight) && (aWeight < (mCumulativeWeight + mSegmentWeight)));
      }

      bool operator<(const WeightedRouteSegment& aSegment) const { return (mSegmentWeight < aSegment.mSegmentWeight); }
   };
};

class WSF_EXPORT XWsfRoadTrafficSetup : public WsfModuleSetup, public XWsfRouteNetwork, public XWsfRoadTrafficData
{
public:
   XWsfRoadTrafficSetup(WsfSetup& aSetup);
   virtual bool ProcessInput(UtInput& aInput);

   bool ProcessRoadTraffic(UtInput& aInput);

   virtual bool ProcessNetworkBlock(UtInput& aInput, RoadTrafficNetworkInput& aNetwork);

   virtual bool CompleteLoad();

   virtual std::string GetModuleKind() const;
};

//! XWsfRoadTraffic provides a basic background traffic capability.  Based on the provided input
//! file a specified number of ground vehicles are created and placed randomly on the specified
//! road network (a vehicle density can also be specified.  Weighted regions can be specified
//! so that vehicles populate those regions with a higher density.  Vehicle type, velocity,
//! travel time, and quantity can be specified.  In  addition, vehicles can travel offroad at the
//! end of their route or runtime and pause for a given amount of time.  The initial number of
//! vehicles is always maintained, so as a vehicle is removed from the simulation another one is added.

class WSF_EXPORT XWsfRoadTraffic : public WsfModule, public XWsfRoadTrafficData
{
public:
   XWsfRoadTraffic(WsfSimulation* aSimulationPtr, XWsfRoadTrafficSetup& aSetup);
   virtual ~XWsfRoadTraffic();

   virtual bool Initialize();
   void         AdvanceTime(double aSimTime);

   class SGroundTraffic
   {
   public:
      SGroundTraffic()
         : mIdentifier(0)
         , mVehicleTypeId(0)
         , mAlive(false)
         , mVehicleOffRoad(false)
         , mStationary(false)
         , mNetwork(-1)
         , mRouteDist(0.0)
         , mSpeed(0.0)
         , mTravelTime(0.0)
         , mStartTime(0.0)
         , mDeadTime(0.0)
         , mOffRoadTime(0.0)
         , mRoadPlatform(0)
         , mRoadPlatformIndex(0)
         , mConvoyVehicle(false)
      {
      }

      int          mIdentifier;     // Unique identifier for ground vehicle
      int          mVehicleTypeId;  // Vehicle type
      bool         mAlive;          // Is this vehicle alive?
      bool         mVehicleOffRoad; // Is this vehicle now off-road?
      bool         mStationary;     // Is this vehicle stationary?
      int          mNetwork;        // Index value of network vehicle is traveling on
      double       mRouteDist;      // The length of the path (meters)
      double       mSpeed;          // Speed (meters/sec)
      double       mTravelTime;     // Amount of time vehicle is moving along road (sec)
      double       mStartTime;      // Time vehicle starts moving (sec)
      double       mDeadTime;       // Time vehicle stops moving and is removed (sec)
      double       mOffRoadTime;    // Time vehicle starts moving off the road (sec)
      WsfPlatform* mRoadPlatform;
      unsigned int mRoadPlatformIndex;
      bool         mConvoyVehicle; // Was this vehicle defined as a convoy vehicle?
   };

   WsfSetup* GetSetup() const;

protected:
   //! Copy constructor and assignment operator are declared but not defined in order to prevent use.

   XWsfRoadTraffic(const XWsfRoadTraffic& aSrc);
   XWsfRoadTraffic& operator=(const XWsfRoadTraffic& aRhs);

private:
   enum EndOfPath
   {
      cRESPAWN,
      cREVERSE_DIRECTION
   };

   void BuildWeightedRouteSegmentList(WsfRouteNetwork& aRouteNetwork, RoadTrafficNetworkInput& aNetwork);

   double CalculateNodeWeight(const WeightedRegionInput& aWeightedRegion, const WsfWaypoint& aNode);

   void AddPlatformToSimulation(double aSimTime, int aVehicleNumber);

   int AllocateVehicle(int aVehicleTypeId);

   bool GetRandomWaypoint(const RoadTrafficNetworkInput& aNetwork,
                          WsfWaypoint&                   aWaypoint,
                          WsfWaypoint&                   aRouteSegEndWpt1,
                          WsfWaypoint&                   aRouteSegEndWpt2);

   bool CreateRandomPath(const RoadTrafficNetworkInput& aNetwork, WsfRoute& aPath);

   double CreateRandomPathForVehicle(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber);

   void GoOffRoad(const RoadTrafficNetworkInput& aNetwork, int aVehicleNumber);

   void ClearVehicleList();

   bool CreateVehicle(const RoadTrafficNetworkInput& aNetworkInput, double aCreateTime, int& aVehicleNumber);

   bool CreateConvoyVehicle(int aVehicleType, int& aVehicleNumber);

   void DeletePlatformFromSimulation(double aSimTime, int aVehicleNumber);

   void ReverseVehicleRoute(int aVehicleNumber, double aSimTime);

   void ProcessVehicleBlock(UtInput& aInput, VehicleInput& aVehicle);

   void ProcessWeightedRegionBlock(UtInput& aInput, WeightedRegionInput& aWeightedRegion);

   void ProcessConvoyBlock(UtInput& aInput, ConvoyInput& aConvoyInput);


   bool ValidateVehicleTypes(const RoadTrafficNetworkInput& aNetwork);

   WsfMover* GetMover(WsfPlatform* aMoverPtr);

   // Vector of vehicles.
   bool                        mProcessedInput;
   bool                        mDebug;
   bool                        mVerbose;
   EndOfPath                   mEndOfPathOption;
   std::vector<SGroundTraffic> mTraffic;
   double                      mNextUpdateTime; // Simulation time when the next update
                                                // (platform add, delete, pause, etc) is required.
   int mNextVehicleIdentifier;
   int mNumVehiclesActive;
   int mMaxVehiclesActive;

   UtCallbackHolder mCallbacks;

   UtRandom          mRandom;
   XWsfRouteNetwork* mRouteNetworkPtr;
};

#endif
