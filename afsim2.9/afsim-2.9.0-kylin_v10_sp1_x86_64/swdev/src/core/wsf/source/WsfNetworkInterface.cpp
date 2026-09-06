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

#include "WsfNetworkInterface.hpp"

#include "GenNetInfo.hpp"
#include "GenSocket.hpp"
#include "GenTCP_IO.hpp"
#include "GenUDP_IO.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtSleep.hpp"
#include "WsfSimulation.hpp"
#include "WsfSimulationObserver.hpp"

WsfNetworkInterfaceInput::WsfNetworkInterfaceInput(WsfScenario& aScenario)
   : mScenarioPtr(&aScenario)
   , mIsRequested(false)
   , mIsDisabled(false)
   , mDebug(false)
   , mUDP_Addr("")
   , mUDP_Interface("")
   , mTCP_Addr("")
   , mTTL(255)
   , mSendPort(0)
   , mReceivePort(0)
   , mUpdateInterval(0.1)
   , mUpdateOnAdvanceTime(false)
   , mUpdateOnExternalTimer(false)
{
}

WsfNetworkInterface::WsfNetworkInterface(WsfSimulation&                  aSimulation,
                                         const std::string&              aName,
                                         const WsfNetworkInterfaceInput& aNetInput)
   : WsfNetworkInterfaceInput(aNetInput)
   , mSimulationPtr(&aSimulation)
   , mName(aName)
   , mIOPtr(nullptr)
   , mTCP_IOPtr(nullptr)
   , mUDP_IOPtr(nullptr)
{
   // mCallbacks.Add(aSimulation->GetObserver().ProcessInput.Connect(&WsfNetworkInterface::ProcessInput, this));
   mCallbacks.Add(WsfObserver::Initialize(&aSimulation).Connect(&WsfNetworkInterface::Initialize, this));

   mTCP_Server.SetOwnsConnections(false);
}

WsfNetworkInterface::~WsfNetworkInterface()
{
   mCallbacks.Clear();
   delete mTCP_IOPtr;
   delete mUDP_IOPtr;
}

bool WsfNetworkInterfaceInput::ProcessNetworkInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command;
   aInput.GetCommand(command);
   if (command == "port")
   {
      aInput.ReadValue(mSendPort);
      mReceivePort = mSendPort;
   }
   else if (command == "update_interval")
   {
      aInput.ReadValueOfType(mUpdateInterval, UtInput::cTIME);
   }
   else if (command == "send_port")
   {
      aInput.ReadValue(mSendPort);
   }
   else if (command == "receive_port")
   {
      aInput.ReadValue(mReceivePort);
   }
   else if (command == "update_on_advance_time")
   {
      aInput.ReadValue(mUpdateOnAdvanceTime);
   }
   else if (command == "update_on_external_timer")
   {
      aInput.ReadValue(mUpdateOnExternalTimer);
   }
   else if (command == "debug")
   {
      aInput.ReadValue(mDebug);
   }
   else if (command == "unicast")
   {
      aInput.ReadValue(mUDP_Addr);
      mUDP_Interface = "";
   }
   else if (command == "broadcast")
   {
      aInput.ReadValue(mUDP_Addr);
      mUDP_Interface = "";
   }
   else if (command == "multicast")
   {
      aInput.ReadValue(mUDP_Addr);
      aInput.ReadValue(mUDP_Interface);
   }
   else if (command == "connect_to_address")
   {
      aInput.ReadValue(mTCP_Addr);
   }
   else if (command == "time_to_live")
   {
      aInput.ReadValue(mTTL);
   }
   else if (command == "disable")
   {
      mIsDisabled = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfNetworkInterface::Initialize()
{
   if (mIsRequested && (!mIsDisabled))
   {
      if ((mSendPort == 0) && (mUDP_Addr.empty()))
      {
         auto out = ut::log::error() << "Interface was requested, but no port or address specified.";
         out.AddNote() << "Interface: " << mName;
         return false;
      }

      bool connected = false;
      if (!mUDP_Addr.empty())
      {
         mUDP_IOPtr = new GenUDP_IO();
         if (mUDP_Interface.empty()) // No interface specified; this is either broadcast or unicast.
         {
            mUDP_IOPtr->GetRecvSocket()->SetSocketOptions(GenSockets::GenSocket::cNON_BLOCKING);
            int attempt = 1;
            while (!connected)
            {
               ut::log::info() << "Connecting. Attempt " << attempt++ << ".";
               connected = mUDP_IOPtr->Init(mUDP_Addr, mSendPort, mReceivePort);
               if (connected)
               {
                  break;
               }
               else
               {
                  UtSleep::Sleep(0.1);
               }
            }
         }
         else // multicast
         {
            mUDP_IOPtr->GetRecvSocket()->SetSocketOptions(
               GenSockets::GenSocket::cNON_BLOCKING | GenSockets::GenSocket::cENABLE_BROADCAST |
               GenSockets::GenSocket::cENABLE_MULTICAST_LOOPBACK | GenSockets::GenSocket::cDISABLE_UNIQUE_BINDING_CHECK);
            connected = mUDP_IOPtr->Init(mUDP_Addr, mUDP_Interface, mSendPort, mReceivePort);
            mUDP_IOPtr->AddMulticastMembership(mUDP_Interface, mUDP_Addr);
            mUDP_IOPtr->GetSocket()->SetMulticastTTL(mTTL);
         }
         if (connected)
         {
            auto out = ut::log::info() << "UDP connection initialized.";
            out.AddNote() << "Interface: " << mName;
            out.AddNote() << "Address: " << mUDP_Addr;
            out.AddNote() << "Send Port: " << mSendPort;
            out.AddNote() << "Receive Port: " << mReceivePort;
         }
         else
         {
            auto out = ut::log::error() << "Unable to initialize UDP connection.";
            out.AddNote() << "Interface: " << mName;
            out.AddNote() << "Address: " << mUDP_Addr;
            out.AddNote() << "Send Port: " << mSendPort;
            out.AddNote() << "Receive Port: " << mReceivePort;
            return false;
         }
      }
      else if (mTCP_Addr.empty()) // tcp server
      {
         mTCP_Server.Init(mSendPort);
         mTCP_Server.GetSocket()->SetSocketOptions(GenSockets::GenSocket::cTCP_NODELAY |
                                                   GenSockets::GenSocket::cNON_BLOCKING);
         { // RAII block
            auto out = ut::log::info() << "Server waiting to accept client.";
            out.AddNote() << "Interface: " << mName;
            out.AddNote() << "Send Port: " << mSendPort;
            out.AddNote() << "Connecting...";
         }

         int timer = 0;
         while (!connected && (timer < 20)) // wait 20 times
         {
            mTCP_IOPtr = mTCP_Server.Accept(1000000); // wait 1 sec
            ++timer;
            connected = (mTCP_IOPtr != nullptr);
         }
         if (connected)
         {
            auto out = ut::log::info() << "Server made connection.";
            out.AddNote() << "Interface: " << mName;
         }
         else
         {
            auto out = ut::log::info() << "Server timed out.";
            out.AddNote() << "Interface: " << mName;
            return false;
         }
      }
      else // tcp client
      {
         auto out = ut::log::info() << mName << " Client: connecting to server at address " << mTCP_Addr << ": "
                                    << mSendPort;
         mTCP_IOPtr = new GenTCP_IO;

         if (mTCP_IOPtr->Init(mTCP_Addr, mSendPort))
         {
            mTCP_IOPtr->SetUseMessageHeaders(true);
            mIOPtr = dynamic_cast<GenNetIO*>(mTCP_IOPtr);
            out.AddNote() << mName << " Client:  successfully connected. ";
         }
         else
         {
            out.AddNote() << mName << " Client:  unable to connect. ";
            return false;
         }
      }

      if (!mUpdateOnExternalTimer)
      {
         if (mUpdateOnAdvanceTime)
         {
            mCallbacks.Add(WsfObserver::AdvanceTime(mSimulationPtr).Connect(&WsfNetworkInterface::NetworkUpdate, this));
         }
         else // Schedule update event
         {
            mSimulationPtr->AddEvent(ut::make_unique<NetworkUpdateEvent>(this));
         }
      }

      PrivateInitialize();
   }

   if (mIsDisabled)
   {
      auto out = ut::log::warning() << "Disabling interface due to user input.";
      out.AddNote() << "Interface: " << mName;
   }
   return true;
}

bool WsfNetworkInterface::IsActive() const
{
   return ((!mIsDisabled) && ((mTCP_IOPtr != nullptr) || (mUDP_IOPtr != nullptr)));
}

WsfNetworkInterface::NetworkUpdateEvent::NetworkUpdateEvent(WsfNetworkInterface* aWsfNetworkInterfacePtr)
   : mWsfNetworkInterfacePtr(aWsfNetworkInterfacePtr)
{
}

WsfEvent::EventDisposition WsfNetworkInterface::NetworkUpdateEvent::Execute()
{
   mWsfNetworkInterfacePtr->NetworkUpdate(GetTime());
   SetTime(GetTime() + mWsfNetworkInterfacePtr->GetUpdateInterval());
   return WsfEvent::cRESCHEDULE;
}
