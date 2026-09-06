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

#ifndef WSFTYPES_HPP
#define WSFTYPES_HPP

#include "wsf_export.h"

#include <string>

//! A collection of enumerations and type definitions that are used across a number of components.

//! Define the 'domain' of an entity (land, air, etc.)
//!
//! The 'domain' provides broad categorization of where and platform primarily exists
//! (a car on land, a plane in the air, etc.). The default value for a platform is defined by the type
//! of mover that is attached to the platform.
//!
//! The enumeration values coincide with the definitions in used by IEEE Standard for Distributed
//! Interaction Simulation (DIS), 1278.1 and the accompanying DIS Enumerations document.
//!
//! @note The values must be usable as a bit number (See WsfSensor).
enum WsfSpatialDomain
{
   WSF_SPATIAL_DOMAIN_UNKNOWN    = 0, //! unknown or undefined
   WSF_SPATIAL_DOMAIN_LAND       = 1, //! Cars, trucks, buildings, etc.
   WSF_SPATIAL_DOMAIN_AIR        = 2, //! Airplanes, etc.
   WSF_SPATIAL_DOMAIN_SURFACE    = 3, //! Ships, boats, etc.
   WSF_SPATIAL_DOMAIN_SUBSURFACE = 4, //! Submarines
   WSF_SPATIAL_DOMAIN_SPACE      = 5  //! Satellites
};

namespace WsfTypes
{
bool WSF_EXPORT StringToEnum(const std::string& aString, WsfSpatialDomain& aSpatialDomain);

std::string WSF_EXPORT EnumToString(WsfSpatialDomain aSpatialDomain);
} // namespace WsfTypes

#endif
