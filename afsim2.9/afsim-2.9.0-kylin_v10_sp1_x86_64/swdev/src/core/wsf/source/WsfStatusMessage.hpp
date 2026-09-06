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

#ifndef WSFSTATUSMESSAGE_HPP
#define WSFSTATUSMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"
#include "WsfTrackId.hpp"

//! A 'status message' provides status about an operation.
class WSF_EXPORT WsfStatusMessage : public WsfMessage
{
public:
   WsfStatusMessage(); // For XIO (de)serialization
   WsfStatusMessage(WsfSimulation& aSimulation);
   WsfStatusMessage(WsfPlatform* aPlatformPtr);
   WsfStatusMessage(WsfStringId aStatusId, WsfPlatform* aPlatformPtr);

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! @name Status methods.
   //!
   //! The message status is a string ID that represents the status being reported.
   //! The status values and their interpretation are defined by convention.
   //@{
   //! Get the status (string ID form).
   WsfStringId GetStatusId() const { return GetSubType(); }

   const std::string& GetStatus() const;

   //! Set the status (string ID form).
   void SetStatusId(WsfStringId aStatusId) { SetSubType(aStatusId); }

   void SetStatus(WsfStringId aStatus);
   //@}

   //! @name Request ID methods.
   //! A status may be associated with a particular request (e.g.: a command).
   //! These methods are concerned with the identifier of the request.
   //@{
   //! Get the request ID to which the status applies.
   const WsfTrackId& GetRequestId() const { return mRequestId; }

   //! Set the request ID to which the status applies.
   void SetRequestId(const WsfTrackId& aRequestId) { mRequestId = aRequestId; }
   //@}

   //! @name System name methods.
   //! A status may be associated with a particular system (e.g.: sensor, comm, mover, etc.)
   //! These methods are concerned the name of the system to which the status applies.
   //@{
   //! Get the system name (string ID form).
   //! @return The string ID of the system name, or zero if the status is not associated
   //! with a particular system.
   WsfStringId GetSystemNameId() const { return mSystemNameId; }

   std::string GetSystemName() const;

   //! Set the system name (string ID form).
   //! @param aSystemNameId The string ID of the system name to which the status applied.
   //! If zero then the status does not apply to a particular system.
   void SetSystemNameId(WsfStringId aSystemNameId) { mSystemNameId = aSystemNameId; }

   void SetSystemName(const std::string& aSystemName);
   //@}

   //! @name Platform methods.
   //! A status may sometimes deal with another platform (such as a spawned object).
   //! \note This is not the platform that is issuing the message. The base class
   //! 'Originator' methods define who is sending the message.
   //@{
   WsfPlatform* GetPlatform() const;
   size_t       GetPlatformIndex() const { return mPlatformIndex; }

   void SetPlatform(WsfPlatform* aPlatformPtr);
   void SetPlatformIndex(size_t aPlatformIndex) { mPlatformIndex = aPlatformIndex; }
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mRequestId& mSystemNameId& mPlatformIndex;
   }

protected:
   // Using the default copy constructor.
   // WsfStatusMessage(const WsfStatusMessage& aSrc);

   //! The request ID to which the status pertains.
   WsfTrackId mRequestId;

   //! The system name ID to which the status pertains.
   WsfStringId mSystemNameId;

   //! The other platform index.
   size_t mPlatformIndex;
};

#endif
