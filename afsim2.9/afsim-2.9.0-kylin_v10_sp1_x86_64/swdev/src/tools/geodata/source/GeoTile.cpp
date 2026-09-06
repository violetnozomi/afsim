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

#include "GeoTile.hpp"

GeoTile::GeoTile()
   : GeoRect()
   , mTileMatrixPtr(nullptr)
   , mTileIndex(0)
   , mUseCount(0)
{
}

// virtual
GeoTile::~GeoTile() {}
