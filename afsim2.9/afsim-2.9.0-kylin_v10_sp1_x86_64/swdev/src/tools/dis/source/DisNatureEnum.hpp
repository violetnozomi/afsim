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

#ifndef DISNATUREENUM_HPP
#define DISNATUREENUM_HPP

/**!
 * Specifies the types of relationships or purposes for a part entity to join a
 * host entity.
 */
enum DisNatureEnum
{
   cNATURE_OTHER                                          = 0,
   cHOST_FIREABLE_MUNITION                                = 1,
   cMUNITION_CARRIED_AS_CARGO                             = 2,
   cFUEL_CARRIED_AS_CARGO                                 = 3,
   cGUNMOUNT_ATTACHED_TO_HOST                             = 4,
   cCOMPUTER_GENERATED_FORCES_CARRIED_AS_CARGO            = 5,
   cVEHICLE_CARRIED_AS_CARGO                              = 6,
   cEMITTER_MOUNTED_ON_HOST                               = 7,
   cMOBILE_COMMAND_AND_CONTROL_ENTITY_CARRIED_ABOARD_HOST = 8,
   // For number 9, please refer to the enhanced rotary wing aircraft record [UID 220]
   cENTITY_STATION_AT_POSITION_WITH_RESPECT_TO_HOST = 9,
   cTEAM_MEMBER_IN_FORMATION_WITH                   = 10
};
#endif
