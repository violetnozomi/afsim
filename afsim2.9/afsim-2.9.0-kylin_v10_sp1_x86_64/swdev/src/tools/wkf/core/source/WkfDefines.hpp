// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFDEFINES_HPP
#define WKFDEFINES_HPP

#include <QObject>

namespace wkf
{
// This ROLES are designed to be used with QVariant that can be specified on a QObject or QItems such as QTreeWidgetItem
// or QTableWidgetItems
//  The value is a high value that is unlikely to collide with any values a developer may choose to use in their own plugin.
enum : int
{
   PLUGIN_ID_ROLE = Qt::UserRole + 101220,
   UPDATER_ROLE, // we use this to indicate a tree item is associated with an updater
   ID_ROLE,
   FILEPATH_ROLE,
   UPDATER_CONTEXT, // we use this to provide some context to an updater
   UNHIDEABLE,      // indicates that a PlatformDataTreeItem is unhideable via preferences
   DEFAULT_HIDDEN,  // indicates that a PlatformDataTreeItem defaults to being hidden
   DISPLAY_PRECEDENCE // indicates we want to show the TreeItem in a place relative to other items.  Unset is treated as if 1000.
};

enum SpatialDomain : int
{
   SPATIAL_DOMAIN_UNKNOWN    = 0, //! unknown or undefined
   SPATIAL_DOMAIN_LAND       = 1, //! Cars, trucks, buildings, etc.
   SPATIAL_DOMAIN_AIR        = 2, //! Airplanes, etc.
   SPATIAL_DOMAIN_SURFACE    = 3, //! Ships, boats, etc.
   SPATIAL_DOMAIN_SUBSURFACE = 4, //! Submarines
   SPATIAL_DOMAIN_SPACE      = 5, //! Satellites

   SPATIAL_DOMAIN_COUNT
};

//! An enumeration for IFF
enum IFF_Status : int
{
   //! No IFF status is being reported.
   cIFF_UNKNOWN = 0,
   //! The reporter cannot determine if the object is a friend or a foe.
   cIFF_AMBIGUOUS = 1,
   //! The object has been determined to be a foe,
   cIFF_FOE = 2,
   //! The object has been determined to be a friend
   cIFF_FRIEND = 3,
   //! The object has been determined to be neutral
   cIFF_NEUTRAL = 4
};
} // namespace wkf
#endif
