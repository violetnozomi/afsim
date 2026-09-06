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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef GeoTileManager_hpp
#define GeoTileManager_hpp

#include "geodata_export.h"

//! 'GeoTileManager' is base class that is used to load and unload 'tiles' of
//! geographic information as required by the application.
class GEODATA_EXPORT GeoTileManager
{
public:
   GeoTileManager();
   virtual ~GeoTileManager();

   //! Return an integer representing the type of tile manager.
   //! (could be an enumerated value or a mapped string).
   int GetType() const { return mType; }

   //! Set an integer representing the type of tile manager.
   void SetType(int aType) { mType = aType; }

private:
   int mType;
};

#endif
