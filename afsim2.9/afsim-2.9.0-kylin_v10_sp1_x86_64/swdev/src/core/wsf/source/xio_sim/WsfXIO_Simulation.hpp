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

#ifndef WSFXIO_SIMULATION_HPP
#define WSFXIO_SIMULATION_HPP

#include "wsf_export.h"

#include "UtCallbackHolder.hpp"
#include "WsfComponentList.hpp"
class WsfDisInterface;
#include "xio_sim/WsfXIO_Component.hpp"
class WsfXIO_DrawService;
class WsfXIO_DeletePlatformPkt;
class WsfXIO_InformationRequestPkt;
class WsfXIO_Interface;
class WsfXIO_MessageService;
class WsfXIO_PartStateChangeRequestPkt;
class WsfXIO_PlatformInfoService;
class WsfXIO_PlatformListService;
class WsfXIO_PlatformPartInfoService;
class WsfXIO_ReRoutePlatformPkt;
class WsfXIO_ScriptService;
class WsfXIO_SensorService;
class WsfXIO_SimTimeCommandPkt;
class WsfXIO_SimTimeService;
class WsfXIO_TaskService;
class WsfXIO_TrackManagerStateChangeRequestPkt;
class WsfXIO_TrackService;

using WsfXIO_ComponentList = WsfComponentListT<WsfXIO_Component>;

//! Implements the XIO side of standard WSF simulations.
//! An instance of this is instantiated what WSF simulation application does:
//! \code
//! WSF_REGISTER_EXTENSION(app, xio_interface);
//! \endcode
//! This class hooks into the simulation performs the necessary functions needed to allow WSF applications
//! to participate in a distributed XIO simulation.
class WSF_EXPORT WsfXIO_Simulation : public WsfXIO_ComponentList
{
public:
   using ComponentList = WsfComponentListT<WsfXIO_Component>;

   WsfXIO_Simulation(WsfXIO_Interface* aInterfacePtr, WsfSimulation& aSimulation);
   ~WsfXIO_Simulation() override;

   bool Initialize();

   WsfXIO_Interface* GetInterface() const { return mInterfacePtr; }
   WsfSimulation&    GetSimulation() const { return mSimulation; }
   WsfDisInterface*  GetDisInterface() const { return mDisInterfacePtr; }

   //! @name Accessors to some of the services created by this class.
   //@{
   WsfXIO_TrackService*            GetTrackService() const { return mTrackServicePtr; }
   WsfXIO_PlatformListService*     GetPlatformListService() const { return mPlatformListServicePtr; }
   WsfXIO_PlatformInfoService*     GetPlatformInfoService() const { return mPlatformInfoServicePtr; }
   WsfXIO_PlatformPartInfoService* GetPlatformPartInfoService() const { return mPlatformPartInfoServicePtr; }
   WsfXIO_TaskService*             GetTaskService() const { return mTaskServicePtr; }
   WsfXIO_MessageService*          GetMessageService() const { return mMessageServicePtr; }
   WsfXIO_SimTimeService*          GetSimTimeService() const { return mSimTimeServicePtr; }
   WsfXIO_ScriptService*           GetScriptService() const { return mScriptServicePtr; }
   WsfXIO_DrawService*             GetDrawService() const { return mDrawServicePtr; }
   WsfXIO_SensorService*           GetSensorService() const { return mSensorServicePtr; }
   //@}

private:
   void HandleInformationRequest(WsfXIO_InformationRequestPkt& aPkt);
   void HandleDeletePlatform(WsfXIO_DeletePlatformPkt& aPkt);
   void HandleReRoutePkt(WsfXIO_ReRoutePlatformPkt& aPkt);
   void HandleStateChangeRequest(WsfXIO_PartStateChangeRequestPkt& aPkt);
   void HandleTrackManagerStateChangeRequest(WsfXIO_TrackManagerStateChangeRequestPkt& aPkt);
   void HandleSimTimeCommand(WsfXIO_SimTimeCommandPkt& aPkt);

   void SendTimeBehind(bool aIsBehind);
   void SimulationTimeBehind(double aTimeBehind);

   UtCallbackHolder  mCallbacks;
   WsfXIO_Interface* mInterfacePtr;
   WsfSimulation&    mSimulation;
   WsfDisInterface*  mDisInterfacePtr;

   WsfXIO_TrackService*            mTrackServicePtr;
   WsfXIO_PlatformListService*     mPlatformListServicePtr;
   WsfXIO_PlatformInfoService*     mPlatformInfoServicePtr;
   WsfXIO_PlatformPartInfoService* mPlatformPartInfoServicePtr;
   WsfXIO_TaskService*             mTaskServicePtr;
   WsfXIO_MessageService*          mMessageServicePtr;
   WsfXIO_SimTimeService*          mSimTimeServicePtr;
   WsfXIO_ScriptService*           mScriptServicePtr;
   WsfXIO_DrawService*             mDrawServicePtr;
   WsfXIO_SensorService*           mSensorServicePtr;

   bool mBehindRealTime = false; // True if simulation is behind real-time
};

#endif
