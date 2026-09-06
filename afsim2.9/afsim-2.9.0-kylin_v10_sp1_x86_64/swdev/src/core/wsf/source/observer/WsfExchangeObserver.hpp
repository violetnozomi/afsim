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

#ifndef WSF_EXCHANGE_OBSERVER_HPP
#define WSF_EXCHANGE_OBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfExchange.hpp"

class WsfSimulation;

namespace WsfObserver
{
using ExchangeQueriedCallback = UtCallbackListN<void(double, const WsfExchange::Query&)>;
using ExchangeNegotiatedCallback =
   UtCallbackListN<void(double, const WsfExchange::Transactor&, const WsfExchange::Query&)>;
using ExchangeRequestFailedCallback =
   UtCallbackListN<void(double, const WsfExchange::Transactor&, const WsfExchange::Query&)>;
using ExchangeCompletedCallback = UtCallbackListN<void(double, const WsfExchange::Transactor&, const WsfExchange::Query&)>;
using PlatformCapabilityChangedCallback = UtCallbackListN<void(double, WsfPlatform*, WsfStringId, bool)>;

WSF_EXPORT ExchangeQueriedCallback&           ExchangeQueried(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ExchangeNegotiatedCallback&        ExchangeNegotiated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ExchangeRequestFailedCallback&     ExchangeRequestFailed(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ExchangeCompletedCallback&         ExchangeCompleted(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformCapabilityChangedCallback& PlatformCapabilityChanged(const WsfSimulation* aSimulationPtr);
}; // namespace WsfObserver

//! The implementation of the Exchange observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfExchangeObserver
{
   //-----------------------------------------------------------------------
   // Callback attributes
   //-----------------------------------------------------------------------

   //! Two platforms are attempting to accomplish a transfer of goods or services.
   WsfObserver::ExchangeQueriedCallback       ExchangeQueried;
   WsfObserver::ExchangeNegotiatedCallback    ExchangeNegotiated;
   WsfObserver::ExchangeRequestFailedCallback ExchangeRequestFailed;
   WsfObserver::ExchangeCompletedCallback     ExchangeCompleted;

   //! The capabilities of a platform has changed.
   //! @param aSimTime        The current simulation time.
   //! @param aPlatformPtr    The platform whose capability has changed.
   //! @param aCapabilityId   The stringId of the capability gained or lost (see WsfExchange.hpp).
   //! @param aGainedOrLost   Indication of gained (true) or lost (false) capability.
   WsfObserver::PlatformCapabilityChangedCallback PlatformCapabilityChanged;
};

#endif
