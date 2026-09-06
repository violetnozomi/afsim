// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSA_Module.hpp"

WsfSA_Module::WsfSA_Module(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr)
   : mSA_ProcessorPtr(aSA_ProcPtr)
   , mDataPtr(aDataPtr)
{
}

WsfSA_Module::WsfSA_Module(const WsfSA_Module& aSrc)
   : mSA_ProcessorPtr(aSrc.mSA_ProcessorPtr)
   , mDataPtr(aSrc.mDataPtr)
{
}

void WsfSA_Module::Update(double aSimTime)
{
   // Many modules will override this
}

void WsfSA_Module::SetSAP(WsfSA_Processor* aSA_ProcPtr, WsfSA_Processor::PerceiveData* aDataPtr)
{
   mSA_ProcessorPtr = aSA_ProcPtr;
   mDataPtr         = aDataPtr;
}

WsfPlatform* WsfSA_Module::GetPlatform() const
{
   return mSA_ProcessorPtr->GetPlatform();
}

WsfScriptContext& WsfSA_Module::GetScriptContext() const
{
   return mSA_ProcessorPtr->GetScriptContext();
}

WsfSimulation* WsfSA_Module::GetSimulation() const
{
   return mSA_ProcessorPtr->GetSimulation();
}
