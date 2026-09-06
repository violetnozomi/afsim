//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************


#ifndef DIS_DISGEOMETRICRECORDTYPEENUM_HPP
#define DIS_DISGEOMETRICRECORDTYPEENUM_HPP

#include "dis_export.h"

//! DisEnvironmentalRecord type specifier
enum class DisEnvironmentalAndGeometricRecordTypeEnum
{
   NONE                    = 250,
   POINT_RECORD_1          = 251,
   POINT_RECORD_2          = 252,
   LINE_RECORD_1           = 253,
   LINE_RECORD_2           = 254,
   BOUNDING_SPHERE_RECORD  = 255,
   SPHERE_RECORD           = 256,
   SPHERE_RECORD_2         = 257,
   ELLIPSOID_RECORD_1      = 258,
   ELLIPSOID_RECORD_2      = 259,
   CONE_RECORD_1           = 260,
   CONE_RECORD_2           = 261,
   RECTANGULAR_RECORD_1    = 262,
   RECTANGULAR_RECORD_2    = 263,
   GAUSSIAN_PLUME_RECORD   = 264,
   GAUSSIAN_PUFF_RECORD    = 265,
   UNIFORM_GEOMETRY_RECORD = 266,
   RECTANGULAR_RECORD_3    = 267,
   COMBIC_STATE            = 268,
   FLARE_STATE             = 269
};
#endif
