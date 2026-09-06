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

#ifndef WSFQUANTUMALLOCATOR_HPP
#define WSFQUANTUMALLOCATOR_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtCloneablePtr.hpp"
#include "UtMemory.hpp"
#include "UtOptimalAssignment.hpp"
class UtScript;
class UtScriptContext;
class UtScriptExecutor;
class WsfAssetPerception;
#include "WsfQuantumMatrix.hpp"
class WsfQuantumTask;
class WsfScriptContext;

using AllocationList = std::vector<std::pair<WsfAssetPerception*, ut::CloneablePtr<WsfQuantumTask>>>;


class WSF_MIL_EXPORT WsfQuantumAllocator
{
public:
   // implement different types of greedy allocation & optimal profit allocation (simplex/Hungarian algorithm)
   // enum AllocationType
   //{
   //    GREEDY_PROFIT,             //highest profit awarded first (profit = value * priority)
   //    OPTIMAL_PROFIT,            //optimal profit assignment algorithm (Hungarian algorithm using hopkroft-karp method:
   //    O(N^3) complexity) GREEDY_VALUE,              //highest value is awarded first (priority used for tie breaks)
   //    GREEDY_PRIORITY,           //highest priority is awarded first (value used for tie breaks)
   //    GREEDY_ISOLATED,           //all assets awarded the task they valued highest (stacks up on tasks)
   //    IDLE                       //assets remain unassigned, left idle (useful for secondary allocation type)
   // };

   WsfQuantumAllocator()                = default;
   virtual ~WsfQuantumAllocator()       = default;
   WsfQuantumAllocator&         operator=(const WsfQuantumAllocator&) = default;
   virtual std::string          AllocatorType()                       = 0; // allocators must define this
   virtual WsfQuantumAllocator* Clone()                               = 0;
   virtual void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr){};
   // allocators must define this
   virtual AllocationList MakeAllocations(WsfQuantumMatrix& aMatrix) = 0;

   virtual WsfQuantumTask*     AllocationFor(WsfAssetPerception* aAssetPtr);
   virtual WsfAssetPerception* AllocatedTo(WsfQuantumTask* aTaskPtr);

   std::string TaskType() { return mTaskType; }
   bool        HasTaskType() { return !mTaskType.empty(); }
   void        SetTaskType(std::string& aType) { mTaskType = aType; }

protected:
   std::string                                    mTaskType;
   std::map<WsfAssetPerception*, WsfQuantumTask*> mAssetAllocationMap;
   std::map<WsfQuantumTask*, WsfAssetPerception*> mTaskAllocationMap;
};

class WSF_MIL_EXPORT WsfQuantumAllocatorFactory
{
public:
   ~WsfQuantumAllocatorFactory()                                 = default;
   WsfQuantumAllocatorFactory(WsfQuantumAllocatorFactory&& aSrc) = delete;
   WsfQuantumAllocatorFactory& operator=(const WsfQuantumAllocatorFactory&) = delete;
   WsfQuantumAllocatorFactory& operator=(WsfQuantumAllocatorFactory&&) = delete;

   static WsfQuantumAllocatorFactory* Instance();
   bool                               Register(std::unique_ptr<WsfQuantumAllocator> aAllocatorPtr);
   WsfQuantumAllocator*               CreateAllocator(const std::string& allocatorName);
   const std::map<std::string, ut::CloneablePtr<WsfQuantumAllocator>>& Allocators() const { return mFactoryMap; }

private:
   WsfQuantumAllocatorFactory();
   WsfQuantumAllocatorFactory(const WsfQuantumAllocatorFactory&) = default;

   std::map<std::string, ut::CloneablePtr<WsfQuantumAllocator>> mFactoryMap;
};

class WSF_MIL_EXPORT WsfQuantumAllocatorScript : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod);
   ~WsfQuantumAllocatorScript() override = default;
   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override;
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorScript(*this); }
   std::string          AllocatorType() override { return mType; }
   virtual bool         Valid() { return (mScriptPtr != nullptr); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;

private:
   WsfQuantumAllocatorScript(const WsfQuantumAllocatorScript& aSrc);
   WsfQuantumAllocatorScript& operator=(const WsfQuantumAllocatorScript&) = delete;

   std::string       mType;
   UtScript*         mScriptPtr;
   UtScriptContext*  mContextPtr;
   UtScriptExecutor* mExecutorPtr;
};


class WSF_MIL_EXPORT WsfQuantumAllocatorSimple : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorSimple()           = default;
   ~WsfQuantumAllocatorSimple() override = default;
   std::string          AllocatorType() override { return "simple"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorSimple(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;
};

// this allocator assigns the highest profit task for each asset, regardless of other assignments
// this could result in several assets being assigned the same task
class WSF_MIL_EXPORT WsfQuantumAllocatorGreedyIsolated : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorGreedyIsolated()           = default;
   ~WsfQuantumAllocatorGreedyIsolated() override = default;
   std::string          AllocatorType() override { return "greedy_isolated"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorGreedyIsolated(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;
};

// this allocator starts assigning assets to the highest priority tasks first
// it assigns the highest valued asset (that still remains unassigned) to each task
// some tasks might be left unassigned
class WSF_MIL_EXPORT WsfQuantumAllocatorGreedyPriority : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorGreedyPriority()           = default;
   ~WsfQuantumAllocatorGreedyPriority() override = default;
   std::string          AllocatorType() override { return "greedy_priority"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorGreedyPriority(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;
};

// this allocator starts assigning tasks to the highest value assets first (task priority only used as a tie-breaker)
// some tasks might be left unassigned
class WSF_MIL_EXPORT WsfQuantumAllocatorGreedyValue : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorGreedyValue()           = default;
   ~WsfQuantumAllocatorGreedyValue() override = default;
   std::string          AllocatorType() override { return "greedy_value"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorGreedyValue(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;
};

// this allocator starts assigning tasks to the highest profit asset pairing first (that still remain)
// some tasks might be left unassigned
class WSF_MIL_EXPORT WsfQuantumAllocatorGreedyProfit : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorGreedyProfit()           = default;
   ~WsfQuantumAllocatorGreedyProfit() override = default;
   std::string          AllocatorType() override { return "greedy_profit"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorGreedyProfit(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;
};

// this allocator finds the optimal profit allocation of tasks to assets
// some tasks might be left unassigned, some assets might be left without tasks
// uses class: UtOptimalAssignment
class WSF_MIL_EXPORT WsfQuantumAllocatorOptimalProfit : public WsfQuantumAllocator
{
public:
   WsfQuantumAllocatorOptimalProfit()           = default;
   ~WsfQuantumAllocatorOptimalProfit() override = default;
   std::string          AllocatorType() override { return "optimal_profit"; }
   WsfQuantumAllocator* Clone() override { return new WsfQuantumAllocatorOptimalProfit(*this); }
   AllocationList       MakeAllocations(WsfQuantumMatrix& aMatrix) override;

private:
   UtOptimalAssignment mOptimalAssigner;
};

#endif
