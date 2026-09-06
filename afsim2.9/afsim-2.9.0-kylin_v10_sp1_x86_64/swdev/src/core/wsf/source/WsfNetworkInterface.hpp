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

#ifndef WSFNETWORKINTERFACE_HPP
#define WSFNETWORKINTERFACE_HPP

#include "wsf_export.h"

#include <string>

class GenNetIO;
#include "GenTCP_Server.hpp"
class GenUDP_IO;
#include "UtCallbackHolder.hpp"
class UtInput;
#include "WsfEvent.hpp"
class WsfScenario;

class WSF_EXPORT WsfNetworkInterfaceInput
{
public:
   WsfNetworkInterfaceInput(WsfScenario& aScenario);
   bool ProcessNetworkInput(UtInput& aInput);

   WsfScenario* mScenarioPtr;
   bool         mIsRequested;
   bool         mIsDisabled;
   bool         mDebug;
   std::string  mUDP_Addr;
   std::string  mUDP_Interface;
   std::string  mTCP_Addr; // address to connect to for TCP clients
   int          mTTL;
   int          mSendPort;
   int          mReceivePort;
   double       mUpdateInterval;
   bool         mUpdateOnAdvanceTime;
   bool         mUpdateOnExternalTimer; // allow the NetworkUpdate to be scheduled by a timing mechanism external to WSF
};

//! WsfNetworkInterface is a general-purpose network interface for TCP client or server,
//! or UDP connections.  It uses the abstract features of the GenIO library in
//! an extensible base class.  To use this class, extend the "NetworkUpdate" method
//! to perform custom processing.
class WSF_EXPORT WsfNetworkInterface : protected WsfNetworkInterfaceInput
{
public:
   WsfNetworkInterface(WsfSimulation& aSimulation, const std::string& aName, const WsfNetworkInterfaceInput& aNetInput);
   virtual ~WsfNetworkInterface();


   virtual bool Initialize();
   virtual void NetworkUpdate(double aSimTime) = 0;

   double GetUpdateInterval() const { return mUpdateInterval; }
   void   SetUpdateInterval(double aUpdateInterval) { mUpdateInterval = aUpdateInterval; }

   void SetUpdateOnExternalTimer(bool aUseExternalTimer) { mUpdateOnExternalTimer = aUseExternalTimer; }

   bool IsActive() const;

   bool IsDebug() const { return mDebug; }
   void SetDebug(bool aIsDebug) { mDebug = aIsDebug; }

   // These methods allow parameters to be set programmatically rather than using ProcessInput
   void SetUDP_Addr(const std::string& aUDP_Addr) { mUDP_Addr = aUDP_Addr; }
   void SetSendPort(int aSendPort) { mSendPort = aSendPort; }
   void SetRecvPort(int aRecvPort) { mReceivePort = aRecvPort; }

   WsfSimulation& GetSimulation() { return *mSimulationPtr; }

protected:
   WsfSimulation*   mSimulationPtr;
   std::string      mName;  // identify the name of interface for debug messages
   GenNetIO*        mIOPtr; // common pointer; either of TCP or UDP pointer below.
   GenTCP_IO*       mTCP_IOPtr;
   GenUDP_IO*       mUDP_IOPtr;
   GenTCP_Server    mTCP_Server;
   UtCallbackHolder mCallbacks;

   virtual void PrivateInitialize() {}

private:
   class NetworkUpdateEvent : public WsfEvent
   {
   public:
      NetworkUpdateEvent(WsfNetworkInterface* aInterfacePtr);
      ~NetworkUpdateEvent() override{};
      EventDisposition Execute() override;

      WsfNetworkInterface* mWsfNetworkInterfacePtr;
   };
};

#endif
