// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MULTIRESOLUTION_ROLES_HPP
#define MULTIRESOLUTION_ROLES_HPP

#include "WsfAcousticSignature.hpp"
#include "WsfComm.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfFuel.hpp"
#include "WsfInfraredSignature.hpp"
#include "WsfMover.hpp"
#include "WsfMultiresolutionPlatformComponent.hpp"
#include "WsfOpticalSignature.hpp"
#include "WsfProcessor.hpp"
#include "WsfRadarSignature.hpp"
#include "WsfSensor.hpp"

//! @name Role ID for multiresolution platform components.
enum
{
   // Note that this was chosen arbitrarily between 200-999 and appears to be unique. Documentation in
   // core/wsf/WsfComponentRoles.hpp indicates that these id values are "agreed to" and so this may change.
   cWSF_COMPONENT_MULTIRESOLUTION_MOVER              = 799, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_SENSOR             = 800, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_FUEL               = 801, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_COMM               = 802, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_PROCESSOR          = 803, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_ACOUSTIC_SIGNATURE = 804, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_RADAR_SIGNATURE    = 805, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_OPTICAL_SIGNATURE  = 806, // TBD
   cWSF_COMPONENT_MULTIRESOLUTION_INFRARED_SIGNATURE = 807  // TBD
};

WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfMover>, cWSF_COMPONENT_MULTIRESOLUTION_MOVER);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfSensor>, cWSF_COMPONENT_MULTIRESOLUTION_SENSOR);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfFuel>, cWSF_COMPONENT_MULTIRESOLUTION_FUEL);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<wsf::comm::Comm>, cWSF_COMPONENT_MULTIRESOLUTION_COMM);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfProcessor>,
                                cWSF_COMPONENT_MULTIRESOLUTION_PROCESSOR);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfAcousticSignature>,
                                cWSF_COMPONENT_MULTIRESOLUTION_ACOUSTIC_SIGNATURE);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfRadarSignature>,
                                cWSF_COMPONENT_MULTIRESOLUTION_RADAR_SIGNATURE);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfOpticalSignature>,
                                cWSF_COMPONENT_MULTIRESOLUTION_OPTICAL_SIGNATURE);
WSF_DECLARE_COMPONENT_ROLE_TYPE(WsfMultiresolutionPlatformComponent<WsfInfraredSignature>,
                                cWSF_COMPONENT_MULTIRESOLUTION_INFRARED_SIGNATURE);
#endif
