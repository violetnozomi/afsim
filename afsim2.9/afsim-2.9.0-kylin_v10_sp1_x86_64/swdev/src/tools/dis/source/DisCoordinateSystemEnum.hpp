// *****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DISCOORDINATESYSTEMENUM_HPP
#define DISCOORDINATESYSTEMENUM_HPP

#include "dis_export.h"

//! CoordinateSystem type
enum DIS_EXPORT DisCoordinateSystemEnum
{
   //! local topographic projection: east, north, up
   cRIGHT_HANDED_CARTESIAN = 0,
   //! local topographic projection: east, north, down
   cLEFT_HANDED_CARTESIAN     = 1,
   cLATITUDE_LONGITUDE_HEIGHT = 2,
   cLATITUDE_LONGITUDE_DEPTH  = 3
};
#endif
