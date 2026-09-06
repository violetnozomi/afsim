// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFOSMTRAFFICINPUT_HPP
#define WSFOSMTRAFFICINPUT_HPP

#include "XWsfRouteNetwork.hpp"

//! Traffic input class to handle scenario input data.
class WSF_EXPORT WsfOSM_TrafficInput : public XWsfRouteNetwork
{
public:
   class VehicleInput
   {
   public:
      VehicleInput() = default;
      VehicleInput(WsfStringId aTypeId)
         : mTypeId(aTypeId)
      {
      }

      WsfStringId mTypeId{};        //! String Id of the type of the vehicle.
      int         mCount{0};        //! The number of vehicles that should be created of this type.
      double      mFraction{0.0};   //! The percentage of this vehicle type that should exist in the simulation.
      double      mMeanSpeed{0.0};  //! Mean velocity in m/s: will only be considered when no speed limit exists.
      double      mSigmaSpeed{0.0}; //! Sigma velocity in m/s: will only be considered when no speed limit exists.
      bool        mIgnoreSpeedLimits{false};
   };

   struct ConvoyInput
   {
      std::string mStartingRoute;      //! The road that the convoy starts at.
      double mDistanceAlongRoute{0.0}; //! The distance along the route where the last vehicle of the convoy is located.
      double mSpacing{0.0};
      double mSpeed{0.0};
      std::vector<VehicleInput> mVehicleInput;
   };

   struct RouteAuxMapping
   {
      std::string mMinSpeed{"minspeed"};
      std::string mMaxSpeed{"maxspeed"};
      std::string mSurface{"surface"};
      std::string mLanes{"lanes"};
      std::string mDensity{
         "density"}; //! Corresponds to the density a particular route should have, create based on highway type.
      std::string mOneWay{"oneway"};
   };

   struct WaypointAuxMapping
   {
      std::string mStop{"stop"};
      std::string mTrafficSignals{"traffic_signals"};
      std::string mHighway{"highway"};
   };

   class RoadTrafficNetworkInput : public XWsfRouteNetwork::NetworkInput
   {
   public:
      RoadTrafficNetworkInput() = default;
      RoadTrafficNetworkInput(WsfStringId aNetworkId)
         : NetworkInput(aNetworkId)
      {
      }

      NetworkInput* Clone() const noexcept override { return new RoadTrafficNetworkInput{*this}; }

      std::vector<VehicleInput> mVehicleInput; //! Data for each of the vehicle types on this road network.
      std::vector<ConvoyInput>  mConvoyInput;  //! Data for each of the convoys in this road network.
      RouteAuxMapping mRouteAuxDataMap;       //! Data to map aux data variables to actual route variables (key: actual,
                                              //! value: aux_data variable name).
      WaypointAuxMapping mWaypointAuxDataMap; //! Data to map aux data variables to waypoint variables.
      double             mMaxSpeed{0.0};      //! Maximum speed of any vehicle (m/s).
      unsigned int       mNetIndex{0};        //! The network's index, used for easier access to the network.

   protected:
      RoadTrafficNetworkInput(const RoadTrafficNetworkInput& aRhs) = default;
   };

   WsfOSM_TrafficInput()           = default;
   ~WsfOSM_TrafficInput() override = default;

   bool ProcessInput(UtInput& aInput) override;

protected:
   WsfOSM_TrafficInput(const WsfOSM_TrafficInput& /* aSrc */) = default;
   bool ProcessRoadTraffic(UtInput& aInput);

   bool mProcessedInput{false};
   bool mDebug{false};
};

#endif
