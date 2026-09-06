// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfUCI_InterfaceExtension.hpp"

#include "WsfScenario.hpp"
#include "WsfScenarioExtension.hpp"
#include "WsfUCI.hpp"

namespace wsf
{

void wsf::UCI_InterfaceExtension::AddedToSimulation()
{
   mInterfacePtr = static_cast<OMS_UCI_Extension&>(GetScenario().GetExtension(OMS_UCI_Extension::GetExtensionName())).GetInterface();
}

const UCI_InterfaceExtension* UCI_InterfaceExtension::Get(WsfSimulation& aSimulation)
{
   return static_cast<UCI_InterfaceExtension*>(aSimulation.FindExtension(GetExtensionName()));
}

//! Return the first interface in the map.
UCI_Interface* UCI_InterfaceExtension::GetInterface() const
{
   return mInterfacePtr.get();
}

} // wsf
