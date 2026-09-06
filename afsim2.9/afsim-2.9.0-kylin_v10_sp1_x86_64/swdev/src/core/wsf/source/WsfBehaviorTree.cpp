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

#include "WsfBehaviorTree.hpp"

#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtPath.hpp"
#include "WsfBehaviorTreeNode.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptProcessor.hpp"

WsfBehaviorTree::WsfBehaviorTree(const WsfScenario& aScenario)
   : WsfObject()
   , mParentPtr(nullptr)
{
   mRootNodePtr = new WsfBehaviorTreeParallelNode(aScenario);
   mRootNodePtr->SetTree(this);
   mRootNodePtr->SetType("root"); // double check this?  behavior types are object names & behavior names are object types
}

WsfBehaviorTree::WsfBehaviorTree(const WsfBehaviorTree& aSrc)
   : mParentPtr(nullptr)
{
   mRootNodePtr = dynamic_cast<WsfBehaviorTreeParallelNode*>(aSrc.mRootNodePtr->Clone()); // get a cloned root object
   mRootNodePtr->SetTree(this);
}

WsfBehaviorTree& WsfBehaviorTree::operator=(const WsfBehaviorTree& aRhs)
{
   if (this != &aRhs)
   {
      WsfObject::operator=(aRhs);
      mParentPtr         = nullptr;
      mRootNodePtr = dynamic_cast<WsfBehaviorTreeParallelNode*>(aRhs.mRootNodePtr->Clone()); // get a cloned root object
      mRootNodePtr->SetTree(this);
   }
   return *this;
}

WsfBehaviorTree* WsfBehaviorTree::Clone() const
{
   return new WsfBehaviorTree(*this);
}

WsfBehaviorTree::~WsfBehaviorTree()
{
   delete mRootNodePtr;
}

WsfBehaviorTreeNode* WsfBehaviorTree::CreateNode(UtInput& aInput)
{
   std::string fileName = aInput.GetCurrentFileName();
   if (fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\'))
   {
      fileName = fileName.substr(2); // strip off first two characters of filename
   }
   std::string rootPath     = UtPath::WorkingDirectory().GetNormalizedPath();
   std::string fullPathName = rootPath + "/" + fileName;

   WsfBehaviorTreeNode* newNodePtr = nullptr;
   std::string          cmd        = aInput.GetCommand();
   // is it a new leaf node?
   if (cmd == "behavior_node")
   {
      std::string behaviorName;
      if (aInput.ReadCommand(behaviorName))
      {
         newNodePtr = WsfScenario::FromInput(aInput).GetBehaviorTreeNodeTypes().Clone(behaviorName);
      }
      if (newNodePtr == nullptr)
      {
         // throw script error, node not defined
         std::string msg = "behavior not found: " + behaviorName + " was not previously defined!";
         throw UtInput::BadValue(aInput, msg);
      }
      newNodePtr->SetTree(this); // this registers the node on the tree too
      // RegisterBehaviorNode((WsfBehaviorTreeLeafNode*)newNodePtr);  //don't register it twice
   }
   else if (cmd == "sequence" || cmd == "selector" || cmd == "parallel" || cmd == "priority_selector" ||
            cmd == "weighted_random")
   {
      if (cmd == "sequence")
      {
         newNodePtr = new WsfBehaviorTreeSequenceNode(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "selector")
      {
         newNodePtr = new WsfBehaviorTreeSelectorNode(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "parallel")
      {
         newNodePtr = new WsfBehaviorTreeParallelNode(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "priority_selector")
      {
         newNodePtr = new WsfBehaviorTreePrioritySelectorNode(WsfScenario::FromInput(aInput));
      }
      else /*if (cmd == "weighted_random")*/
      {
         newNodePtr = new WsfBehaviorTreeWeightedRandomNode(WsfScenario::FromInput(aInput));
      }
      newNodePtr->SetFilePath(fullPathName);
      newNodePtr->SetTree(this);
      UtInputBlock block(aInput);
      while (block.ReadCommand(cmd))
      {
         if (cmd == "sequence" || cmd == "selector" || cmd == "parallel" || cmd == "priority_selector" ||
             cmd == "weighted_random" || cmd == "behavior_node")
         {
            WsfBehaviorTreeNode* newChildPtr = CreateNode(aInput);
            if (newChildPtr != nullptr)
            {
               newNodePtr->AddChild(newChildPtr);
            }
         }
         else if (newNodePtr->ProcessInput(aInput))
         {
            // sets any generic node definitions
            //[see WsfBehaviorTreeNode::ProcessInput() for what is supported]
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return newNodePtr;
}

bool WsfBehaviorTree::ProcessInput(UtInput& aInput)
{
   std::string fileName = aInput.GetCurrentFileName();
   if (fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\'))
   {
      fileName = fileName.substr(2); // strip off first two characters of filename
   }
   std::string rootPath     = UtPath::WorkingDirectory().GetNormalizedPath();
   std::string fullPathName = rootPath + "/" + fileName;
   mRootNodePtr->SetFilePath(fullPathName);

   bool        myCommand = false;
   std::string cmd       = aInput.GetCommand();
   if (cmd == "behavior_tree")
   {
      myCommand = true;
      UtInputBlock block(aInput);
      while (block.ReadCommand(cmd))
      {
         if (cmd == "sequence" || cmd == "selector" || cmd == "parallel" || cmd == "priority_selector" ||
             cmd == "weighted_random" || cmd == "behavior_node")
         {
            WsfBehaviorTreeNode* newChildPtr = CreateNode(aInput);
            if (newChildPtr != nullptr)
            {
               mRootNodePtr->AddChild(newChildPtr);
            }
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

bool WsfBehaviorTree::Execute(double simTime)
{
   if (mParentPtr != nullptr)
   {
      // TODO - is this needed? (useful for nested behavior trees in FSM states?)
      // mParentPtr->MarkCurrentBehaviorTree(this);
   }
   return mRootNodePtr->Execute(simTime);
}

bool WsfBehaviorTree::Initialize(double aSimTime, WsfScriptProcessor* aParentPtr, WsfScriptContext* aParentContextPtr)
{
   if (aParentPtr == nullptr && aParentContextPtr == nullptr)
   {
      return false;
   }
   mParentPtr = aParentPtr;

   WsfScriptContext* parentContextPtr =
      (aParentContextPtr != nullptr) ? (aParentContextPtr) : (&mParentPtr->GetScriptContext());

   // initialize the nodes of the tree
   bool retVal = mRootNodePtr->Initialize(aSimTime, parentContextPtr);

   // log the structure of the tree
   mRootNodePtr->LogNodeStructure(aSimTime, true);

   return retVal;
}

WsfBehaviorTreeNode* WsfBehaviorTree::FindNode(const std::string& aNodeName) const
{
   return mRootNodePtr->FindNode(aNodeName);
}

void WsfBehaviorTree::FindLastExecuted(std::vector<WsfBehaviorTreeNode*>& aLastExecuted) const
{
   mRootNodePtr->FindLastExecuted(aLastExecuted);
}

size_t WsfBehaviorTree::NodeCount() const
{
   return mBehaviorNodes.size();
}

WsfBehaviorTreeLeafNode* WsfBehaviorTree::NodeEntry(size_t aIndex) const
{
   if (aIndex >= mBehaviorNodes.size())
   {
      return nullptr;
   }
   return mBehaviorNodes[aIndex];
}

void WsfBehaviorTree::RegisterBehaviorNode(WsfBehaviorTreeLeafNode* aNodePtr)
{
   mBehaviorNodes.push_back(aNodePtr);
}

WsfSimulation* WsfBehaviorTree::GetSimulation()
{
   return (mParentPtr != nullptr) ? mParentPtr->GetSimulation() : mRootNodePtr->GetScriptContext().GetSimulation();
}
