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

#ifndef WSFMILCOMPONENTROLES_HPP
#define WSFMILCOMPONENTROLES_HPP

// reserved block 200 - 250 for wsf_mil
enum
{
   cWSF_COMPONENT_WEAPON                    = 200, //!< Weapon component
   cWSF_COMPONENT_WEAPON_PLATFORM_EXTENSION = 201, //!< Platform extension for weapon support
   cWSF_COMPONENT_WEAPON_TASK_MANAGER       = 202, //!< WsfTaskManager extension for weapons
   cWSF_COMPONENT_WEAPON_XIO_COMPONENT      = 203, //!< XIO extension for weapons
   cWSF_COMPONENT_MIL_DIS_EXTENSION         = 204,
   cWSF_COMPONENT_MIL_DIS_PLATFORM          = 205,
   cWSF_COMPONENT_MIL_EXT_INTERFACE         = 206,
   cWSF_COMPONENT_EW_RESULT                 = 207, //!< EW extension for WsfEM_Interaction
   cWSF_COMPONENT_EW_RCVR                   = 208, //!< EW extension for WsfEM_Rcvr
   cWSF_COMPONENT_EW_XMTR                   = 209, //!< EW extension for WsfEM_Xmtr
   cWSF_COMPONENT_EW_COMM                   = 210, //!< EW extension for WsfComm
   cWSF_COMPONENT_EW_SENSOR                 = 211, //!< EW extension for WsfSensor
   cWSF_COMPONENT_LAUNCH_HANDOFF_DATA       = 212, //!< Launch handoff data
   cWSF_COMPONENT_DIRECTED_ENERGY_WEAPON    = 213, //!< Directed energy weapon component
   cWSF_COMPONENT_COMM_HW_JTIDS             = 214,
   cWSF_COMPONENT_COMM_HW_SUBSURFACE        = 215,
   cWSF_COMPONENT_COMM_HW_LASER             = 216,
   cWSF_COMPONENT_LASER_RCVR                = 217, //!< LASER extension for WsfEM_Rcvr
   cWSF_COMPONENT_LASER_XMTR                = 218, //!< LASER extension for WsfEM_Xmtr
   cWSF_COMPONENT_CTD_SENSOR                = 219, //!< Close Target Detection (CTD) model sensor component
   cWSF_COMPONENT_RF_JAMMER                 = 220  //!< Jammer component
};

enum
{
   //! @name WSF 1.x ordering...
   //! The following impose initialization order that is the same as WSF 1.x This isn't strictly necessary but it
   //! makes regression testing possible.
   //@{
   cWSF_INITIALIZE_ORDER_WEAPON = -300000000 // This is the RESERVED value in WsfComponentRoles.hpp
                                             //@}
};

#endif
