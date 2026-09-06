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

#include "xio_sim/WsfXIO_DisExtension.hpp"

#include "UtLog.hpp"
#include "xio/WsfXIO_Interface.hpp"
#include "xio_sim/WsfXIO_AutoDisMapping.hpp"
#include "xio_sim/WsfXIO_DisService.hpp"
#include "xio_sim/WsfXIO_Simulation.hpp"

// =================================================================================================
WsfXIO_DisExtension::WsfXIO_DisExtension(WsfXIO_Simulation& aXIO_Simulation)
   : mXIO_Simulation(aXIO_Simulation)
   , mAutoDisMappingPtr(nullptr)
   , mDisServicePtr(nullptr)
{
}

// =================================================================================================
WsfXIO_DisExtension::~WsfXIO_DisExtension()
{
   delete mAutoDisMappingPtr;
   delete mDisServicePtr;
}

// =================================================================================================
bool WsfXIO_DisExtension::Initialize()
{
   bool ok(true);
   mDisServicePtr = new WsfXIO_DisService(mXIO_Simulation);

   // If auto DIS mapping has been requested, allocate and initialize the interface.
   WsfXIO_Interface* interfacePtr = mXIO_Simulation.GetInterface();
   if (interfacePtr->mAutoMappingDefault || (!interfacePtr->mAutoMappedApplications.empty()))
   {
      mAutoDisMappingPtr = new WsfXIO_AutoDisMapping(mXIO_Simulation, this);
      if (!mAutoDisMappingPtr->Initialize())
      {
         ut::log::error() << "auto_dis_mapping: Initialization failed.";
         ok = false;
      }
   }
   return ok;
}
