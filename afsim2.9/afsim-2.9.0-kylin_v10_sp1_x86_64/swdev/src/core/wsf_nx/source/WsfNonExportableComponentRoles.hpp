// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFNONEXPORTABLECOMPONENTROLES_HPP
#define WSFNONEXPORTABLECOMPONENTROLES_HPP

// reserved block 300 - 325 for wsf_nx
enum
{
   cWSF_COMPONENT_TRIMSIM_SENSOR = 300, //!< TDOA TRIMSIM extension for WsfSensor
   cWSF_COMPONENT_CHAFF_PARCEL   = 301, //!< Chaff parcel extension for WsfPlatform
   cWSF_COMPONENT_CHAFF_EJECTOR  = 302, //!< Chaff ejector extension for WsfWeapon
};

#endif
