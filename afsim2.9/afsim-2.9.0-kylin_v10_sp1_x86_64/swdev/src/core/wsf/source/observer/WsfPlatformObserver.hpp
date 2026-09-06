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

#ifndef WSFPLATFORMOBSERVER_HPP
#define WSFPLATFORMOBSERVER_HPP

#include <string>

#include "UtCallback.hpp"
class WsfCallback;
#include "wsf_export.h"
class WsfMessage;
class WsfPlatform;
class WsfRoute;
class WsfTrack;
class WsfSimulation;

namespace WsfObserver
{

//! The type of data being changed in the PlatformAppearanceChanged callback
enum AppearanceType
{
   cAPPEARANCE_WORD,
   cCONCEALMENT_FACTOR,
   // The following two items allow for a maximum of 100 signature types.
   cSIGNATURE_STATE_START        = 100, //!< Starting index of signature state changes
   cSIGNATURE_STATE_LIMIT        = 199, //!< Limiting index of signature state changes
   cSIGNATURE_SCALE_FACTOR_START = 200, //!< Starting index of signature scale factor changes
   cSIGNATURE_SCALE_FACTOR_LIMIT = 299  //!< Limiting index of signature scale factor changes
};

using CommentCallback                   = UtCallbackListN<void(double, WsfPlatform*, const std::string&)>;
using CrashedIntoGroundCallback         = UtCallbackListN<void(double, WsfPlatform*)>;
using ExecuteCallbackCallback           = UtCallbackListN<void(double, WsfPlatform*, const WsfCallback&)>;
using GroupAddedCallback                = UtCallbackListN<void(const std::string&, const std::string&)>;
using NavigationStatusChangedCallback   = UtCallbackListN<void(double, WsfPlatform*, const std::string&)>;
using PlatformAppearanceChangedCallback = UtCallbackListN<void(double, WsfPlatform*, AppearanceType)>;
using PlatformAddedCallback             = UtCallbackListN<void(double, WsfPlatform*)>;
using PlatformBrokenCallback            = UtCallbackListN<void(double, WsfPlatform*)>;
using PlatformDamageChangedCallback     = UtCallbackListN<void(double, WsfPlatform*)>;
using PlatformDeletedCallback           = UtCallbackListN<void(double, WsfPlatform*)>;
using PlatformInitializedCallback       = UtCallbackListN<void(double, WsfPlatform*)>;
using PlatformOmittedCallback           = UtCallbackListN<void(double, WsfPlatform*)>;

WSF_EXPORT CommentCallback&                   Comment(const WsfSimulation* aSimulationPtr);
WSF_EXPORT CrashedIntoGroundCallback&         CrashedIntoGround(const WsfSimulation* aSimulationPtr);
WSF_EXPORT ExecuteCallbackCallback&           ExecuteCallback(const WsfSimulation* aSimulationPtr);
WSF_EXPORT GroupAddedCallback&                GroupAdded(const WsfSimulation* aSimulationPtr);
WSF_EXPORT NavigationStatusChangedCallback&   NavigationStatusChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformAppearanceChangedCallback& PlatformAppearanceChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformDamageChangedCallback&     PlatformDamageChanged(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformAddedCallback&             PlatformAdded(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformBrokenCallback&            PlatformBroken(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformDeletedCallback&           PlatformDeleted(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformInitializedCallback&       PlatformInitialized(const WsfSimulation* aSimulationPtr);
WSF_EXPORT PlatformOmittedCallback&           PlatformOmitted(const WsfSimulation* aSimulationPtr);
} // namespace WsfObserver

//! The implementation of the Platform observer objects.
//! An instance of this class is maintained by the simulation.
struct WSF_EXPORT WsfPlatformObserver
{
   WsfObserver::CommentCallback Comment;

   //! A platform has crashed into the ground.
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform that crashed.
   WsfObserver::CrashedIntoGroundCallback CrashedIntoGround;

   //! A 'callback' is being invoked on a platform.
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform.
   //! @param aCallback The callback being invoked.
   WsfObserver::ExecuteCallbackCallback ExecuteCallback;

   //! A new platform / platform part group has been added.
   //! @param aGroupName - the name of the group.
   //! @param aGroupType - the type of the group.
   WsfObserver::GroupAddedCallback GroupAdded;

   //! The navigation status (GPS availability) of a platform has changed.
   //! @param aSimTime     The current simulation time.
   //! @param aPlatformPtr The platform whose status changed.
   //! @param aStatus      The new navigation status.
   WsfObserver::NavigationStatusChangedCallback NavigationStatusChanged;

   //! A platform was added to the simulation.
   //!
   //! This is called before the platforms' 'Initialize()' method so be
   //! careful. The only assumptions that can be made are that the platform
   //! and its associated systems are present and they have been properly
   //! registered.  Other than that you should not assume anything about
   //! position, state, etc.
   //!
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform that was added.
   WsfObserver::PlatformAddedCallback PlatformAdded;

   //! The appearance of a platform has changed.
   //! @param aSimTime     The current simulation time.
   //! @param aPlatformPtr The platform whose appearance has changed.
   //! @param aItemType    The type of the item being changed.
   WsfObserver::PlatformAppearanceChangedCallback PlatformAppearanceChanged;

   //! A platform that was completely damaged is being removed from the simulation.
   //!
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform that has been broken.
   WsfObserver::PlatformBrokenCallback PlatformBroken;

   //! The damage state of a platform has changed.
   //! @param aSimTime                 The current simulation time.
   //! @param aPlatformPtr             The platform whose damage state has changed.
   WsfObserver::PlatformDamageChangedCallback PlatformDamageChanged;

   //! A platform is being deleted from simulation.
   //!
   //! This is called just before the platform is removed from the simulation.
   //!
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform that is being deleted.
   WsfObserver::PlatformDeletedCallback PlatformDeleted;

   //! A platform has been initialized.
   //!
   //! This is called just after the platforms Initialize method has been called.
   //! @param aSimTime The current simulation time.
   //! @param aPlatformPtr The platform that was initialized.
   WsfObserver::PlatformInitializedCallback PlatformInitialized;

   //! A platform has been omitted from the simulation because the 'platform_availability'
   //! has indicated the platform is not available or the user-defined 'on_platform_injection'
   //! script has indicated it shouldn't be added.
   //! @param aSimTime The current simulation time (should always be zero in this case).
   //! @param aPlatformPtr The platform is unavailable.
   WsfObserver::PlatformOmittedCallback PlatformOmitted;
};

#endif
