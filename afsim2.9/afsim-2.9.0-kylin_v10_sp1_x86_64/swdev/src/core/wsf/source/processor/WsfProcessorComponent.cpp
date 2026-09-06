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

#include "WsfProcessorComponent.hpp"

#include "WsfProcessor.hpp"

// =================================================================================================
WsfPlatform* WsfProcessorComponent::GetPlatform() const
{
   return GetComponentParent()->GetPlatform();
}

// =================================================================================================
WsfSimulation* WsfProcessorComponent::GetSimulation() const
{
   return GetComponentParent()->GetSimulation();
}
