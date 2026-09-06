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

#include "WsfQuantumGenerator.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptDataPack.hpp"
#include "UtScriptExecutor.hpp"
#include "UtStringUtil.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfQuantumTask.hpp"
#include "WsfTrack.hpp"
#include "WsfWeaponTaskResource.hpp"
#include "script/WsfScriptContext.hpp"


WsfQuantumGeneratorFactory::WsfQuantumGeneratorFactory()
{
   Register(ut::make_unique<WsfQuantumGeneratorSimpleWeapon>());
   Register(ut::make_unique<WsfQuantumGeneratorSimpleJammer>());
   Register(ut::make_unique<WsfQuantumGeneratorSimpleSensor>());
}

// static
WsfQuantumGeneratorFactory* WsfQuantumGeneratorFactory::Instance()
{
   static WsfQuantumGeneratorFactory instance;
   return &instance;
}

bool WsfQuantumGeneratorFactory::Register(std::unique_ptr<WsfQuantumGenerator> aGeneratorPtr)
{
   if (aGeneratorPtr != nullptr)
   {
      return mFactoryMap.emplace(aGeneratorPtr->GeneratorType(), std::move(aGeneratorPtr)).second;
   }
   return false;
}

WsfQuantumGenerator* WsfQuantumGeneratorFactory::CreateGenerator(const std::string& generatorName)
{
   auto it = mFactoryMap.find(generatorName);
   if (it != mFactoryMap.end())
   {
      return it->second->Clone();
   }
   return nullptr;
}

WsfQuantumGeneratorScript::WsfQuantumGeneratorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod)
   : WsfQuantumGenerator()
   , mType(aScriptMethod)
   , mScriptPtr(nullptr)
   , mContextPtr(&aContextPtr->GetContext())
   , mExecutorPtr(aContextPtr->GetExecutor())
{
   mScriptPtr = mContextPtr->FindScript(mType);
   while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
   {
      mContextPtr = mContextPtr->GetParent();
      mScriptPtr  = mContextPtr->FindScript(mType);
   }

   if (mScriptPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find script for quantum generator.";
      out.AddNote() << "Type: " << mType;
   }
   else if (!aContextPtr->ValidateScript(mScriptPtr, "Array<WsfQuantumTask>", "Array<WsfLocalTrack>, Array<WsfAssetPerception>"))
   {
      auto out = ut::log::error() << "Invalid inputs or outputs for script as quantum generator.";
      out.AddNote() << "Type: " << mType;
      mScriptPtr = nullptr;
   }
}

// virtual
void WsfQuantumGeneratorScript::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   if (aParentContextPtr != nullptr)
   {
      mContextPtr  = &aParentContextPtr->GetContext();
      mExecutorPtr = aParentContextPtr->GetExecutor();

      mScriptPtr = mContextPtr->FindScript(mType);
      while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
      {
         mContextPtr = mContextPtr->GetParent();
         mScriptPtr  = mContextPtr->FindScript(mType);
      }
      if (mScriptPtr != nullptr)
      {
         mContextPtr->GetTypes()->GetOrCreateContainer("Array<WsfTask>");
         mAssetsClassPtr     = mContextPtr->GetTypes()->GetOrCreateContainer("Array<WsfAssetPerception>");
         mTracksClassPtr     = mContextPtr->GetTypes()->GetOrCreateContainer("Array<WsfLocalTrack>");
         mLocalTrackClassPtr = mContextPtr->GetTypes()->GetClass("WsfLocalTrack");
         mAssetClassPtr      = mContextPtr->GetTypes()->GetClass("WsfAssetPerception");
      }
   }
}

// virtual
WsfQuantumGenerator* WsfQuantumGeneratorScript::Clone() const
{
   return dynamic_cast<WsfQuantumGenerator*>(new WsfQuantumGeneratorScript(*this));
}

// virtual
std::vector<WsfQuantumTask*> WsfQuantumGeneratorScript::GenerateTasks(double                            aSimTime,
                                                                      std::vector<WsfLocalTrack*>&      aThreats,
                                                                      std::vector<WsfAssetPerception*>& aAssets)
{
   std::vector<WsfQuantumTask*> tasks;
   if (mScriptPtr != nullptr)
   {
      // script Array<WsfTask> methodName ( Array<WsfLocalTrack>, Array<WsfAssetPerception> )
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      // fill in the first argument (Array<WsfLocalTrack>) for the script method
      auto* arrayPtr = new std::vector<UtScriptData>();
      for (auto& threat : aThreats)
      {
         arrayPtr->push_back(UtScriptData(new UtScriptRef(threat, mLocalTrackClassPtr)));
      }
      scriptArgs.push_back(UtScriptData(new UtScriptRef(arrayPtr, mTracksClassPtr, UtScriptRef::cMANAGE)));

      // fill in the second argument (Array<WsfAssetPerception>) for the script method
      auto* array2Ptr = new std::vector<UtScriptData>();
      for (auto& asset : aAssets)
      {
         array2Ptr->push_back(UtScriptData(new UtScriptRef(asset, mAssetClassPtr)));
      }
      scriptArgs.push_back(UtScriptData(new UtScriptRef(array2Ptr, mAssetsClassPtr, UtScriptRef::cMANAGE)));

      // execute the generator script method
      mContextPtr->Execute(mExecutorPtr, mScriptPtr, scriptRetVal, scriptArgs);

      // read the WsfQuantumTask objects out of the returned type
      std::vector<UtScriptData>* tasksPtr =
         static_cast<std::vector<UtScriptData>*>(scriptRetVal.GetPointer()->GetAppObject());
      auto it = tasksPtr->begin();
      while (it != tasksPtr->end())
      {
         WsfQuantumTask* taskPtr = static_cast<WsfQuantumTask*>((*it).GetPointer()->GetAppObject())->Clone();
         tasks.push_back(taskPtr);
         ++it;
      }
   }
   return tasks;
}


WsfQuantumGeneratorScript::WsfQuantumGeneratorScript(const WsfQuantumGeneratorScript& aSrc)
   : mType(aSrc.mType)
   , mContextPtr(aSrc.mContextPtr)
   , mExecutorPtr(aSrc.mExecutorPtr)
{
   mScriptPtr = mContextPtr->FindScript(mType);
   while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
   {
      mContextPtr = mContextPtr->GetParent();
      mScriptPtr  = mContextPtr->FindScript(mType);
   }
}

// virtual
std::vector<WsfQuantumTask*> WsfQuantumGeneratorSimpleWeapon::GenerateTasks(double                            aSimTime,
                                                                            std::vector<WsfLocalTrack*>&      aThreats,
                                                                            std::vector<WsfAssetPerception*>& aAssets)
{
   std::vector<WsfQuantumTask*> tasks;
   for (auto& threat : aThreats)
   {
      WsfQuantumTask* tPtr = new WsfQuantumTask(1.0, WsfWeaponTaskResource(), dynamic_cast<WsfTrack*>(threat));
      tPtr->SetTaskType("WEAPON");
      tasks.push_back(tPtr); // target this enemy?
   }
   return tasks;
}

// virtual
std::vector<WsfQuantumTask*> WsfQuantumGeneratorSimpleJammer::GenerateTasks(double                            aSimTime,
                                                                            std::vector<WsfLocalTrack*>&      aThreats,
                                                                            std::vector<WsfAssetPerception*>& aAssets)
{
   std::vector<WsfQuantumTask*> tasks;
   for (auto& threat : aThreats)
   {
      WsfQuantumTask* tPtr = new WsfQuantumTask(1.0, WsfJammerTaskResource(), dynamic_cast<WsfTrack*>(threat));
      tPtr->SetTaskType("JAMMER");
      tasks.push_back(tPtr); // jam this enemy?
   }
   return tasks;
}

// virtual
std::vector<WsfQuantumTask*> WsfQuantumGeneratorSimpleSensor::GenerateTasks(double                            aSimTime,
                                                                            std::vector<WsfLocalTrack*>&      aThreats,
                                                                            std::vector<WsfAssetPerception*>& aAssets)
{
   std::vector<WsfQuantumTask*> tasks;
   for (auto& threat : aThreats)
   {
      WsfQuantumTask* tPtr =
         new WsfQuantumTask(1.0, WsfTaskResource(cTASK_RESOURCE_TYPE_SENSOR), dynamic_cast<WsfTrack*>(threat));
      tPtr->SetTaskType("SENSOR");
      tasks.push_back(tPtr); // track this enemy?
   }
   return tasks;
}
