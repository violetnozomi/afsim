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

#ifndef GeoTextureRecord_hpp
#define GeoTextureRecord_hpp

#include "geodata_export.h"

class GEODATA_EXPORT GeoTextureRecord
{
public:
   GeoTextureRecord();
   GeoTextureRecord(const double& neLatA,
                    const double& neLonA,
                    const double& swLatA,
                    const double& swLonA,
                    const float&  heightPixelA,
                    const float&  widthPixelA,
                    const int&    numPixelTrimA = 0,
                    const float&  latOffsetA    = 0.0,
                    const float&  lonOffsetA    = 0.0);

   ~GeoTextureRecord();

protected:
   double neLatM;
   double neLonM;
   double swLatM;
   double swLonM;
   float  heightPixelM;
   float  widthPixelM;
   float  latOffsetM;
   float  lonOffsetM;
   int    numPixelTrimM;

   friend class GeoTexture;
   friend class GeoEarthSection;
};


#endif
