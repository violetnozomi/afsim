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

#ifndef DISSTATIONNAMEENUM_HPP
#define DISSTATIONNAMEENUM_HPP

/**!
 * Defines the types of stations within a host entity that a
 * part entity is located.
 */
enum DisStationNameEnum
{
   cSTATION_OTHER                     = 0,
   cAIRCRAFT_WING_STATION             = 1,
   cSHIP_FORWARD_GUN_MOUNT_STARBOARD  = 2,
   cSHIP_FORWARD_GUN_MOUNT_PORT       = 3,
   cSHIP_FORWARD_GUN_MOUNT_CENTERLINE = 4,
   cSHIP_AFT_GUN_MOUNT_STARBOARD      = 5,
   cSHIP_AFT_GUN_MOUNT_PORT           = 6,
   cSHIP_AFT_GUN_MOUNT_CENTERLINE     = 7,
   cFORWARD_TORPEDO_TUBE              = 8,
   cAFT_TORPEDO_TUBE                  = 9,
   cBOMB_BAY                          = 10,
   cCARGO_BAY                         = 11,
   cTRUCK_BED                         = 12,
   cTRAILER_BED                       = 13,
   cWELL_DECK                         = 14,
   cON_STATION_RANGE_AND_BEARING      = 15,
   cON_STATION_XYZ                    = 16,
   cAIR_TO_AIR_REFUELING_BOOM         = 17,
   cAERIAL_REFUELING_RECEPTACLE       = 18
};
#endif
