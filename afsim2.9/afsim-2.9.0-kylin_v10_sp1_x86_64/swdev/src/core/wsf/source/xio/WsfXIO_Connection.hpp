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

#ifndef WSFXIO_CONNECTION_HPP
#define WSFXIO_CONNECTION_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

#include "GenUniqueId.hpp"
#include "PakConnection.hpp"
class PakSocketIO;
class PakTCP_IO;
class PakUDP_IO;
#include "UtReferenceTracked.hpp"
class UtScriptClass;
class UtScriptTypes;
#include "WsfXIO.hpp"
class WsfXIO_Packet;
class WsfXIO_InitPkt;
class WsfXIO_Interface;

//! WsfXIO_Connection provides a means to communicate between applications.
//! It may represent a single connection to a remote application,
//! Or a broadcast/multicast setup between several applications.
class WSF_EXPORT WsfXIO_Connection : public PakConnection, public UtReferenceTracked
{
public:
   friend class WsfXIO_Interface;

   //! Returns a pointer to the linked connection if available
   //! This is only valid for TCP connections, and returns an unreliable connection
   //! capable of communicating to the remote application
   WsfXIO_Connection* GetLinkedConnection() const { return mLinkedConnectionPtr; }

   //! Sets the linked connection
   void SetLinkedConnection(WsfXIO_Connection* aConnectionPtr) { mLinkedConnectionPtr = aConnectionPtr; }

   //! Returns the IO Device used in this connection.
   PakSocketIO& GetIO() const { return *mIOPtr; }

   //! Returns the GenTCP_IO is this connection uses TCP, returns null otherwise
   PakTCP_IO* GetTCP_IO() const { return mTCP_IO_Ptr; }

   //! Returns the GenUDP_IO is this connection uses UDP, returns null otherwise
   PakUDP_IO* GetUDP_IO() const { return mUDP_IO_Ptr; }

   //! Returns the associated WsfXIO_Interface
   WsfXIO_Interface& GetInterface() { return *mInterfacePtr; }

   //! Returns the application type
   wsf::xio::ApplicationType GetApplicationType() const { return mApplicationType; }

   //! Sets the application type
   void SetApplicationType(wsf::xio::ApplicationType aApplicationType) { mApplicationType = aApplicationType; }

   //! Sends a packet to this connection
   void Send(WsfXIO_Packet& aPkt);

   //! Returns a locally unique identifier for this connection
   int GetConnectionId() const { return mConnectionId; }

   //! Returns the name of the connected application
   const std::string& GetApplicationName() const { return mApplicationName; }

   //! Sets the name of the connected application
   void SetApplicationName(const std::string& aApplicationName) { mApplicationName = aApplicationName; }

   //! Sets the application ID
   void SetApplicationId(GenUniqueId& aApplicationId) { mApplicationId = aApplicationId; }

   //! Returns the remote application's unique ID
   //! This is only valid for TCP connections (UDP may have multiple listeners)
   GenUniqueId GetApplicationId() const { return mApplicationId; }

   //! Returns true if the connection is reliable
   bool IsReliable() const { return mTCP_IO_Ptr != nullptr; }

   //! Specifies that the connection is initialized
   void SetInitialized() { mIsInitialized = true; }

   //! Returns 'true' if the connection is initialized.
   bool IsInitialized() const { return mIsInitialized; }

   //! Returns 'true' if the connection is in the process of disconnecting
   bool IsDisconnecting() const { return mDisconnecting; }

   void SetDisconnecting() { mDisconnecting = true; }

   void SetClockOffset(double aRecvOffset);

   double GetClockOffset() const;

   //! Returns 'true' if the clock offset is configured for packet synchronization with this connection
   bool HasClockTranslation() { return mHasClockTranslation; }
   void SetHasClockTranslation(bool aUseTranslation) { mHasClockTranslation = aUseTranslation; }

   //! Returns the last time stamp (simulation-time) from a PDU received by this connection.
   double GetLastTimeStamp() const { return mLastTimeStamp; }
   void   SetLastTimeStamp(double aTimeStamp) { mLastTimeStamp = aTimeStamp; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(UtScriptTypes* aScriptTypesPtr);
   static UtScriptClass*                 GetScriptClass() { return mScriptClassPtr; }

   static UtScriptClass* mScriptClassPtr;

protected:
   // Connections are created and deleted by WsfXIO_Interface

   WsfXIO_Connection(WsfXIO_Interface* aInterfacePtr, PakSocketIO* aIOPtr);

   ~WsfXIO_Connection() override;

private:
   std::string               mApplicationName;
   GenUniqueId               mApplicationId;
   int                       mConnectionId;
   WsfXIO_Interface*         mInterfacePtr;
   WsfXIO_Connection*        mLinkedConnectionPtr;
   PakSocketIO*              mIOPtr;
   PakTCP_IO*                mTCP_IO_Ptr;
   PakUDP_IO*                mUDP_IO_Ptr;
   wsf::xio::ApplicationType mApplicationType;
   bool                      mIsServer;
   bool                      mIsInitialized;
   bool                      mDisconnecting;
   bool                      mHasClockTranslation;
   double                    mRecvClockOffset;
   double                    mLastTimeStamp;
};

#endif
