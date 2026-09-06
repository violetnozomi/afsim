// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEM_ALARM_GEOMETRY_HPP
#define WSFEM_ALARM_GEOMETRY_HPP

class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfPlatform;

namespace WsfEM_ALARM_Geometry
{
void ComputeGeometry(WsfEM_XmtrRcvr* aXmtrRcvrPtr,
                     WsfPlatform*    aTargetPtr,
                     WsfEM_Xmtr*     aXmtrPtr,
                     double&         rdr_height_msl,
                     double&         rdr_lat,
                     double&         rdr_lon,
                     double&         rkfact,
                     double&         tgt_height_msl,
                     double&         tgt_lat,
                     double&         tgt_lon,
                     double&         ground_range,
                     double&         tanept,
                     double&         slant_range,
                     double&         tgt_az,
                     double&         tgt_el,
                     double&         tgt_x,
                     double&         tgt_z);
}

#endif
