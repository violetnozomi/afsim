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

#ifndef WSFCONTROLMESSAGE_HPP
#define WSFCONTROLMESSAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>

class UtScriptClass;
class UtScriptTypes;
#include "WsfMessage.hpp"
class WsfPlatform;
#include "WsfRoute.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"

//! A 'control message' provides the ability for a platform to direct another platform.
//!
//! The 'control message' provides the ability of a platform to direct another platform
//! (i.e.: issue an order for change-route, return-home, refuel, etc).

class WSF_EXPORT WsfControlMessage : public WsfMessage
{
public:
   WsfControlMessage(); // For XIO (de)serialization.
   WsfControlMessage(WsfPlatform* aPlatformPtr);

   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   static WsfStringId GetTypeId();

   //! @name Message function methods.
   //!
   //! The message function is a string ID that represents the function (or command)
   //! that is to be performed.  The functions and their interpretation are defined by
   //! convention.
   //@{
   WsfStringId GetFunction() const;

   void SetFunction(WsfStringId aFunction);
   //@}

   //! @name Request ID methods.
   //!
   //! A command typically has an associated request ID.
   //! These methods are concerned with the identifier of the request.
   //@{
   //! Get the request ID to which the status applies.
   const WsfTrackId& GetRequestId() const { return mRequestId; }

   //! Set the request ID to which the status applies.
   void SetRequestId(const WsfTrackId& aRequestId) { mRequestId = aRequestId; }
   //@}

   //! @name Resource methods.
   //!
   //! A command may have an associated resource (sensor, etc.).  The resource
   //! is a string ID of the required resource.
   //@{
   //! Get the resource (string ID form).
   //! @returns The string ID of the resource.  The value will be 0 if no resource was supplied.
   WsfStringId GetResourceId() const { return mResourceId; }

   std::string GetResource() const;

   //! Set the resource (string ID form).
   void SetResourceId(WsfStringId aResourceId) { mResourceId = aResourceId; }

   void SetResource(const std::string& aResource);
   //@}

   //! @name Track methods.
   //!
   //! A message may or may not have an associated track.
   //! The track may represent a target to be attacked, a sensor cue, etc.
   //@{
   //! Return the track (const form).
   const WsfTrack& GetTrack() const { return mTrack; }

   //! Return the track (non-const form).
   WsfTrack& GetTrack() { return mTrack; }

   void SetTrack(const WsfTrack& aTrack);
   //@}

   //! @name Route methods.
   //!
   //! A message may or may not have an associated route.
   //@{
   //! Return the route (const form).
   const WsfRoute& GetRoute() const { return mRoute; }

   //! Return the route (non-const form).
   WsfRoute& GetRoute() { return mRoute; }

   void SetRoute(const WsfRoute& aRoute);
   //@}

   //! For XIO (de)serialization.
   template<typename T>
   void Serialize(T& aBuff)
   {
      WsfMessage::Serialize(aBuff);
      aBuff& mRequestId& mResourceId& mTrack& mRoute;
   }

protected:
   WsfControlMessage(const WsfControlMessage& aSrc);
   WsfControlMessage& operator=(const WsfControlMessage& aRhs);

   WsfTrackId  mRequestId;
   WsfStringId mResourceId; //!< String ID of the resource.
   WsfTrack    mTrack;      //!< The associated track.
   WsfRoute    mRoute;      //!< The associated route.
};

#endif
