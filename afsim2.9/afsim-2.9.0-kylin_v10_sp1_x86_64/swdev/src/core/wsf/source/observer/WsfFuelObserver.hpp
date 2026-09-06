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

#ifndef WSFFUELOBSERVER_HPP
#define WSFFUELOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfFuel.hpp"
class WsfFuelingOperation;
class WsfSimulation;

namespace WsfObserver
{
using FuelEventCallback    = UtCallbackListN<void(double, WsfFuel*, WsfStringId)>;
using TankingEventCallback = UtCallbackListN<void(double, WsfFuelingOperation*, WsfStringId)>;

WSF_EXPORT FuelEventCallback&    FuelEvent(const WsfSimulation* aSimulationPtr);
WSF_EXPORT TankingEventCallback& TankingEvent(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Fuel observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfFuelObserver
{
   //-----------------------------------------------------------------------
   // Callback attributes
   //-----------------------------------------------------------------------

   //! The object the fuel pointer references has been modified.
   //! @param aSimTime     The current simulation time.
   //! @param aFuelPtr     The WsfFuel object the event applies to.
   //! @param aEventNameId The WsfStringId of the fuel event name.
   WsfObserver::FuelEventCallback FuelEvent;

   //! A tanking operation event has occurred.
   //! @param aSimTime         The current simulation time.
   //! @param aFuelingOp       The WsfFuelingOperation in progress.
   //! @param aEventNameId     The WsfStringId of the tanking event name.
   WsfObserver::TankingEventCallback TankingEvent;
};

#endif
