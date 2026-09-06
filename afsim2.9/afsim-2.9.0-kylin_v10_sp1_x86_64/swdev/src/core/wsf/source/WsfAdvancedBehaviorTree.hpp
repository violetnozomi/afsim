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

#ifndef WSFADVANCEDBEHAVIORTREE_HPP
#define WSFADVANCEDBEHAVIORTREE_HPP

#include "wsf_export.h"

#include <map>
#include <string>
#include <vector>

#include "WsfAdvancedBehaviorTreeNode.hpp"
#include "WsfObject.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptObjectClass.hpp"

class UtInput;
class WsfAttributeContainer;
class WsfPlatform;
class WsfScenario;
class WsfScriptContext;
class WsfScriptProcessor;
class WsfSimulation;


class WSF_EXPORT WsfScriptAdvancedBehaviorTreeClass : public WsfScriptObjectClass
{
public:
   WsfScriptAdvancedBehaviorTreeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   /** Return a pointer to the script context for an object.
    * @param aObjectPtr object to get context for.
    */
   UtScriptContext* GetContext(void* aObjectPtr) override;

   // Script methods
   // Each of the exported methods are defined as function objects.
   UT_DECLARE_SCRIPT_METHOD(RootNode);
   UT_DECLARE_SCRIPT_METHOD(FindNode);
   UT_DECLARE_SCRIPT_METHOD(Id);
   UT_DECLARE_SCRIPT_METHOD(Name);
};

class WSF_EXPORT WsfAdvancedBehaviorTree : public WsfObject
{
   enum RootNodeType
   {
      PARALLEL,
      SEQUENCE,
      SEQUENCE_WITH_MEMORY,
      SELECTOR,
      SELECTOR_WITH_MEMORY,
      PRIORITY_SELECTOR,
      WEIGHTED_RANDOM
   };

public:
   WsfAdvancedBehaviorTree(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTree(const WsfAdvancedBehaviorTree& aSrc);
   ~WsfAdvancedBehaviorTree() override = default;
   WsfAdvancedBehaviorTree& operator=(const WsfAdvancedBehaviorTree& aRhs) = delete;

   /** Add a blackboard to the blackboard list
    * @param aName name to reference blackboard with.
    * @param aBlackboardPtr blackboard to add.
    */
   void AddBlackboard(const std::string& aName, std::shared_ptr<WsfAttributeContainer> aBlackboardPtr);

   /** Clone this tree. */
   WsfAdvancedBehaviorTree* Clone() const override;

   /** Get a blackboard with the specified name from the shared list.
    * @param aName name of the blackboard to get
    */
   WsfAttributeContainer* GetSharedBlackboard(const std::string& aName) const;

   /** Get a list of all shared blackboard names. */
   std::vector<std::string> GetSharedBlackboardNames() const;

   /** Return a pointer to the simulation. */
   WsfSimulation* GetSimulation();

   /** Initialize this tree.
    * @param aSimTime sim time at initialization.
    * @param aParentPtr pointer to the parent script processor.
    * @param aParentContextPtr pointer to the parent script context.
    */
   bool Initialize(double aSimTime, WsfScriptProcessor* aParentPtr, WsfScriptContext* aParentContextPtr = nullptr);

   /** Return a count of all leaf nodes. */
   const size_t NodeCount() const { return mBehaviorNodes.size(); }

   /** Get the id of this tree. */
   const unsigned int& GetTreeId() const { return mId; }

   /** Get the name of this tree. */
   const std::string& GetName() const { return mName; }

   /** Get a unique int id for this tree. */
   static unsigned int GetUniqueTreeId() { return ++mNextUniqueId; }

   /** Reset the unique int counter. */
   static void Reset() { mNextUniqueId = 0; }

   /** Returns an action node at an index.
    * @param aIndex index of node to retrieve.
    */
   WsfAdvancedBehaviorTreeActionNode* NodeEntry(size_t aIndex) const;

   /** Return a pointer to the parent processor of this tree. */
   WsfScriptProcessor* GetParentProcessor() const { return mParentPtr; }

   /** Process input from a script.
    * @param aInput input to parse.
    */
   bool ProcessInput(UtInput& aInput) override;

   /** Register an action node in our list, for easier tracking and updating in the processor.
    * @param aNodePtr node to register.
    */
   void RegisterActionNode(WsfAdvancedBehaviorTreeActionNode* aNodePtr);

   /** Return a shared pointer to the root node of this tree. */
   std::shared_ptr<WsfAdvancedBehaviorTreeCompositeNode> RootNode() const { return mRootNodePtr; }

   /** Get if a blackboard with the specified name exists in the shared list.
    * @param aName name of the blackboard to get
    */
   const bool SharedBlackboardExists(const std::string& aName) const;

   /** Set if the tree should output node values to the event pipe next tick.
    * @param aValue value to set.
    */
   void SetShouldOutputNextTick(const bool aValue) { mShouldOutput = aValue; }

   /** Tick(update) the tree.
    * @param aSimTime sim time when this function is called.
    */
   bool Tick(double aSimTime);

   /** Find and return a pointer to the node with a given name.
    * @param aNodeName name of the node to find.
    */
   WsfAdvancedBehaviorTreeNode* FindNode(const std::string& aNodeName) const;

   /** Fill a given vector with pointers to nodes that executed last tick.
    * @param aLastExecuted vector to fill.
    */
   void FindLastExecuted(std::vector<WsfAdvancedBehaviorTreeNode*>& aLastExecuted) const;

   /** Return a pointer to the owning platform of this tree. */
   WsfPlatform* GetOwningPlatform() const;

   /** Return the script class name for this tree. */
   const char* GetScriptClassName() const override;

   /** Returns a reference to the script context for this tree. */
   const WsfScriptContext& GetScriptContext() const { return *mContextPtr; }

   /** Set the parent processor.
    * @param aParentPtr parent to set.
    */
   void SetParent(WsfScriptProcessor* aParentPtr) { mParentPtr = aParentPtr; }

   /** Set the parent script context for this tree.
    * @param aParentContextPtr parent context to set.
    */
   void SetParentContext(WsfScriptContext* aParentContextPtr) { mContextPtr->SetParent(aParentContextPtr); }

   /** Cast the root node to the composite node class set in the tree. */
   void SetRootNodeClass();

   /** Output the structure of this tree to the event pipe.
    * @param aSimTime time at output.
    */
   void OutputTreeStructure(const double aSimTime);

   /** Output the state of this tree to the event pipe.
    * @param aSimTime time at output.
    */
   void OutputTreeState(const double aSimTime);

   /** Set the success policy for this node.
    * @param aPolicy policy to use when evaluating for success
    */
   void SetSuccessPolicy(const BT::SuccessPolicy aPolicy) { mSuccessPolicy = aPolicy; }

   /** Set the threshold(number) of children that must succeed for this node to succeed.
    * @param aThreshold the threshold that must be hit for this node to succeed.
    */
   void SetThreshold(const unsigned int aThreshold) { mThreshold = aThreshold; }

protected:
   /** Return the context from the script. */
   UtScriptContext* GetScriptAccessibleContext() const override;

   /** Register an external variable.
    * @param aVariableType variable type to register.
    * @param aVariableName name of the variable.
    */
   bool RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName);

private:
   /** Clones a node from its type definition and returns it for insertion into the tree.
    * @param aInput input to parse.
    */
   std::unique_ptr<WsfAdvancedBehaviorTreeNode> CreateNode(UtInput& aInput);

   /** Process input from a script.
    * @param aInput input to parse.
    */
   bool ProcessTree(UtInput& aInput);

   /** Get the full file path from the input.
    * @param aInput input to parse.
    */
   const std::string GetFullFilePath(UtInput& aInput) const;

private:
   std::unique_ptr<WsfScriptContext>               mContextPtr{nullptr}; ///< Pointer to the script context of this tree
   std::vector<WsfAdvancedBehaviorTreeActionNode*> mBehaviorNodes; ///< List of leaf nodes owned by this tree.  The
                                                                   ///< WsfScriptProcessor uses this list.
   std::map<std::string, std::shared_ptr<WsfAttributeContainer>> mBlackboards; ///< Mapping of names to blackboards. This
                                                                               ///< is a container for blackboards to be shared.
   WsfScriptProcessor* mParentPtr; ///< Pointer to the parent script processor of this tree.
   std::shared_ptr<WsfAdvancedBehaviorTreeCompositeNode> mRootNodePtr; ///< Pointer to the root node of this tree.
   bool                mShouldOutput; ///< True if this tree should output to the event pipe next tick, false otherwise.
   bool                mOutputForBTT{false};           ///< True if this tree should output to the event pipe at all.
   unsigned int        mId;                            ///< Unique integer id of this tree
   std::string         mName;                          ///< Name of the tree
   RootNodeType        mRootNodeType;                  ///< Root node type (stored for cloning)
   BT::SuccessPolicy   mSuccessPolicy{BT::cTHRESHOLD}; ///< Success Policy (stored for cloning parallel nodes)
   unsigned int        mThreshold{1};                  ///< Threshold (stored for cloning parallel nodes)
   static unsigned int mNextUniqueId;                  ///< Counter for unique tree ids
};

#endif // WSFADVANCEDBEHAVIORTREE_HPP
