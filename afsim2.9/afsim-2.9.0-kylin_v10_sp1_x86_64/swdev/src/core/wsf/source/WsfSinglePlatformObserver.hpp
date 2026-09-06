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

#ifndef WSFSINGLEPLATFORMOBSERVER_HPP
#define WSFSINGLEPLATFORMOBSERVER_HPP

#include "wsf_export.h"
class WsfPlatform;

//! A light-weight observer of a single platform.
//!
//! A object may register an instance of this class with a particular platform in order to receive
//! notification of the events defined by the virtual methods. Unlike the simulation observers which
//! receive notification of ALL occurrences of a subscribed event, this is directed at observing
//! a SINGLE platform. It is lighter weight and faster, but the user must take care to follow the
//! process described below.
//!
//! The typical use is to subclass this, define one or more of the virtual methods and attach to the
//! platform using WsfPlatform::AttachObserver(). Once an object attaches, it MUST detach from the
//! platform using WsfPlatform::DetachObserver prior to its destruction.
class WSF_EXPORT WsfSinglePlatformObserver
{
public:
   virtual ~WsfSinglePlatformObserver() {}

   //! Called before the platform is actually deleted.
   virtual void OnPlatformDeleting(double aSimTime, WsfPlatform* aPlatformPtr) {}

   //! Called just before the platform is removed from the simulation.
   virtual void OnPlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr) {}

   //! Called after each mover update.
   virtual void OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr) {}
};

#endif
