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

#ifndef WSFEM_ALARM_ANTENNA_HPP
#define WSFEM_ALARM_ANTENNA_HPP

#include "WsfEM_Interaction.hpp"
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;

//! An implementation of the ALARM 'antenna' class using WSF constructs.
namespace WsfEM_ALARM_Antenna
{
class antenna
{
public:
   antenna(WsfEM_XmtrRcvr* aXmtrRcvrPtr, WsfEM_Interaction& aInteraction, double tgt_az, double tgt_el, double slant_range);

   double get_az_point_ang();
   double get_height_agl();
   double get_height_msl();
   void   get_relative_gain(double  az_angle,  // in, radians
                            double  el_angle,  // in, radians
                            double& rel_gain); // out, absolute
   void   offbor(double alphar, double epslnr, double alphap, double epslnp, double& offazp, double& offelp);

private:
   WsfEM_XmtrRcvr*    mXmtrRcvrPtr;
   WsfEM_Interaction& mInteraction;
   WsfEM_Xmtr*        mXmtrPtr;
   double             mEBS_Az;
   double             mEBS_El;

   double az_point_ang_rad;
   double el_point_ang_rad;
};

double get_az_point_ang(antenna& ant_data);
double get_height_agl(antenna& ant_data);
double get_height_msl(antenna& ant_data);
void   get_relative_gain(antenna& ant_data,
                         double   az_angle, // in, radians
                         double   el_angle, // in, radians
                         double&  rel_gain); // out, absolute

} // namespace WsfEM_ALARM_Antenna

#endif
