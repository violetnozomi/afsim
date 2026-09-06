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

#ifndef WSFPLATFORMPARTOBSERVER_HPP
#define WSFPLATFORMPARTOBSERVER_HPP

#include <string>

#include "UtCallback.hpp"
class WsfCallback;
#include "wsf_export.h"
class WsfPlatform;
class WsfVisualPart;
class WsfSimulation;

namespace WsfObserver
{
using VisualPartTurnedOnCallback       = UtCallbackListN<void(double, WsfVisualPart*)>;
using VisualPartTurnedOffCallback      = UtCallbackListN<void(double, WsfVisualPart*)>;
using VisualPartNonOperationalCallback = UtCallbackListN<void(double, WsfVisualPart*)>;
using VisualPartOperationalCallback    = UtCallbackListN<void(double, WsfVisualPart*)>;
using VisualPartBrokenCallback         = UtCallbackListN<void(double, WsfVisualPart*)>;

WSF_EXPORT VisualPartTurnedOnCallback&       VisualPartTurnedOn(const WsfSimulation* aSimulationPtr);
WSF_EXPORT VisualPartTurnedOffCallback&      VisualPartTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT VisualPartNonOperationalCallback& VisualPartNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT VisualPartOperationalCallback&    VisualPartOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT VisualPartBrokenCallback&         VisualPartBroken(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the PlatformPart observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfPlatformPartObserver
{
   //! A visual part has been turned on
   //! @param aSimTime The current simulation time
   //! @param aVisualPartPtr The part that is being turned on
   WsfObserver::VisualPartTurnedOnCallback VisualPartTurnedOn;

   //! A visual part has been turned off
   //! @param aSimTime The current simulation time
   //! @param aVisualPartPtr The part that is being turned off
   WsfObserver::VisualPartTurnedOffCallback VisualPartTurnedOff;

   //! A visual part has been made non-operational
   //! @param aSimTime The current simulation time
   //! @param aVisualPartPtr The part that is non-operational
   WsfObserver::VisualPartNonOperationalCallback VisualPartNonOperational;

   //! A visual part has been made operational
   //! @param aSimTime The current simulation time
   //! @param aVisualPartPtr The part that is operational
   WsfObserver::VisualPartOperationalCallback VisualPartOperational;

   //! A visual part has been broken
   //! @param aSimTime The current simulation time
   //! @param aVisualPartPtr The part that is broken
   WsfObserver::VisualPartBrokenCallback VisualPartBroken;
};

#endif
