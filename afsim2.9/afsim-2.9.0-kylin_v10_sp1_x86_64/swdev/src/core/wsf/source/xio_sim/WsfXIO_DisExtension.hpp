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

#ifndef WSFXIO_DISEXTENSION_HPP
#define WSFXIO_DISEXTENSION_HPP

#include "wsf_export.h"

class WsfXIO_AutoDisMapping;
class WsfXIO_DisService;
class WsfXIO_Simulation;

namespace WsfXIO_DisExtension_Detail
{
class PlatformListRequest;
}

//! Overrides the creation of DIS platforms.
//! Provides translation and routing of WsfMessage's sent over XIO
class WSF_EXPORT WsfXIO_DisExtension
{
protected:
   struct ConnectionData;

public:
   WsfXIO_DisExtension(WsfXIO_Simulation& aXIO_Simulation);
   virtual ~WsfXIO_DisExtension();

   bool Initialize();

   WsfXIO_AutoDisMapping* GetAutoDisMapping() { return mAutoDisMappingPtr; }

protected:
   WsfXIO_Simulation&     mXIO_Simulation;
   WsfXIO_AutoDisMapping* mAutoDisMappingPtr;
   WsfXIO_DisService*     mDisServicePtr;
};

#endif
