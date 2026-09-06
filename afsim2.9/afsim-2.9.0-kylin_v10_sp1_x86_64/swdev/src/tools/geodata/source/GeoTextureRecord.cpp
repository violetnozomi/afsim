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

#include "GeoTextureRecord.hpp"

GeoTextureRecord::GeoTextureRecord() {}

GeoTextureRecord::GeoTextureRecord(const double& neLatA,
                                   const double& neLonA,
                                   const double& swLatA,
                                   const double& swLonA,
                                   const float&  heightPixelA,
                                   const float&  widthPixelA,
                                   const int&    numPixelTrimA,
                                   const float&  latOffsetA,
                                   const float&  lonOffsetA)
   : neLatM(neLatA)
   , neLonM(neLonA)
   , swLatM(swLatA)
   , swLonM(swLonA)
   , heightPixelM(heightPixelA)
   , widthPixelM(widthPixelA)
   , latOffsetM(latOffsetA)
   , lonOffsetM(lonOffsetA)
   , numPixelTrimM(numPixelTrimA)

{
}

GeoTextureRecord::~GeoTextureRecord() {}
