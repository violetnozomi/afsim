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

#include "xio/WsfXIO_InputData.hpp"

#include <limits>

#include "GenIP.hpp"
#include "GenNetInfo.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtStringUtil.hpp"
#include "ext/WsfExtEntityId.hpp"
#include "xio/WsfXIO.hpp"

// =================================================================================================
WsfXIO_InputData::WsfXIO_InputData()
   : mApplicationName("WSF")
   , mTCP_Port(0)
   , mMulticastTimeToLive(-1)
   , mMulticastLoopback(true)
   , mHeartbeatInterval(5.0)
   , mAutoMappingDefault(false)
   , mPublisherDebugLevel(0)
   , mDebugEnabled(false)
   , mShowConnections(true)
   , mShowTransferRate(false)
   , mPauseForConnection(false)
   , mInterfaceRequested(false){};

// =================================================================================================
// virtual
bool WsfXIO_InputData::ProcessInput(UtInput& aInput)
{
   bool myCommand(false);
   if (aInput.GetCommand() == "xio_interface")
   {
      UtInputBlock block(aInput);
      std::string  cmd;
      while (block.ReadCommand(cmd))
      {
         if (cmd == "port")
         {
            throw UtInput::BadValue(aInput, "port command must directly follow broadcast or multicast commands.");
         }
         else if (cmd == "unicast")
         {
            UDP_Target target;
            target.mType = cUNICAST;
            aInput.ReadValue(target.mAddress);
            if (ProcessPorts(aInput, target.mSendPort, target.mRecvPort))
            {
               mUDP_Targets.push_back(target);
               mInterfaceRequested = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Expected port or receive_port and send_port commands");
            }
         }
         else if (cmd == "broadcast")
         {
            UDP_Target target;
            target.mType = cBROADCAST;
            aInput.ReadValue(target.mAddress);

            if (ProcessPorts(aInput, target.mSendPort, target.mRecvPort))
            {
               GenSockets::GenIP ip(target.mAddress);
               if (!ip.IsBroadcast())
               {
                  throw UtInput::BadValue(aInput, "Invalid broadcast IP: " + target.mAddress);
               }
               mUDP_Targets.push_back(target);
               mInterfaceRequested = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Expected port or receive_port and send_port commands");
            }
         }
         else if (cmd == "multicast")
         {
            UDP_Target target;
            target.mType = cMULTICAST;
            aInput.ReadValue(target.mAddress);
            aInput.ReadValue(target.mInterface);

            if (ProcessPorts(aInput, target.mSendPort, target.mRecvPort))
            {
               GenSockets::GenIP ip(target.mAddress);
               if (!ip.IsMulticast())
               {
                  throw UtInput::BadValue(aInput, "Invalid multicast IP: " + target.mAddress);
               }
               mUDP_Targets.push_back(target);
               mInterfaceRequested = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Expected port or receive_port and send_port commands");
            }
         }
         else if (cmd == "time_to_live")
         {
            aInput.ReadValue(mMulticastTimeToLive);
            aInput.ValueInClosedRange(mMulticastTimeToLive, 0, 255);
         }
         else if (cmd == "multicast_loopback")
         {
            aInput.ReadValue(mMulticastLoopback);
         }
         else if (cmd == "heartbeat_interval")
         {
            aInput.ReadValueOfType(mHeartbeatInterval, UtInput::cTIME);
         }
         else if (cmd == "debug")
         {
            aInput.ReadValue(mDebugEnabled);
         }
         else if ((cmd == "show_connections") || (cmd == "verbose"))
         {
            aInput.ReadValue(mShowConnections);
         }
         else if (cmd == "application")
         {
            std::string appName;
            aInput.ReadValue(appName);
            mApplicationName = appName;
         }
         else if (cmd == "connect_to_simulations")
         {
            AddConnectType(wsf::xio::cAPP_SIMULATION);
         }
         else if (cmd == "show_transfer_rate")
         {
            aInput.ReadValue(mShowTransferRate);
         }
         else if (cmd == "publisher_debug")
         {
            mPublisherDebugLevel = 2;
         }
         // The following commands have no applicability to a user-created WsfXIO_Interface, but they are accepted.
         else if (cmd == "auto_dis_mapping")
         {
            aInput.ReadValue(mAutoMappingDefault);
         }
         else if (cmd == "auto_map_application")
         {
            mAutoMappedApplications.push_back(ReadApplicationId(aInput));
         }
         else if (cmd == "no_auto_map_application")
         {
            mRegularApplications.push_back(ReadApplicationId(aInput));
         }
         else if (cmd == "pause_for_connection")
         {
            aInput.ReadValue(mPauseForConnection);
         }
         else if (cmd == "falling_behind_threshold")
         {
            aInput.ReadValueOfType(mFallingBehindThreshold.emplace(), UtInput::cTIME);
         }
         else if (cmd == "send_aux_data_updates")
         {
            aInput.ReadValue(mAuxDataUpdates);
         }
         else if (ProcessInputEvent.Callback(aInput))
         {
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      myCommand = true;
   }
   return myCommand;
}

// =================================================================================================
//! Specifies that applications of a certain type will be automatically connected to.
// private
void WsfXIO_InputData::AddConnectType(int aApplicationType)
{
   mApplicationTypeFilter.insert(aApplicationType);
}

// =================================================================================================
// private
bool WsfXIO_InputData::ProcessPorts(UtInput& aInput, int& aSendPort, int& aRecvPort)
{
   bool        myCommand = true;
   std::string command;
   aInput.ReadCommand(command);
   if (command == "port")
   {
      aInput.ReadValue(aSendPort);
      aInput.ValueInClosedRange(aSendPort, 1, 65535);
      aRecvPort = aSendPort;
   }
   else if (command == "send_port")
   {
      aInput.ReadValue(aSendPort);
      aInput.ValueInClosedRange(aSendPort, 1, 65535);
   }
   else if (command == "receive_port")
   {
      aInput.ReadValue(aRecvPort);
      aInput.ValueInClosedRange(aRecvPort, 1, 65535);
   }
   else
   {
      myCommand = false;
      aInput.PushBack(command);
   }
   if (myCommand)
   {
      ProcessPorts(aInput, aSendPort, aRecvPort);
   }
   return aSendPort != 0 || aRecvPort != 0;
}

// =================================================================================================
// private
WsfExtEntityId WsfXIO_InputData::ReadApplicationId(UtInput& aInput)
{
   std::string application;
   aInput.ReadValue(application);
   size_t cPos = application.find(':');
   if (cPos == std::string::npos)
   {
      throw UtInput::BadValue(aInput, "Expected application in form site:application");
   }
   WsfExtEntityId appId;
   std::string    siteStr = application.substr(0, cPos);
   std::string    appStr  = application.substr(cPos + 1);
   if (siteStr == "*")
   {
      appId.SetSite(WsfExtEntityId::ALL_SITES);
   }
   else
   {
      appId.SetSite(UtStringUtil::ToInt(siteStr));
   }
   if (appStr == "*")
   {
      appId.SetApplication(WsfExtEntityId::ALL_APPLIC);
   }
   else
   {
      appId.SetApplication(UtStringUtil::ToInt(appStr));
   }
   if (appId.GetApplication() == 0 || appId.GetSite() == 0)
   {
      throw UtInput::BadValue(aInput, "Site and application must be a 1-65534 or '*'");
   }
   return appId;
}
