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

#include "WsfQuantumAllocator.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptExecutor.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfQuantumTask.hpp"
#include "script/WsfScriptContext.hpp"

// virtual
WsfQuantumTask* WsfQuantumAllocator::AllocationFor(WsfAssetPerception* aAssetPtr)
{
   auto it = mAssetAllocationMap.find(aAssetPtr);
   if (it != mAssetAllocationMap.end())
   {
      return it->second;
   }
   return nullptr;
}

// virtual
WsfAssetPerception* WsfQuantumAllocator::AllocatedTo(WsfQuantumTask* aTaskPtr)
{
   auto it = mTaskAllocationMap.find(aTaskPtr);
   if (it != mTaskAllocationMap.end())
   {
      return it->second;
   }
   return nullptr;
}

WsfQuantumAllocatorFactory::WsfQuantumAllocatorFactory()
{
   Register(ut::make_unique<WsfQuantumAllocatorSimple>());
   Register(ut::make_unique<WsfQuantumAllocatorGreedyIsolated>());
   Register(ut::make_unique<WsfQuantumAllocatorGreedyPriority>());
   Register(ut::make_unique<WsfQuantumAllocatorGreedyValue>());
   Register(ut::make_unique<WsfQuantumAllocatorGreedyProfit>());
   Register(ut::make_unique<WsfQuantumAllocatorOptimalProfit>());
}

// static
WsfQuantumAllocatorFactory* WsfQuantumAllocatorFactory::Instance()
{
   static WsfQuantumAllocatorFactory instance;
   return &instance;
}

bool WsfQuantumAllocatorFactory::Register(std::unique_ptr<WsfQuantumAllocator> aAllocatorPtr)
{
   if (aAllocatorPtr != nullptr)
   {
      return mFactoryMap.emplace(aAllocatorPtr->AllocatorType(), std::move(aAllocatorPtr)).second;
   }
   return false;
}

WsfQuantumAllocator* WsfQuantumAllocatorFactory::CreateAllocator(const std::string& allocatorName)
{
   auto it = mFactoryMap.find(allocatorName);
   if (it != mFactoryMap.end())
   {
      return it->second->Clone();
   }
   return nullptr;
}

WsfQuantumAllocatorScript::WsfQuantumAllocatorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod)
   : WsfQuantumAllocator()
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
      auto out = ut::log::error() << "Unable to find script for quantum allocator.";
      out.AddNote() << "Type: " << mType;
   }
   else if (!aContextPtr->ValidateScript(mScriptPtr,
                                         "Map<WsfAssetPerception,WsfQuantumTask>",
                                         "Array<Array<double>>, Array<WsfAssetPerception>, Array<WsfQuantumTask>"))
   {
      auto out = ut::log::error() << "Invalid inputs or outputs for script as quantum allocator.";
      out.AddNote() << "Type: " << mType;
      mScriptPtr = nullptr;
   }
}

// virtual
void WsfQuantumAllocatorScript::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   if (aParentContextPtr != nullptr)
   {
      mContextPtr  = &aParentContextPtr->GetContext();
      mExecutorPtr = aParentContextPtr->GetExecutor();
      mScriptPtr   = mContextPtr->FindScript(mType);
      while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
      {
         mContextPtr = mContextPtr->GetParent();
         mScriptPtr  = mContextPtr->FindScript(mType);
      }
      if (mScriptPtr != nullptr)
      {
         // Map<WsfAssetPerception, WsfQuantumTask> allocator_name ( Array<Array<double>> , Array<WsfAssetPerception> ,
         // Array<WsfQuantumTask> )
         mContextPtr->GetTypes()->GetOrCreateContainer("Array<double>");
         mContextPtr->GetTypes()->GetOrCreateContainer("Array<Array<double>>");
         mContextPtr->GetTypes()->GetOrCreateContainer("Array<WsfAssetPerception>");
         mContextPtr->GetTypes()->GetOrCreateContainer("Array<WsfQuantumTask>");
      }
   }
}

// virtual
AllocationList WsfQuantumAllocatorScript::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   if (mScriptPtr != nullptr)
   {
      UtScriptData       scriptRetVal;
      UtScriptDataPacker scriptArgs{mContextPtr->GetTypes()};

      // fill in the first argument, array<array<double>>  values
      auto           array2dPtr    = ut::make_unique<std::vector<UtScriptData>>();
      UtScriptClass* arrayClassPtr = mContextPtr->GetTypes()->GetClass("Array<double>");
      for (const auto& values : aMatrix.Values())
      {
         auto arrayPtr = ut::make_unique<std::vector<UtScriptData>>(values.begin(), values.end());
         array2dPtr->emplace_back(new UtScriptRef(arrayPtr.release(), arrayClassPtr, UtScriptRef::cMANAGE));
      }
      scriptArgs.Push(array2dPtr.release(), "Array<Array<double>>", UtScriptRef::cMANAGE);

      // fill in the second argument (Array<WsfAssetPerception>) for the script method
      UtScriptClass* assetClassPtr = mContextPtr->GetTypes()->GetClass("WsfAssetPerception"); // local track? or array?
      auto           array2Ptr     = ut::make_unique<std::vector<UtScriptData>>();
      for (auto assetPtr : aMatrix.Assets())
      {
         array2Ptr->emplace_back(new UtScriptRef(assetPtr, assetClassPtr));
      }
      scriptArgs.Push(array2Ptr.release(), "Array<WsfAssetPerception>", UtScriptRef::cMANAGE);

      // fill in the third argument (Array<WsfTask>) for the script method
      UtScriptClass* taskClassPtr = mContextPtr->GetTypes()->GetClass("WsfQuantumTask");
      auto           array3Ptr    = ut::make_unique<std::vector<UtScriptData>>();
      for (auto taskPtr : aMatrix.Tasks())
      {
         array3Ptr->emplace_back(new UtScriptRef(taskPtr, taskClassPtr));
      }
      scriptArgs.Push(array3Ptr.release(), "Array<WsfQuantumTask>", UtScriptRef::cMANAGE);

      // execute the allocator script method
      mContextPtr->Execute(mExecutorPtr, mScriptPtr, scriptRetVal, scriptArgs);

      // read the Map objects out of the returned type
      auto allocationsPtr = scriptRetVal.GetPointer()->GetAppObject<std::map<UtScriptData, UtScriptData>>();
      for (const auto& allocation : *allocationsPtr)
      {
         auto assetPtr = allocation.first.GetPointer()->GetAppObject<WsfAssetPerception>();
         auto taskPtr  = allocation.second.GetPointer()->GetAppObject<WsfQuantumTask>();
         allocations.emplace_back(assetPtr, ut::clone(taskPtr));
      }
   }
   return allocations;
}

WsfQuantumAllocatorScript::WsfQuantumAllocatorScript(const WsfQuantumAllocatorScript& aSrc)
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
AllocationList WsfQuantumAllocatorSimple::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   std::vector<WsfAssetPerception*>& aAssets = aMatrix.Assets();
   std::vector<WsfQuantumTask*>&     aTasks  = aMatrix.Tasks();

   for (size_t i = 0; i < aAssets.size() && i < aTasks.size(); ++i)
   {
      allocations.emplace_back(aAssets[i], ut::clone(aTasks[i]));
      mAssetAllocationMap[aAssets[i]] = aTasks[i];
      mTaskAllocationMap[aTasks[i]]   = aAssets[i];
   }
   return allocations;
}

// virtual
AllocationList WsfQuantumAllocatorGreedyIsolated::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   std::vector<WsfAssetPerception*>& aAssets  = aMatrix.Assets();
   std::vector<WsfQuantumTask*>&     aTasks   = aMatrix.Tasks();
   std::vector<std::vector<double>>& aProfits = aMatrix.Profit();

   for (size_t j = 0; j < aAssets.size(); ++j)
   {
      double maxProfit     = 0.0;
      size_t bestTaskIndex = 0;
      bool   foundTask     = false;
      for (size_t i = 0; i < aTasks.size(); ++i)
      {
         // profit = value * priority
         if (aProfits[i][j] > maxProfit)
         {
            maxProfit     = aProfits[i][j];
            bestTaskIndex = i;
            foundTask     = true;
         }
      }
      if (foundTask)
      {
         allocations.emplace_back(aAssets[j], ut::clone(aTasks[bestTaskIndex]));
         mAssetAllocationMap[aAssets[j]]           = aTasks[bestTaskIndex];
         mTaskAllocationMap[aTasks[bestTaskIndex]] = aAssets[j];
      }
   }

   return allocations;
}

struct MyPair
{
   MyPair(double v, int k)
      : val(v)
      , key(k)
   {
   }

   double      val;
   int         key;
   inline bool operator<(const MyPair& aRhs) const { return val < aRhs.val; }
};

// virtual
AllocationList WsfQuantumAllocatorGreedyPriority::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   std::vector<WsfAssetPerception*> assets = std::vector<WsfAssetPerception*>(aMatrix.Assets());
   std::vector<WsfQuantumTask*>&    aTasks = aMatrix.Tasks();

   // make our own copy (because we remove columns during assignment below)
   std::vector<std::vector<double>> values(aMatrix.Values());

   // value is how well the asset can perform said task
   std::vector<MyPair> sortedTasks;
   for (size_t i = 0; i < aTasks.size(); ++i)
   {
      sortedTasks.emplace_back(aTasks[i]->GetPriority(), (int)i);
   }
   std::sort(sortedTasks.begin(), sortedTasks.end());
   // now highest priority task is at the back
   size_t sts = sortedTasks.size();
   for (size_t i = 0; i < sts; ++i)
   {
      auto taskIndex = sortedTasks[(sts - 1U) - i].key;
      if (!values[taskIndex].empty())
      {
         double maxValue       = 0.0;
         size_t bestAssetIndex = 0;
         bool   foundTask      = false;
         for (size_t j = 0; j < values[taskIndex].size(); ++j)
         {
            if (values[taskIndex][j] > maxValue)
            {
               maxValue       = values[taskIndex][j];
               bestAssetIndex = j;
               foundTask      = true;
            }
         }
         if (foundTask)
         {
            allocations.emplace_back(assets[bestAssetIndex], ut::clone(aTasks[taskIndex]));
            mAssetAllocationMap[assets[bestAssetIndex]] = aTasks[taskIndex];
            mTaskAllocationMap[aTasks[taskIndex]]       = assets[bestAssetIndex];
            // now remove this asset from consideration of other tasks
            WsfQuantumMatrix::RemoveColumn(values, bestAssetIndex); // should reduce the size of the "values" matrix
            assets.erase(assets.begin() + bestAssetIndex);
         }
      }
   }
   return allocations;
}

AllocationList WsfQuantumAllocatorGreedyValue::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   // make our own copies, because we remove entries during processing below
   std::vector<WsfAssetPerception*> assets = std::vector<WsfAssetPerception*>(aMatrix.Assets());
   std::vector<WsfQuantumTask*>     tasks(aMatrix.Tasks());
   std::vector<std::vector<double>> values(aMatrix.Values()); // value is how well the asset can perform said task

   // brute force search for now
   while ((!values.empty()) && (!values[0].empty()))
   {
      double maxValue          = 0.0;
      double tieBreakPriority  = 0.0;
      size_t bestTaskIndex     = 0;
      size_t bestAssetIndex    = 0;
      bool   foundTaskandAsset = false;

      for (size_t i = 0; i < values.size(); ++i)
      {
         for (size_t j = 0; j < values[0].size(); ++j)
         {
            if ((values[i][j] > maxValue) ||
                (maxValue > 0 && values[i][j] == maxValue && tasks[i]->GetPriority() > tieBreakPriority))
            {
               maxValue          = values[i][j];
               tieBreakPriority  = tasks[i]->GetPriority();
               bestTaskIndex     = i;
               bestAssetIndex    = j;
               foundTaskandAsset = true;
            }
         }
      }

      if (foundTaskandAsset)
      {
         allocations.emplace_back(assets[bestAssetIndex], ut::clone(tasks[bestTaskIndex]));
         mAssetAllocationMap[assets[bestAssetIndex]] = tasks[bestTaskIndex];
         mTaskAllocationMap[tasks[bestTaskIndex]]    = assets[bestAssetIndex];

         // now remove this asset from consideration of other tasks
         WsfQuantumMatrix::RemoveColumn(values, bestAssetIndex); // should reduce the size of the "values" matrix
         assets.erase(assets.begin() + bestAssetIndex);
         // now remove this task from consideration by other assets
         WsfQuantumMatrix::RemoveRow(values, bestTaskIndex); // should reduce the size of the "values" matrix
         tasks.erase(tasks.begin() + bestTaskIndex);
      }
      else
      {
         // done with the search, will not find a positive value any more
         break;
      }
   }
   return allocations;
}

AllocationList WsfQuantumAllocatorGreedyProfit::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   // make our own copies, because we remove entries during processing below
   std::vector<WsfAssetPerception*> assets = std::vector<WsfAssetPerception*>(aMatrix.Assets());
   std::vector<WsfQuantumTask*>     tasks(aMatrix.Tasks());
   std::vector<std::vector<double>> profit(aMatrix.Profit());

   // brute force search for now
   while ((!profit.empty()) && (!profit[0].empty()))
   {
      double maxProfit         = 0.0;
      size_t bestTaskIndex     = 0;
      size_t bestAssetIndex    = 0;
      bool   foundTaskandAsset = false;

      for (size_t i = 0; i < profit.size(); ++i)
      {
         for (size_t j = 0; j < profit[0].size(); ++j)
         {
            if (profit[i][j] > maxProfit)
            {
               maxProfit         = profit[i][j];
               bestTaskIndex     = i;
               bestAssetIndex    = j;
               foundTaskandAsset = true;
            }
         }
      }

      if (foundTaskandAsset)
      {
         allocations.emplace_back(assets[bestAssetIndex], ut::clone(tasks[bestTaskIndex]));
         mAssetAllocationMap[assets[bestAssetIndex]] = tasks[bestTaskIndex];
         mTaskAllocationMap[tasks[bestTaskIndex]]    = assets[bestAssetIndex];

         // now remove this asset from consideration of other tasks
         WsfQuantumMatrix::RemoveColumn(profit, bestAssetIndex); // should reduce the size of the "profit" matrix
         assets.erase(assets.begin() + bestAssetIndex);
         // now remove this task from consideration by other assets
         WsfQuantumMatrix::RemoveRow(profit, bestTaskIndex); // should reduce the size of the "profit" matrix
         tasks.erase(tasks.begin() + bestTaskIndex);
      }
      else
      {
         // done with the search, will not find a positive profit any more
         break;
      }
   }
   return allocations;
}

//! this allocator finds the optimal profit allocation of tasks to assets
//! some tasks might be left unassigned, some assets might be left without tasks
AllocationList WsfQuantumAllocatorOptimalProfit::MakeAllocations(WsfQuantumMatrix& aMatrix)
{
   AllocationList allocations;
   mAssetAllocationMap.clear();
   mTaskAllocationMap.clear();

   std::vector<WsfAssetPerception*>& aAssets = aMatrix.Assets();
   std::vector<WsfQuantumTask*>&     aTasks  = aMatrix.Tasks();

   if (aAssets.empty() || aTasks.empty())
   {
      return allocations;
   }

   // iterate over returned assignments (stores indices) & populate: mAllocations
   std::vector<int> assignments = mOptimalAssigner.AssignMaxProfit(aMatrix.Profit());
   for (size_t taskIndex = 0; taskIndex < assignments.size(); ++taskIndex)
   {
      int assetIndex = assignments[taskIndex];
      if (assetIndex >= 0) // verify indices, perhaps nobody can do this task
      {
         // verify the profit was non-zero before assigning (optimal assignment algorithm can make zero value
         // assignments, assumes every asset gets a task)
         if (aMatrix.Profit(taskIndex, assetIndex) > 0)
         {
            allocations.emplace_back(aAssets[assetIndex], ut::clone(aTasks[taskIndex]));
         }
      }
   }

   return allocations;
}
