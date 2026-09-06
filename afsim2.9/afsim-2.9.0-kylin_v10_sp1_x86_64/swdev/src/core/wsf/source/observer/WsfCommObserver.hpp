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

#ifndef WSFCOMMOBSERVER_HPP
#define WSFCOMMOBSERVER_HPP

#include "wsf_export.h"

#include "UtCallback.hpp"
#include "WsfCommAddress.hpp"
#include "WsfPlatform.hpp"
class WsfMessage;
class WsfSimulation;

namespace wsf
{
namespace comm
{
class Address;
class Comm;
class Network;
class Result;

namespace router
{
class Router;
class ProtocolInterface;
} // namespace router
} // namespace comm
} // namespace wsf

namespace WsfObserver
{
using wsf::comm::Address;
using wsf::comm::Comm;
using wsf::comm::Network;
using wsf::comm::Result;
using wsf::comm::router::ProtocolInterface;
using wsf::comm::router::Router;

using CommFrequencyChangedCallback        = UtCallbackListN<void(double, Comm*)>;
using CommBrokenCallback                  = UtCallbackListN<void(double, Comm*)>;
using RouterBrokenCallback                = UtCallbackListN<void(double, Router*)>;
using CommNonOperationalCallback          = UtCallbackListN<void(double, Comm*)>;
using RouterNonOperationalCallback        = UtCallbackListN<void(double, Router*)>;
using CommOperationalCallback             = UtCallbackListN<void(double, Comm*)>;
using RouterOperationalCallback           = UtCallbackListN<void(double, Router*)>;
using CommTurnedOffCallback               = UtCallbackListN<void(double, Comm*)>;
using RouterTurnedOffCallback             = UtCallbackListN<void(double, Router*)>;
using CommTurnedOnCallback                = UtCallbackListN<void(double, Comm*)>;
using RouterTurnedOnCallback              = UtCallbackListN<void(double, Router*)>;
using MessageDeliveryAttemptCallback      = UtCallbackListN<void(double, Comm*, Comm*, const WsfMessage&, Result&)>;
using MessageDiscardedCallback            = UtCallbackListN<void(double, Comm*, const WsfMessage&, const std::string&)>;
using MessageFailedRoutingCallback        = UtCallbackListN<void(double, Comm*, WsfPlatform*, const WsfMessage&)>;
using MessageHopCallback                  = UtCallbackListN<void(double, Comm*, Comm*, const WsfMessage&)>;
using MessageQueuedCallback               = UtCallbackListN<void(double, Comm*, const WsfMessage&, size_t)>;
using MessageReceivedCallback             = UtCallbackListN<void(double, Comm*, Comm*, const WsfMessage&, Result&)>;
using MessageTransmittedCallback          = UtCallbackListN<void(double, Comm*, const WsfMessage&)>;
using MessageTransmitEndedCallback        = UtCallbackListN<void(double, Comm*, const WsfMessage&)>;
using MessageTransmittedHeartbeatCallback = UtCallbackListN<void(double, Comm*, const WsfMessage&)>;
using MessageUpdatedCallback              = UtCallbackListN<void(double, Comm*, const WsfMessage&, const WsfMessage&)>;
using CommAddedToManagerCallback          = UtCallbackListN<void(double, Comm*)>;
using CommRemovedFromManagerCallback      = UtCallbackListN<void(double, Comm*)>;
using LinkAddedToManagerCallback          = UtCallbackListN<void(double, Comm*, Comm*)>;
using LinkRemovedFromManagerCallback      = UtCallbackListN<void(double, Comm*, Comm*)>;
using LinkEnabledOnManagerCallback        = UtCallbackListN<void(double, Comm*, Comm*)>;
using LinkDisabledOnManagerCallback       = UtCallbackListN<void(double, Comm*, Comm*)>;
using CommAddedToLocalCallback            = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*)>;
using CommRemovedFromLocalCallback        = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*)>;
using LinkAddedToLocalCallback     = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*, Address*)>;
using LinkRemovedFromLocalCallback = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*, Address*)>;
using LinkEnabledOnLocalCallback   = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*, Address*)>;
using LinkDisabledOnLocalCallback  = UtCallbackListN<void(double, Router*, ProtocolInterface*, Address*, Address*)>;
using NetworkAddedCallback         = UtCallbackListN<void(double, Network*)>;
using NetworkRemovedCallback       = UtCallbackListN<void(double, Network*)>;

WSF_EXPORT CommFrequencyChangedCallback&        CommFrequencyChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommTurnedOffCallback&               CommTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommTurnedOnCallback&                CommTurnedOn(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommNonOperationalCallback&          CommNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommOperationalCallback&             CommOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommBrokenCallback&                  CommBroken(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageDeliveryAttemptCallback&      MessageDeliveryAttempt(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageDiscardedCallback&            MessageDiscarded(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageHopCallback&                  MessageHop(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageFailedRoutingCallback&        MessageFailedRouting(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageQueuedCallback&               MessageQueued(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageReceivedCallback&             MessageReceived(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageTransmittedCallback&          MessageTransmitted(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageTransmitEndedCallback&        MessageTransmitEnded(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageTransmittedHeartbeatCallback& MessageTransmittedHeartbeat(const WsfSimulation* aSimulationPtr);
WSF_EXPORT MessageUpdatedCallback&              MessageUpdated(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommAddedToManagerCallback&          CommAddedToManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommRemovedFromManagerCallback&      CommRemovedFromManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkAddedToManagerCallback&          LinkAddedToManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkRemovedFromManagerCallback&      LinkRemovedFromManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkEnabledOnManagerCallback&        LinkEnabledOnManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkDisabledOnManagerCallback&       LinkDisabledOnManager(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommAddedToLocalCallback&            CommAddedToLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CommRemovedFromLocalCallback&        CommRemovedFromLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkAddedToLocalCallback&            LinkAddedToLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkRemovedFromLocalCallback&        LinkRemovedFromLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkEnabledOnLocalCallback&          LinkEnabledOnLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT LinkDisabledOnLocalCallback&         LinkDisabledOnLocal(const WsfSimulation* aSimulationPtr);
WSF_EXPORT NetworkAddedCallback&                NetworkAdded(const WsfSimulation* aSimulationPtr);
WSF_EXPORT NetworkRemovedCallback&              NetworkRemoved(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouterBrokenCallback&                RouterBroken(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouterNonOperationalCallback&        RouterNonOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouterOperationalCallback&           RouterOperational(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouterTurnedOffCallback&             RouterTurnedOff(const WsfSimulation* aSimulationPtr);
WSF_EXPORT RouterTurnedOnCallback&              RouterTurnedOn(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the comm observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfCommObserver
{
   //-----------------------------------------------------------------------
   // Callback attributes
   //-----------------------------------------------------------------------

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommFrequencyChangedCallback CommFrequencyChanged;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommTurnedOffCallback CommTurnedOff;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   WsfObserver::RouterTurnedOffCallback RouterTurnedOff;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommTurnedOnCallback CommTurnedOn;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   WsfObserver::RouterTurnedOnCallback RouterTurnedOn;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommNonOperationalCallback CommNonOperational;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   WsfObserver::RouterNonOperationalCallback RouterNonOperational;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommOperationalCallback CommOperational;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   WsfObserver::RouterOperationalCallback RouterOperational;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommBrokenCallback CommBroken;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   WsfObserver::RouterBrokenCallback RouterBroken;

   //! @param aSimTime The current simulation time.
   //! @param aXmtrPtr The transmitting comm.
   //! @param aRcvrPtr The receiving comm.
   //! @param aMessage The message.
   //! @param aResult  The EM interaction result.
   WsfObserver::MessageDeliveryAttemptCallback MessageDeliveryAttempt;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   //! @param aMessage The message.
   WsfObserver::MessageDiscardedCallback MessageDiscarded;

   //! @param aSimTime The current simulation time.
   //! @param aRcvrPtr The receiving comm device.
   //! @param aDstPtr The destination comm device.
   //! @param aMessage The message.
   WsfObserver::MessageHopCallback MessageHop;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   //! @param aPlatformPtr The platform.
   //! @param aMessage The message.
   WsfObserver::MessageFailedRoutingCallback MessageFailedRouting;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   //! @param aOldMessage The previous message.
   //! @param aNewMessage The current message.
   WsfObserver::MessageUpdatedCallback MessageUpdated;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   //! @param aMessage The message.
   WsfObserver::MessageQueuedCallback MessageQueued;

   //! @param aSimTime The current simulation time.
   //! @param aXmtrPtr The transmitting comm device.
   //! @param aRcvrPtr The receiving comm device.
   //! @param aMessage The message.
   //! @param aResult  The EM interaction result.
   WsfObserver::MessageReceivedCallback MessageReceived;

   //! @param aSimTime The current simulation time.
   //! @param aXmtrPtr The transmitting comm device.
   //! @param aMessage The message.
   WsfObserver::MessageTransmittedCallback MessageTransmitted;

   //! @param aSimTime The current simulation time.
   //! @param aXmtrPtr The transmitting comm device.
   //! @param aMessage The message.
   WsfObserver::MessageTransmitEndedCallback MessageTransmitEnded;

   //! @param aSimTime The current simulation time.
   //! @param aXmtrPtr The transmitting comm device.
   //! @param aMessage The message.
   WsfObserver::MessageTransmittedHeartbeatCallback MessageTransmittedHeartbeat;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommAddedToManagerCallback CommAddedToManager;

   //! @param aSimTime The current simulation time.
   //! @param aCommPtr The comm device.
   WsfObserver::CommRemovedFromManagerCallback CommRemovedFromManager;

   //! @param aSimTime The current simulation time.
   //! @param aCommSenderPtr The sending comm device.
   //! @param aCommRecipientPtr The recipient comm device.
   WsfObserver::LinkAddedToManagerCallback LinkAddedToManager;

   //! @param aSimTime The current simulation time.
   //! @param aCommSenderPtr The sending comm device.
   //! @param aCommRecipientPtr The recipient comm device.
   WsfObserver::LinkRemovedFromManagerCallback LinkRemovedFromManager;

   //! @param aSimTime The current simulation time.
   //! @param aCommSenderPtr The sending comm device.
   //! @param aCommRecipientPtr The recipient comm device.
   WsfObserver::LinkEnabledOnManagerCallback LinkEnabledOnManager;

   //! @param aSimTime The current simulation time.
   //! @param aCommSenderPtr The sending comm device.
   //! @param aCommRecipientPtr The recipient comm device.
   WsfObserver::LinkDisabledOnManagerCallback LinkDisabledOnManager;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   //! @param aProtocolPtr The protocol.
   //! @param aAddressPtr The address.
   WsfObserver::CommAddedToLocalCallback CommAddedToLocal;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   //! @param aProtocolPtr The protocol.
   //! @param aAddressPtr The address.
   WsfObserver::CommRemovedFromLocalCallback CommRemovedFromLocal;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   //! @param aProtocolPtr The protocol.
   //! @param aSourceAddressPtr The source address.
   //! @param aDestinationAddressPtr The destination address.
   WsfObserver::LinkAddedToLocalCallback LinkAddedToLocal;

   //! A link was removed from a local router's table.
   //! @param aSimTime The current simulation time.
   //! @param The router object that removed a link.
   //! @param The protocol object that removed a link.
   //! @param The address of the link source comm.
   //! @param The address of the link destination comm.
   WsfObserver::LinkRemovedFromLocalCallback LinkRemovedFromLocal;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   //! @param aProtocolPtr The protocol.
   //! @param aSourceAddressPtr The source address.
   //! @param aDestinationAddressPtr The destination address.
   WsfObserver::LinkEnabledOnLocalCallback LinkEnabledOnLocal;

   //! @param aSimTime The current simulation time.
   //! @param aRouterPtr The router.
   //! @param aProtocolPtr The protocol.
   //! @param aSourceAddressPtr The source address.
   //! @param aDestinationAddressPtr The destination address.
   WsfObserver::LinkDisabledOnLocalCallback LinkDisabledOnLocal;

   //! @param aSimTime The current simulation time.
   //! @param aNetworkPtr The network.
   WsfObserver::NetworkAddedCallback NetworkAdded;

   //! @param aSimTime The current simulation time.
   //! @param aNetworkPtr The network.
   WsfObserver::NetworkRemovedCallback NetworkRemoved;
};

#endif
