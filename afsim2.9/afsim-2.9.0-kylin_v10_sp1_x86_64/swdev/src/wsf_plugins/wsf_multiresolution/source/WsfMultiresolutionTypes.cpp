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

#include "WsfMultiresolutionTypes.hpp"

void WsfMultiresolutionTypesRegistration::AddedToScenario()
{
   // Platform components
   AddMultiresolutionType<wsf::comm::Comm>();
   AddMultiresolutionType<WsfFuel>();
   AddMultiresolutionType<WsfMover>();
   AddMultiresolutionType<WsfProcessor>();
   AddMultiresolutionType<WsfSensor>();
   // Signatures
   AddMultiresolutionType<WsfAcousticSignature>();
   AddMultiresolutionType<WsfInfraredSignature>();
   AddMultiresolutionType<WsfOpticalSignature>();
   AddMultiresolutionType<WsfRadarSignature>();
}
