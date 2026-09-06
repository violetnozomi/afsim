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

#include "xio/WsfXIO_WeaponInterface.hpp"

#include "xio/WsfXIO_Interface.hpp"
#include "xio/WsfXIO_WeaponPacketRegistry.hpp"

// =================================================================================================
// PROGRAMMING NOTE: This file should not refer to any WSF object. This ensures that a non-WSF
//                   application doesn't link with any of the WSF objects UNLESS they otherwise
//                   reference them.
// =================================================================================================

// =================================================================================================
//! Create the XIO Weapon Interface for a non-WSF application.
//! The calling application will typically do something like:
//! \code
//!    WsfXIO_Interface* interfacePtr = new WsfXIO_Interface;
//!    WsfXIO_WeaponInterface::Create(interfacePtr);
//! \endcode
void WsfXIO_WeaponInterface::Create(WsfXIO_Interface* aInterfacePtr)
{
   WsfXIO_WeaponPacketRegistry::RegisterPackets(*aInterfacePtr);
}
