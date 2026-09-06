// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfAdvancedBehaviorTree.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtPath.hpp"
#include "WsfAdvancedBehaviorObserver.hpp"
#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptProcessor.hpp"

unsigned int WsfAdvancedBehaviorTree::mNextUniqueId = 0;

WsfScriptAdvancedBehaviorTreeClass::WsfScriptAdvancedBehaviorTreeClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfAdvancedBehaviorTree");
   AddMethod(ut::make_unique<RootNode>());
   AddMethod(ut::make_unique<FindNode>());
   AddMethod(ut::make_unique<Id>());
   AddMethod(ut::make_unique<Name>());
}

UtScriptContext* WsfScriptAdvancedBehaviorTreeClass::GetContext(void* aObjectPtr)
{
   UtScriptContext* contextPtr = nullptr;
   auto             treePtr    = static_cast<WsfAdvancedBehaviorTree*>(aObjectPtr);
   if (treePtr != nullptr)
   {
      contextPtr = &(treePtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeClass, WsfAdvancedBehaviorTree, RootNode, 0, "WsfAdvancedBehaviorTreeNode", "")
{
   aReturnVal.SetPointer(UtScriptRef::Ref(aObjectPtr->RootNode().get(), aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeClass,
                        WsfAdvancedBehaviorTree,
                        FindNode,
                        1,
                        "WsfAdvancedBehaviorTreeNode",
                        "string")
{
   std::string behaviorName = aVarArgs[0].GetString();
   auto        nodePtr      = aObjectPtr->FindNode(behaviorName);
   aReturnVal.SetPointer(UtScriptRef::Ref(nodePtr, aReturnClassPtr, UtScriptRef::cDONT_MANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeClass, WsfAdvancedBehaviorTree, Id, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetTreeId());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptAdvancedBehaviorTreeClass, WsfAdvancedBehaviorTree, Name, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetName());
}

WsfAdvancedBehaviorTree::WsfAdvancedBehaviorTree(const WsfScenario& aScenario)
   : mContextPtr(ut::make_unique<WsfScriptContext>(*aScenario.GetScriptContext(), "WsfAdvancedBehaviorTree"))
   , mParentPtr(nullptr)
   , mShouldOutput(false)
{
   mId   = GetUniqueTreeId();
   mName = "abt_" + std::to_string(mId);
   WsfObject::SetName(WsfStringId("abt_" + std::to_string(mId)));
   WsfObject::SetType(WsfStringId("WSF_ADVANCED_BEHAVIOR_TREE"));

   // Create the root node as a parallel node.
   mRootNodeType = PARALLEL;
   mRootNodePtr  = std::make_shared<WsfAdvancedBehaviorTreeParallelNode>(aScenario);
   mRootNodePtr->SetTree(this);
   mRootNodePtr->SetType("root");
   mRootNodePtr->SetName("root");
   mRootNodePtr->SetIsTreeRootNode(true);

   // setup extern variables so any scripts compiled with this member context wont barf at us
   RegisterExternVariable("WsfPlatform", "PLATFORM");
   RegisterExternVariable("WsfProcessor", "PROCESSOR");
}

WsfAdvancedBehaviorTree::WsfAdvancedBehaviorTree(const WsfAdvancedBehaviorTree& aSrc)
   : mContextPtr(ut::make_unique<WsfScriptContext>(*aSrc.mContextPtr))
   , mBlackboards(aSrc.mBlackboards)
   , mParentPtr(nullptr)
   , mRootNodePtr(aSrc.mRootNodePtr)
   , mShouldOutput(false)
   , mOutputForBTT(aSrc.mOutputForBTT)
   , mName(aSrc.mName)
   , mRootNodeType(aSrc.mRootNodeType)
   , mSuccessPolicy(aSrc.mSuccessPolicy)
   , mThreshold(aSrc.mThreshold)
{
   mId = GetUniqueTreeId();
   SetName(aSrc.GetNameId());
   SetType(aSrc.GetTypeId());

   // Re-set the root's class based on its type
   SetRootNodeClass();
}

void WsfAdvancedBehaviorTree::AddBlackboard(const std::string& aName, std::shared_ptr<WsfAttributeContainer> aBlackboardPtr)
{
   mBlackboards[aName] = aBlackboardPtr;
}

WsfAdvancedBehaviorTree* WsfAdvancedBehaviorTree::Clone() const
{
   return new WsfAdvancedBehaviorTree(*this);
}

const std::string WsfAdvancedBehaviorTree::GetFullFilePath(UtInput& aInput) const
{
   // Get the full file path
   std::string fileName = aInput.GetCurrentFileName();
   if (fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\'))
   {
      fileName = fileName.substr(2); // Strip off first two characters of filename
   }
   std::string rootPath = UtPath::WorkingDirectory().GetNormalizedPath();
   return rootPath + "/" + fileName;
}

void WsfAdvancedBehaviorTree::SetRootNodeClass()
{
   mBehaviorNodes.clear();

   switch (mRootNodeType)
   {
   case SEQUENCE:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeSequenceNode>(*mRootNodePtr);
      mRootNodePtr->SetType("sequence");
      break;
   case SEQUENCE_WITH_MEMORY:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeSequenceNodeWithMemory>(*mRootNodePtr, BT::cON_FAILURE);
      mRootNodePtr->SetType("sequence*");
      break;
   case SELECTOR:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeSelectorNode>(*mRootNodePtr);
      mRootNodePtr->SetType("selector");
      break;
   case SELECTOR_WITH_MEMORY:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeSelectorNodeWithMemory>(*mRootNodePtr, BT::cON_SUCCESS);
      mRootNodePtr->SetType("selector*");
      break;
   case PRIORITY_SELECTOR:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreePrioritySelectorNode>(*mRootNodePtr);
      mRootNodePtr->SetType("priority_selector");
      break;
   case WEIGHTED_RANDOM:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeWeightedRandomNode>(*mRootNodePtr);
      mRootNodePtr->SetType("weighted_random");
      break;
   case PARALLEL:
   default:
      mRootNodePtr = std::make_shared<WsfAdvancedBehaviorTreeParallelNode>(*mRootNodePtr, mSuccessPolicy, mThreshold);
      mRootNodePtr->SetType("parallel");
      break;
   }

   // Set all of the nodes to this tree recursively
   if (mRootNodePtr != nullptr)
   {
      mRootNodePtr->SetTree(this);
   }
}

WsfAttributeContainer* WsfAdvancedBehaviorTree::GetSharedBlackboard(const std::string& aName) const
{
   if (SharedBlackboardExists(aName))
   {
      return mBlackboards.at(aName).get();
   }
   return nullptr; // throw std::logic_error("Blackboard with name " + aName + " doesn't exist!");
}

std::vector<std::string> WsfAdvancedBehaviorTree::GetSharedBlackboardNames() const
{
   std::vector<std::string> keys;
   for (auto const& kv : mBlackboards)
   {
      keys.push_back(kv.first);
   }
   return keys;
}

WsfSimulation* WsfAdvancedBehaviorTree::GetSimulation()
{
   return (mParentPtr != nullptr) ? mParentPtr->GetSimulation() : mRootNodePtr->GetScriptContext().GetSimulation();
}


bool WsfAdvancedBehaviorTree::Initialize(double aSimTime, WsfScriptProcessor* aParentPtr, WsfScriptContext* aParentContextPtr)
{
   // If we don't have a parent or parent context, return.
   if (aParentPtr == nullptr && aParentContextPtr == nullptr)
   {
      return false;
   }
   mParentPtr = aParentPtr;

   // Otherwise, set our pointers
   WsfScriptContext* parentContextPtr =
      (aParentContextPtr != nullptr) ? (aParentContextPtr) : (&mParentPtr->GetScriptContext());

   mContextPtr->SetParent(parentContextPtr);

   bool retVal = mContextPtr->Initialize(aSimTime, *parentContextPtr, this);

   // Initialize the nodes of the tree
   retVal &= mRootNodePtr->Initialize(aSimTime, mContextPtr.get());

   // Log the structure of the tree.
   // This prints the node info to the event pipe that will draw the tree on the Mystic side.
   // Loop through all sub trees and output them as well
   OutputTreeStructure(aSimTime);
   mRootNodePtr->OutputTreeStructures(aSimTime);

   return retVal;
}

WsfAdvancedBehaviorTreeActionNode* WsfAdvancedBehaviorTree::NodeEntry(size_t aIndex) const
{
   // If the index given is bigger than our list, return
   if (aIndex >= mBehaviorNodes.size())
   {
      return nullptr;
   }

   // Otherwise, return the node at the index given
   return mBehaviorNodes[aIndex];
}

bool WsfAdvancedBehaviorTree::ProcessInput(UtInput& aInput)
{
   mRootNodePtr->SetFilePath(GetFullFilePath(aInput));

   // If we have a new behavior tree, process input for all children of the root node
   bool        myCommand = false;
   std::string cmd       = aInput.GetCommand();
   if (cmd == "advanced_behavior_tree")
   {
      myCommand = ProcessTree(aInput);
   }
   return myCommand;
}

std::unique_ptr<WsfAdvancedBehaviorTreeNode> WsfAdvancedBehaviorTree::CreateNode(UtInput& aInput)
{
   // Create the node
   std::unique_ptr<WsfAdvancedBehaviorTreeNode> newNodePtr = nullptr;
   std::string                                  cmd        = aInput.GetCommand();
   // If we have a new action node
   if (cmd == "behavior_node")
   {
      std::string behaviorName;
      // Read the behavior name
      if (aInput.ReadCommand(behaviorName))
      {
         newNodePtr = std::unique_ptr<WsfAdvancedBehaviorTreeNode>(
            WsfScenario::FromInput(aInput).GetAdvancedBehaviorTreeNodeTypes().Clone(behaviorName));
      }
      // If our node is nullptr here, the name couldn't be read
      if (newNodePtr == nullptr)
      {
         // Throw script error, node not defined
         std::string msg = "behavior not found: " + behaviorName + " was not previously defined!";
         throw UtInput::BadValue(aInput, msg);
      }

      // Set the file path and a pointer to the tree for the node.
      // Action nodes will get registered in a separate list for the WsfProcessor.
      newNodePtr->SetFilePath(GetFullFilePath(aInput));
      newNodePtr->SetTree(this);
   }
   // If we have a new condition node
   else if (cmd == "condition")
   {
      std::string conditionName;
      if (aInput.ReadCommand(conditionName))
      {
         newNodePtr = std::unique_ptr<WsfAdvancedBehaviorTreeNode>(
            WsfScenario::FromInput(aInput).GetAdvancedBehaviorTreeNodeTypes().Clone(conditionName));
      }

      // If our node is nullptr here, the name couldn't be read
      if (newNodePtr == nullptr)
      {
         // Throw script error, node not defined
         std::string msg = "behavior not found: " + conditionName + " was not previously defined!";
         throw UtInput::BadValue(aInput, msg);
      }

      // Set the file path and a pointer to the tree for the node.
      newNodePtr->SetFilePath(GetFullFilePath(aInput));
      newNodePtr->SetTree(this);
   }
   // If we have a new composite node
   else if (cmd == "sequence" || cmd == "selector" || cmd == "selector_with_memory" || cmd == "sequence_with_memory" ||
            cmd == "parallel" || cmd == "priority_selector" || cmd == "weighted_random" || cmd == "decorator")
   {
      if (cmd == "sequence")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeSequenceNode>(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "sequence_with_memory")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeSequenceNodeWithMemory>(WsfScenario::FromInput(aInput),
                                                                                     BT::cON_FAILURE);
      }
      else if (cmd == "selector")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeSelectorNode>(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "selector_with_memory")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeSelectorNodeWithMemory>(WsfScenario::FromInput(aInput),
                                                                                     BT::cON_SUCCESS);
      }
      else if (cmd == "parallel")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeParallelNode>(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "priority_selector")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreePrioritySelectorNode>(WsfScenario::FromInput(aInput));
      }
      else if (cmd == "weighted_random")
      {
         newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeWeightedRandomNode>(WsfScenario::FromInput(aInput));
      }
      else // if (cmd == "decorator")
      {
         UtInputBlock block(aInput);
         block.ReadCommand(cmd);
         // If we have a new decorator node, read what type of decorator we have, and any commands that come with it.
         if (cmd == "inverter")
         {
            newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeInverterNode>(WsfScenario::FromInput(aInput));
         }
         else if (cmd == "succeeder")
         {
            newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeSucceederNode>(WsfScenario::FromInput(aInput));
         }
         else if (cmd == "negator")
         {
            newNodePtr = ut::make_unique<WsfAdvancedBehaviorTreeNegatorNode>(WsfScenario::FromInput(aInput));
         }
         // If we have a new repeater decorator, read in how the node should repeat
         else if (cmd == "repeater")
         {
            newNodePtr    = ut::make_unique<WsfAdvancedBehaviorTreeRepeaterNode>(WsfScenario::FromInput(aInput));
            auto repeater = static_cast<WsfAdvancedBehaviorTreeRepeaterNode*>(newNodePtr.get());

            std::string repeatCommand;
            aInput.ReadCommand(repeatCommand);
            // repeat for <num> <time-unit>
            if (repeatCommand == "for")
            {
               repeater->SetRunType(WsfAdvancedBehaviorTreeRepeaterNode::RunType::ForTime);
               aInput.ReadValueOfType(repeater->GetRunDuration(), UtInput::cTIME);
            }
            // repeat <int> times
            else if (repeatCommand == "repeat")
            {
               repeater->SetRunType(WsfAdvancedBehaviorTreeRepeaterNode::RunType::Repeat);
               aInput.ReadValue(repeater->GetRepeats());
               repeater->SetRepeats(repeater->GetRepeats() > 1 ? repeater->GetRepeats() : 1); // minimum of 1 repeat
            }
            // repeat until done
            else if (repeatCommand == "until_done")
            {
               repeater->SetRunType(WsfAdvancedBehaviorTreeRepeaterNode::RunType::UntilDone);
            }
            else
            {
               std::string msg = "Unknown repeat type: " + repeatCommand +
                                 "! Valid uses: [until_done, repeat <int>, for <num> <time-unit>]";
               throw UtInput::BadValue(aInput, msg);
            }
         } // else if (cmd == "repeater")
         else
         {
            std::string msg = "Unknown decorator type: " + cmd + "!";
            throw UtInput::BadValue(aInput, msg);
         }

         // Add a child to the decorator if it has one.
         block.ReadCommand(cmd);
         if (cmd == "sequence" || cmd == "selector" || cmd == "selector_with_memory" || cmd == "sequence_with_memory" ||
             cmd == "parallel" || cmd == "priority_selector" || cmd == "weighted_random" || cmd == "behavior_node" ||
             cmd == "decorator" || cmd == "condition")
         {
            auto newChildPtr = CreateNode(aInput);
            if (newChildPtr != nullptr)
            {
               newNodePtr->AddChild(std::move(newChildPtr));
            }
         }
         else if (cmd == "advanced_behavior_tree")
         {
            ut::CloneablePtr<WsfAdvancedBehaviorTree> newTreePtr =
               ut::make_unique<WsfAdvancedBehaviorTree>(WsfScenario::FromInput(aInput));
            if (newTreePtr != nullptr)
            {
               newTreePtr->RootNode()->SetName(newTreePtr->GetName());

               // Set the file path and the tree
               newTreePtr->RootNode()->SetFilePath(GetFullFilePath(aInput));
               newTreePtr->RootNode()->SetIsTreeRootNode(true);

               newTreePtr->ProcessTree(aInput);

               newNodePtr->AddTree(newTreePtr);
            }
         }
         else if (cmd == "root_node_type")
         {
            // Get the tree name
            std::string rootNodeType;
            aInput.ReadValue(rootNodeType);
            if (rootNodeType == "sequence")
            {
               mRootNodeType = SEQUENCE;
            }
            else if (rootNodeType == "sequence_with_memory")
            {
               mRootNodeType = SEQUENCE_WITH_MEMORY;
            }
            else if (rootNodeType == "selector")
            {
               mRootNodeType = SELECTOR;
            }
            else if (rootNodeType == "selector_with_memory")
            {
               mRootNodeType = SELECTOR_WITH_MEMORY;
            }
            else if (rootNodeType == "parallel")
            {
               mRootNodeType = PARALLEL;
            }
            else if (rootNodeType == "priority_selector")
            {
               mRootNodeType = PRIORITY_SELECTOR;
            }
            else if (rootNodeType == "weighted_random")
            {
               mRootNodeType = WEIGHTED_RANDOM;
            }
         }
         else
         {
            std::string msg = "Invalid decorator child: " + cmd + "!";
            throw UtInput::BadValue(aInput, msg);
         }
         return newNodePtr;
      } // else (cmd == "decorator")

      // Set the file path and the tree
      newNodePtr->SetFilePath(GetFullFilePath(aInput));
      newNodePtr->SetTree(this);

      UtInputBlock block(aInput);
      // Recursively check and create children for the composite nodes
      while (block.ReadCommand(cmd))
      {
         if (cmd == "sequence" || cmd == "selector" || cmd == "selector_with_memory" || cmd == "sequence_with_memory" ||
             cmd == "parallel" || cmd == "priority_selector" || cmd == "weighted_random" || cmd == "behavior_node" ||
             cmd == "decorator" || cmd == "condition")
         {
            auto newChildPtr = CreateNode(aInput);
            if (newChildPtr != nullptr)
            {
               newNodePtr->AddChild(std::move(newChildPtr));
            }
         }
         else if (cmd == "advanced_behavior_tree")
         {
            ut::CloneablePtr<WsfAdvancedBehaviorTree> newTreePtr =
               ut::make_unique<WsfAdvancedBehaviorTree>(WsfScenario::FromInput(aInput));
            if (newTreePtr != nullptr)
            {
               newTreePtr->RootNode()->SetName(newTreePtr->GetName());

               // Set the file path and the tree
               newTreePtr->RootNode()->SetFilePath(GetFullFilePath(aInput));
               newTreePtr->RootNode()->SetIsTreeRootNode(true);

               newTreePtr->ProcessTree(aInput);

               newNodePtr->AddTree(newTreePtr);
            }
         }
         else if (cmd == "root_node_type")
         {
            // Get the tree name
            std::string rootNodeType;
            aInput.ReadValue(rootNodeType);
            if (rootNodeType == "sequence")
            {
               mRootNodeType = SEQUENCE;
            }
            else if (rootNodeType == "sequence_with_memory")
            {
               mRootNodeType = SEQUENCE_WITH_MEMORY;
            }
            else if (rootNodeType == "selector")
            {
               mRootNodeType = SELECTOR;
            }
            else if (rootNodeType == "selector_with_memory")
            {
               mRootNodeType = SELECTOR_WITH_MEMORY;
            }
            else if (rootNodeType == "parallel")
            {
               mRootNodeType = PARALLEL;
            }
            else if (rootNodeType == "priority_selector")
            {
               mRootNodeType = PRIORITY_SELECTOR;
            }
            else if (rootNodeType == "weighted_random")
            {
               mRootNodeType = WEIGHTED_RANDOM;
            }
         }
         else if (newNodePtr->ProcessInput(aInput))
         {
            // sets any generic node definitions
            // see WsfAdvancedBehaviorTreeNode::ProcessInput()
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      } // while (block.ReadCommand(cmd))
   }    // else if (cmd == "sequence" || ...
   return newNodePtr;
}

bool WsfAdvancedBehaviorTree::ProcessTree(UtInput& aInput)
{
   std::string  cmd = aInput.GetCommand();
   UtInputBlock block(aInput);
   while (block.ReadCommand(cmd))
   {
      // If we have a node command, create the node and add it to the root node
      if (cmd == "sequence" || cmd == "selector" || cmd == "selector_with_memory" || cmd == "sequence_with_memory" ||
          cmd == "parallel" || cmd == "priority_selector" || cmd == "weighted_random" || cmd == "behavior_node" ||
          cmd == "decorator" || cmd == "condition")
      {
         // We do not need to cast our rootNodePtr as it is pre-set to a parallel node.
         auto newChildPtr = CreateNode(aInput);
         if (newChildPtr != nullptr)
         {
            mRootNodePtr->AddChild(std::move(newChildPtr));
         }
      }
      else if (cmd == "advanced_behavior_tree")
      {
         ut::CloneablePtr<WsfAdvancedBehaviorTree> newTreePtr =
            ut::make_unique<WsfAdvancedBehaviorTree>(WsfScenario::FromInput(aInput));
         if (newTreePtr != nullptr)
         {
            newTreePtr->RootNode()->SetName(newTreePtr->GetName());

            // Set the file path and the tree
            newTreePtr->RootNode()->SetFilePath(GetFullFilePath(aInput));
            newTreePtr->RootNode()->SetIsTreeRootNode(true);

            newTreePtr->ProcessTree(aInput);

            mRootNodePtr->AddTree(newTreePtr);
         }
      }
      else if (cmd == "btt")
      {
         // Get if this tree wants to output to a BTT
         aInput.ReadValue(mOutputForBTT);
      }
      else if (cmd == "name")
      {
         // Get the tree name
         aInput.ReadValueQuoted(mName);
         SetName(mName);
         SetType(mName);
         mRootNodePtr->SetName(mName);
      }
      else if (cmd == "desc" || cmd == "description")
      {
         // Get the description
         std::string tempDesc;
         aInput.ReadValueQuoted(tempDesc);
         mRootNodePtr->SetDescription(tempDesc);
      }
      else if (cmd == "root_node_type")
      {
         // Get the tree name
         std::string rootNodeType;
         aInput.ReadValue(rootNodeType);
         if (rootNodeType == "sequence")
         {
            mRootNodeType = SEQUENCE;
         }
         else if (rootNodeType == "sequence_with_memory")
         {
            mRootNodeType = SEQUENCE_WITH_MEMORY;
         }
         else if (rootNodeType == "selector")
         {
            mRootNodeType = SELECTOR;
         }
         else if (rootNodeType == "selector_with_memory")
         {
            mRootNodeType = SELECTOR_WITH_MEMORY;
         }
         else if (rootNodeType == "parallel")
         {
            mRootNodeType = PARALLEL;
         }
         else if (rootNodeType == "priority_selector")
         {
            mRootNodeType = PRIORITY_SELECTOR;
         }
         else if (rootNodeType == "weighted_random")
         {
            mRootNodeType = WEIGHTED_RANDOM;
         }
         SetRootNodeClass();
      }
      else if (mRootNodePtr->ProcessInput(aInput))
      {
      }
      else
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }
   return true;
}

void WsfAdvancedBehaviorTree::RegisterActionNode(WsfAdvancedBehaviorTreeActionNode* aNodePtr)
{
   mBehaviorNodes.push_back(aNodePtr);
}

const bool WsfAdvancedBehaviorTree::SharedBlackboardExists(const std::string& aName) const
{
   if (mBlackboards.find(aName) != mBlackboards.end())
   {
      return true;
   }
   return false;
}

bool WsfAdvancedBehaviorTree::Tick(double aSimTime)
{
   if (mOutputForBTT && mShouldOutput)
   {
      mRootNodePtr->OutputTreeStates(aSimTime);
      mShouldOutput = false;
   }

   mRootNodePtr->ResetPreconditionVars();
   return mRootNodePtr->Tick();
}

WsfAdvancedBehaviorTreeNode* WsfAdvancedBehaviorTree::FindNode(const std::string& aNodeName) const
{
   if (mRootNodePtr != nullptr)
   {
      return mRootNodePtr->FindNode(aNodeName);
   }
   return nullptr;
}

void WsfAdvancedBehaviorTree::FindLastExecuted(std::vector<WsfAdvancedBehaviorTreeNode*>& aLastExecuted) const
{
   if (mRootNodePtr != nullptr)
   {
      mRootNodePtr->FindLastExecuted(aLastExecuted);
   }
}

WsfPlatform* WsfAdvancedBehaviorTree::GetOwningPlatform() const
{
   if (mParentPtr != nullptr)
   {
      return mParentPtr->GetPlatform();
   }
   else if (mRootNodePtr != nullptr)
   {
      return WsfScriptContext::GetPLATFORM(mRootNodePtr->GetScriptContext().GetContext());
   }
   return nullptr;
}

const char* WsfAdvancedBehaviorTree::GetScriptClassName() const
{
   return "WsfAdvancedBehaviorTree";
}

UtScriptContext* WsfAdvancedBehaviorTree::GetScriptAccessibleContext() const
{
   return &mContextPtr->GetContext();
}

bool WsfAdvancedBehaviorTree::RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName)
{
   bool registered = false;
   if (mContextPtr != nullptr)
   {
      registered =
         (mContextPtr->GetContext().ModifyScope()->RegisterExternVariable(aVariableName, aVariableType) != nullptr);
   }
   return registered;
}

void WsfAdvancedBehaviorTree::OutputTreeStructure(const double aSimTime)
{
   if (mOutputForBTT)
   {
      WsfObserver::AdvancedBehaviorTree(GetSimulation())(aSimTime, this);
   }
}

void WsfAdvancedBehaviorTree::OutputTreeState(const double aSimTime)
{
   if (mOutputForBTT)
   {
      WsfObserver::AdvancedBehaviorTreeState(GetSimulation())(aSimTime, mRootNodePtr.get());
   }
}
