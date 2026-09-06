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

#ifndef WSFXIO_INPUTDATA_HPP
#define WSFXIO_INPUTDATA_HPP

#include "wsf_export.h"

#include <set>
#include <vector>

class UtInput;
#include "UtCallback.hpp"
#include "UtOptional.hpp"
#include "WsfStringId.hpp"
#include "ext/WsfExtEntityId.hpp"
#include "xio/WsfXIO_SerializeTypes.hpp"

//! Contains the 'xio_interface' input data.
//! This class is derived from by both the scenario extension (WsfXIO_Extension) and the simulation
//! extension (WsfXIO_Interface). The data is filled by the scenario extension and simply copied
//! to the simulation extension.
class WSF_EXPORT WsfXIO_InputData
{
public:
   enum UDP_Type
   {
      cBROADCAST,
      cMULTICAST,
      cUNICAST
   };

   struct UDP_Target
   {
      UDP_Target()
         : mSendPort(0)
         , mRecvPort(0)
         , mConnectionId(0)
      {
      }
      UDP_Type    mType;
      std::string mAddress;
      std::string mInterface;
      int         mSendPort;
      int         mRecvPort;
      int         mConnectionId;
   };

   WsfXIO_InputData();

   bool ProcessInput(UtInput& aInput);

   //! An input processing callback list.
   class InputCallbackList : public UtCallbackListN<bool(UtInput&)>
   {
   public:
      bool Callback(UtInput& aInput)
      {
         for (IterType it = mCallbackList.begin(); it != mCallbackList.end();)
         {
            if (static_cast<CallbackType*>(*(it++))->operator()(aInput))
            {
               return true;
            }
         }
         return false;
      }
   };

   //! XIO extenders can subscribe to this to add additional input into the 'xio_interface' block.
   InputCallbackList ProcessInputEvent;

   //! The current application's name
   WsfStringId mApplicationName;

   //! Socket port used to receive TCP connections
   int mTCP_Port;

   //! Time to live for multicast connections
   int mMulticastTimeToLive;
   //! Determines if sent multicast can be received on the local machine
   bool mMulticastLoopback;

   double mHeartbeatInterval;

   ut::optional<double> mFallingBehindThreshold;

   //! Set of application types to connect with automatically
   std::set<int> mApplicationTypeFilter;

   //! Contains user input UDP target information
   std::vector<UDP_Target> mUDP_Targets;

   std::vector<WsfXIO_EntityId> mRegularApplications;
   std::vector<WsfXIO_EntityId> mAutoMappedApplications;
   bool                         mAutoMappingDefault;

   int mPublisherDebugLevel;

   bool mDebugEnabled;
   bool mShowConnections;
   bool mShowTransferRate;
   bool mPauseForConnection;

   bool mInterfaceRequested; //!< true if user has requested the XIO interface

   bool mAuxDataUpdates = true;

   void AddConnectType(int aApplicationType);

private:
   bool ProcessPorts(UtInput& aInput, int& aSendPort, int& aRecvPort);

   WsfExtEntityId ReadApplicationId(UtInput& aInput);
};

#endif
