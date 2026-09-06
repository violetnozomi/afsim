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

#ifndef WSFBEHAVIORTREE_HPP
#define WSFBEHAVIORTREE_HPP

#include <string>
#include <vector>

class UtInput;
class WsfBehaviorTreeNode;
class WsfBehaviorTreeLeafNode;
class WsfBehaviorTreeParallelNode;
#include "wsf_export.h"

#include "WsfObject.hpp"
class WsfScenario;
class WsfScriptContext;
class WsfScriptProcessor;
class WsfSimulation;

class WSF_EXPORT WsfBehaviorTree : public WsfObject
{
public:
   WsfBehaviorTree(const WsfScenario& aScenario);
   WsfBehaviorTree(const WsfBehaviorTree& aSrc);
   WsfBehaviorTree& operator=(const WsfBehaviorTree& aRhs);
   WsfBehaviorTree* Clone() const override;
   ~WsfBehaviorTree() override;

   // clones a node from its type definition, records it in map,
   // returns it for insertion into tree structure
   WsfBehaviorTreeNode* CreateNode(UtInput& aInput);

   bool ProcessInput(UtInput& aInput) override;

   bool Execute(double simTime);

   bool Initialize(double aSimTime, WsfScriptProcessor* aParentPtr, WsfScriptContext* aParentContextPtr = nullptr);

   WsfBehaviorTreeParallelNode* RootNode() const { return mRootNodePtr; }

   WsfScriptProcessor* ParentProcessor() { return mParentPtr; }

   WsfBehaviorTreeNode* FindNode(const std::string& aNodeName) const; // from whole tree

   //! Provides information on what the behavior tree is "currently" doing.
   //! @param aLastExecuted Returns the last nodes that were executed.
   void FindLastExecuted(std::vector<WsfBehaviorTreeNode*>& aLastExecuted) const; // from whole tree

   size_t                   NodeCount() const;              // only counting leaf "behavior" nodes
   WsfBehaviorTreeLeafNode* NodeEntry(size_t aIndex) const; // only counting leaf "behavior" nodes

   void RegisterBehaviorNode(WsfBehaviorTreeLeafNode* aNodePtr);

   WsfSimulation* GetSimulation();

private:
   WsfBehaviorTreeParallelNode*          mRootNodePtr;
   WsfScriptProcessor*                   mParentPtr;
   std::vector<WsfBehaviorTreeLeafNode*> mBehaviorNodes;
};

#endif
