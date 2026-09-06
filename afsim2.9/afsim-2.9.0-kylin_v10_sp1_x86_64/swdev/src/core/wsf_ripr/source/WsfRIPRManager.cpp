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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

//***************************************************************************
//* FILE NAME:
//*   WsfRIPRManager.cpp
//*
//* PURPOSE:
//*   The WsfRIPRManager class holds all the RIPR processor IDs.
//*   Only one instance of this class is needed at a time, so it is
//*   implemented as a singleton for the time being.
//*
//* NOTES:
//*   None
//***************************************************************************

#include "WsfRIPRManager.hpp"

#include <algorithm>
#include <memory>

#ifndef SIM_VTAB
#include "WsfRIPRProcessor.hpp"
#include "WsfScriptRIPRProcessorClass.hpp"
#endif

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfEventOutput.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessorTypes.hpp"
#include "WsfRIPRJob.hpp"
#include "WsfRIPRObserver.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "script/WsfScriptClusterManagerClass.hpp"
#include "script/WsfScriptManager.hpp"

namespace
{
class ApplicationExtension : public WsfApplicationExtension
{
public:
   void ScenarioCreated(WsfScenario& aScenario) override
   {
      aScenario.RegisterExtension(GetExtensionName(), ut::make_unique<WsfRIPR_ModuleSetup>(aScenario));
   }
};
} // namespace

void RIPR_EXPORT Register_wsf_ripr(WsfApplication& aApplication)
{
   if (!aApplication.ExtensionIsRegistered("wsf_ripr"))
   {
      aApplication.RegisterFeature("ripr", "wsf_ripr");
      UtScriptTypes* aTypesPtr = aApplication.GetScriptTypes();
      aTypesPtr->Register(ut::make_unique<WsfScriptRIPRProcessorClass>("WsfRIPRProcessor", aTypesPtr));
      aTypesPtr->Register(ut::make_unique<WsfScriptRIPRJobClass>("WsfRIPRJob", aTypesPtr));
      aTypesPtr->GetOrCreateContainer("Map<string,Object>");

      aApplication.RegisterExtension("wsf_ripr", ut::make_unique<ApplicationExtension>());
   }
}

WsfRIPRManagerData::WsfRIPRManagerData(WsfScenario& aScenario)
{
   mScenarioPtr = &aScenario;
}

WsfRIPRManager::WsfRIPRManager(const WsfRIPRManagerData& aData)
   : mNextUniqueId(0)
   , mShare(&aData)
   , mRIPR_XIOPtr(nullptr)
   , mEventOutputPtr(nullptr)
   , mRiprTypeList()
{
   mObserverPtr = new WsfRIPRObserver;

   // store the list of RIPR Processor base and derived types.
   mRiprTypeList.push_back(WsfStringId("WSF_RIPR_PROCESSOR"));

   // Sort for faster searching.  The sort is by order of the WsfStringId,
   // not the alpha-numeric string.
   sort(mRiprTypeList.begin(), mRiprTypeList.end());

   //// if XIO has already been initialized, and RIPR XIO has not been initialized,
   //// initialize it now.
   //// if XIO has not been initialized, register the InitializeXIO() function with
   //// XIO's post-initialization callback.  This addresses a problem in which Monte
   //// Carlo runs were causing RIPR XIO to be re-initialized every iteration.
   // if (WsfXIO::IsInitialized())
   //{
   //    InitializeXIO();
   // }
   // else // XIO not initialized.  Postpone RIPR XIO initialization until it happens
   //{
   //    mCallbacks.Add(WsfXIO::XIO_InitializedClbk.Connect(&WsfRIPRManager::InitializeXIO,this));
   // }
}

bool WsfRIPRManager::IsRiprType(WsfStringId aType) const
{
   return binary_search(mRiprTypeList.begin(), mRiprTypeList.end(), aType);
}


// virtual
WsfRIPRManager::~WsfRIPRManager()
{
   delete mObserverPtr;
}

unsigned int WsfRIPRManager::GetUniqueRiprId(WsfRIPRProcessor* aProcPtr)
{
   // looks like the unique id's will start at one;
   mIdToProcMap[++mNextUniqueId] = SafeRIPRProc(aProcPtr);
   GetObserver().RIPRManagerChanged();
   return mNextUniqueId;
}

WsfRIPRProcessor* WsfRIPRManager::GetRiprProcById(unsigned int aId)
{
   std::map<unsigned int, SafeRIPRProc>::iterator it;
   it = mIdToProcMap.find(aId);
   if (it != mIdToProcMap.end())
   {
      if (it->second.get() == nullptr)
      {
         //         mIdToProcMap[aId] = 0;
         mIdToProcMap.erase(it);
         return nullptr;
      }
      return it->second.get();
   }
   return nullptr;
}

void WsfRIPRManager::RemoveRiprProc(unsigned int aId)
{
   std::map<unsigned int, SafeRIPRProc>::iterator it;
   it = mIdToProcMap.find(aId);
   if (it != mIdToProcMap.end())
   {
      //      mIdToProcMap[aId] = 0;
      mIdToProcMap.erase(it);
   }
}

//! Initialize the XIO extension that manages network requests for RIPR information.
// it and return 'true' and set 'aCommandOk' to 'true' if it was
// valid or 'false' if not. If it isn't one of mine then return 'false'.
// virtual
bool WsfRIPRManagerData::ProcessRIPR_Input(UtInput& aInput)
{
   bool myCommand = false;

   return myCommand;
}

WsfRIPRManagerData::~WsfRIPRManagerData() {}

WsfRIPR_ModuleSetup::~WsfRIPR_ModuleSetup()
{
   WsfRIPRJob::Reset();                   // resets job IDs
   WsfRIPRJobBoard::DependencyClearAll(); // reset static job dependencies
}

WsfRIPR_ModuleSetup::WsfRIPR_ModuleSetup(WsfScenario& aScenario)
   : WsfRIPRManagerData(aScenario)
{
   WsfProcessorTypes::Get(aScenario).Add("WSF_RIPR_PROCESSOR", ut::make_unique<WsfRIPRProcessor>(aScenario));
}

void WsfRIPR_ModuleSetup::SimulationCreated(WsfSimulation& aSimulation)
{
   aSimulation.RegisterExtension(GetExtensionName(), ut::make_unique<WsfRIPRManager>(*this));
}

bool WsfRIPR_ModuleSetup::ProcessInput(UtInput& aInput)
{
   return ProcessRIPR_Input(aInput);
}
