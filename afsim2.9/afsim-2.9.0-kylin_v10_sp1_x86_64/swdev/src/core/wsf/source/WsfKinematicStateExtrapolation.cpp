//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfKinematicStateExtrapolation.hpp"

wsf::KinematicStateExtrapolation::KinematicStateExtrapolation()
   : TrackExtrapolation()
{
   SetType(GetTypeId());
}

//! Get the type ID associated with this message.
//! @return The string ID of this message.
// static
WsfStringId wsf::KinematicStateExtrapolation::GetTypeId()
{
   return "WSF_KINEMATIC_STATE_EXTRAPOLATION";
}
