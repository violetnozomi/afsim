// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptOrbitalConjunctionProcessor.hpp"

#include "WsfOrbitalConjunctionProcessor.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptOrbitalConjunctionProcessor::WsfScriptOrbitalConjunctionProcessor(const std::string& aClassName,
                                                                           UtScriptTypes*     aTypesPtr)
   : WsfScriptProcessorClass(aClassName, aTypesPtr)
{
   SetClassName("WsfOrbitalConjunctionProcessor");
}

void* WsfScriptOrbitalConjunctionProcessor::Create(const UtScriptContext& aInstance)
{
   WsfScenario* sPtr = WsfScriptContext::GetSCENARIO(aInstance);
   return static_cast<void*>(new WsfOrbitalConjunctionProcessor(*sPtr));
}

void WsfScriptOrbitalConjunctionProcessor::Destroy(void* aObjectPtr)
{
   WsfOrbitalConjunctionProcessor* aProcPtr = static_cast<WsfOrbitalConjunctionProcessor*>(aObjectPtr);
   delete aProcPtr;
}

void* WsfScriptOrbitalConjunctionProcessor::Clone(void* aObjectPtr)
{
   return static_cast<WsfOrbitalConjunctionProcessor*>(aObjectPtr)->Clone();
}
