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

#ifndef WSFADVANCEDBEHAVIORTREENODE_HPP
#define WSFADVANCEDBEHAVIORTREENODE_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

#include "UtColor.hpp"
#include "WsfEventPipeClasses.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "script/WsfScriptObjectClass.hpp"

class UtInput;
class WsfAdvancedBehaviorTree;
class WsfAttributeContainer;
class WsfMessage;
class WsfPlatform;
class WsfScript;
class WsfScriptContext;
class WsfScriptMessageHandler;
class WsfScriptProcessor;
class WsfScriptStateMachine;
class WsfSimulation;

namespace BT
{
/** The return status of a node at a given time step (tick).
 * SUCCESS - The node has completed running.
 * FAILURE - The node has determined it will not be able to complete its task.
 * RUNNING - The node has successfully moved forward during this tick, has not finished yet.
 * IDLE - The node has not run yet.
 * HALTED - The node has been halted by its parent.
 * Note - these must match the numbers in the wsf.utpack enum NodeExecState
 */
enum ReturnStatus
{
   cRUNNING  = 1,
   cSUCCESS  = 2,
   cFAILURE  = 3,
   cIDLE     = 4,
   cHALTED   = 5,
   cDISABLED = 6
};

/** Success policy for a Parallel composite node.
 * SUCCEED_ON_ONE - The node will return success as soon as one of its children succeeds.
 * SUCCEED_ON_ALL - All of the node's children must succeed before it returns success.
 * THRESHOLD      - The node will return success when enough children succeed
 */
enum SuccessPolicy
{
   cSUCCEED_ON_ONE,
   cSUCCEED_ON_ALL,
   cTHRESHOLD
};

/** Failure policy for a Parallel composite node.
 * FAIL_ON_ONE - The node will return failure as soon as one of its children fails.
 * FAIL_ON_ALL - All of the node's children must fail before it returns failure.
 */
enum FailurePolicy
{
   cFAIL_ON_ONE,
   cFAIL_ON_ALL
};

/** Reset policy for nodes with memory.
 * ON_SUCCESS_OR_FAILURE - Memory will reset on success or failure of a child node.
 * ON_SUCCESS - Memory will reset on success of a child node.
 * ON_FAILURE - Memory will reset on failure of a child node.
 */
enum ResetPolicy
{
   cON_SUCCESS_OR_FAILURE,
   cON_SUCCESS,
   cON_FAILURE
};
} // namespace BT

class WSF_EXPORT WsfScriptAdvancedBehaviorTreeNodeClass : public WsfScriptObjectClass
{
public:
   WsfScriptAdvancedBehaviorTreeNodeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   /** Return a pointer to the script context for an object.
    * @param aObjectPtr object to get context for.
    */
   UtScriptContext* GetContext(void* aObjectPtr) override;

   // Script methods
   // Each of the exported methods are defined as function objects.
   UT_DECLARE_SCRIPT_METHOD(Failure_1);
   UT_DECLARE_SCRIPT_METHOD(Failure_2);
   UT_DECLARE_SCRIPT_METHOD(Type);
   UT_DECLARE_SCRIPT_METHOD(Id);
   UT_DECLARE_SCRIPT_METHOD(Running_1);
   UT_DECLARE_SCRIPT_METHOD(Running_2);
   UT_DECLARE_SCRIPT_METHOD(Success_1);
   UT_DECLARE_SCRIPT_METHOD(Success_2);
   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);
   UT_DECLARE_SCRIPT_METHOD(State);
   UT_DECLARE_SCRIPT_METHOD(Tree);

   // Blackboard methods
   UT_DECLARE_SCRIPT_METHOD(BlackboardVarExists);
   UT_DECLARE_SCRIPT_METHOD(CreateSharedBlackboard);
   UT_DECLARE_SCRIPT_METHOD(DeleteBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(DeleteSharedBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(GetBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(GetSharedBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(SetBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(SetBlackboardTrackVar);
   UT_DECLARE_SCRIPT_METHOD(SetBlackboardPlatformVar);
   UT_DECLARE_SCRIPT_METHOD(SetSharedBlackboardVar);
   UT_DECLARE_SCRIPT_METHOD(SetSharedBlackboardTrackVar);
   UT_DECLARE_SCRIPT_METHOD(SetSharedBlackboardPlatformVar);
   UT_DECLARE_SCRIPT_METHOD(ShareBlackboard);
   UT_DECLARE_SCRIPT_METHOD(ShareBlackboardWithName);
   UT_DECLARE_SCRIPT_METHOD(SharedBlackboardExists);
   UT_DECLARE_SCRIPT_METHOD(SharedBlackboardVarExists);
};

class WSF_EXPORT WsfAdvancedBehaviorTreeNode : public WsfObject
{
public:
   WsfAdvancedBehaviorTreeNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeNode(const WsfAdvancedBehaviorTreeNode& aSrc);
   ~WsfAdvancedBehaviorTreeNode() override;
   WsfAdvancedBehaviorTreeNode& operator=(const WsfAdvancedBehaviorTreeNode&) = delete;

   /** Add a child to this node.
    * @param aNodePtr child node to add.
    */
   virtual void AddChild(std::shared_ptr<WsfAdvancedBehaviorTreeNode> aNodePtr) {}

   /** Add a tree child to this node.
    * @param aTreePtr tree to add.
    */
   virtual void AddTree(ut::CloneablePtr<WsfAdvancedBehaviorTree>& aTreePtr) {}

   /** Return a list of current children held by this node. */
   virtual std::vector<std::shared_ptr<WsfAdvancedBehaviorTreeNode>>& GetChildren()
   {
      throw std::logic_error("This node doesn't have children!");
   }

   /** Return a list of current children trees held by this node. */
   virtual std::map<size_t, ut::CloneablePtr<WsfAdvancedBehaviorTree>>& GetTrees()
   {
      throw std::logic_error("This node doesn't have children trees!");
   }

   /** Output this nodes children trees structures to the event pipe. */
   virtual void OutputTreeStructures(const double aSimTime) {}

   /** Output this nodes tree states to the event pipe if they are outputting. */
   virtual void OutputTreeStates(const double aSimTime) {}

   /** Clone this node. */
   WsfAdvancedBehaviorTreeNode* Clone() const override = 0;

   /** Get the depth of this node. */
   virtual const int GetDepth() const { return 0; }

   /** Return the last time the script this node was declared in was modified. */
   const time_t& GetFileLastModTime() const { return mFileLastModTime; }

   /** Return the file path to the script this node was declared in. */
   const std::string& GetFilePath() const { return mFullPathFileName; }

   /** Return a pointer to the blackboard for this node. */
   std::shared_ptr<WsfAttributeContainer> GetBlackboard() { return mBlackboard; }

   /** Get the name of this node. */
   const std::string& GetNodeName() const { return mName; }

   /** Set the name of this node.
    * @param aName name to set
    */
   void SetNodeName(const std::string& aName);

   /** Get the description of this node. */
   const std::string& GetDescription() const { return mDescription; }

   /** Set the description of this node.
    * @param aDesc description to set
    */
   void SetDescription(const std::string& aDesc) { mDescription = aDesc; }

   /** Get the ReturnStatus of this node. */
   const BT::ReturnStatus& GetNodeStatus() const { return mReturnStatus; }

   /** Return the current scenario. */
   const WsfScenario& GetScenario() const { return mScenario; }

   /** Return the script class name for this node. */
   const char* GetScriptClassName() const override;

   /** Returns a reference to the script context for this node. */
   const WsfScriptContext& GetScriptContext() const { return *mContextPtr; }

   /** Return a pointer to the simulation. */
   WsfSimulation* GetSimulation() const;

   /** Return the current precondition tooltip text for this node. */
   const std::string& GetPreconditionTooltip() const { return mPreconditionTooltip; }

   /** Return the current execute tooltip text for this node. */
   const std::string& GetExecuteTooltip() const { return mExecuteTooltip; }

   /** Returns if this node is on. */
   const bool& GetTurnedOn() const { return mTurnedOn; };

   /** Get a unique int id for this node. */
   static unsigned int GetUniqueNodeId() { return ++mNextUniqueId; }

   /** Halt the execution of this node.
    * @param aReason reason for halting.
    */
   virtual void Halt(const std::string& aReason) {}

   /** Halt the execution of this node's children starting at an index. Helper function for Halt().
    * @param aIndex index to halt up to.
    * @param aReason reason for halting.
    */
   virtual void HaltChildren(unsigned int aIndex, const std::string& aReason) {}

   /** Get if this node can have children. */
   virtual const bool HasChildrenContainer() const { return false; };

   /** Get if this node has a parent. */
   const bool& HasParent() const { return mHasParent; }

   /** Return the unique id for this node. */
   const unsigned int& Id() const { return mId; }

   /** Return this node's color. Used exclusively for visualization. */
   const UtColor& GetColor() const { return mColor; }

   /** Return the string representation of this node's color. Used exclusively for visualization. */
   bool HasDefinedColor() const { return mHasDefinedColor; }

   /** Initializes this node.
    * @param aSimTime time to initialize the node.
    * @param aParentContextPtr script context pointer that will populate data for the node.
    */
   virtual bool Initialize(double aSimTime, WsfScriptContext* aParentContextPtr);

   /** Return a pointer to the owning platform of this node. */
   WsfPlatform* GetOwningPlatform() const;

   /** Return a pointer to the owning processor of this node. */
   WsfScriptProcessor* GetOwningProcessor() const;

   /** Return a pointer to the owning tree of this node. */
   WsfAdvancedBehaviorTree* GetOwningTree() const;

   /** Returns a pointer to the current parent of this node. */
   WsfAdvancedBehaviorTreeNode* GetParent() const { return mParentPtr; }

   /** Returns a boolean representing if the precondition ran successfully or not. */
   const bool PreconditionsMet() const;

   /** Returns a fitness value representing how well the precondition ran.
    * This will be used to determine which node has the highest
    * priority to execute or which node has the highest weight.
    */
   virtual const double PreconditionValue() const { return 0.0; }

   /** Process input from a script.
    * @param aInput input to parse.
    */
   bool ProcessInput(UtInput& aInput) override;

   /** Process a script message.
    * @param aSimTime sim time at time of processing.
    * @param aMessage message to process.
    */
   virtual bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

   /** Register a file path and last modified time from an input.
    * @param aInput input to parse.
    */
   virtual void RegisterInput(UtInput& aInput) {}

   /** Reset the unique int counter. */
   static void Reset() { mNextUniqueId = 0; }

   /** Reset the precondition ran and met var to false at
     * the beginning of the tree tick.
     */
   virtual void ResetPreconditionVars() {}

   /** Set the file path for this node.
    * @param aPath file path to use.
    */
   void SetFilePath(const std::string& aPath);

   /** Set if this node has a parent.
    * @param aValue value to set
    */
   void SetHasParent(const bool aValue);

   /** Set the parent of this node.
    * @param aNodePtr node to set as parent.
    */
   void SetParent(WsfAdvancedBehaviorTreeNode* aNodePtr) { mParentPtr = aNodePtr; }

   /** Set the tooltip text for this node.
    * @param aText reason for failure.
    * @param aSendEvent should we send an event when text is set?
    */
   void SetTooltip(const std::string& aText, const bool aSendEvent = false);

   /** Set the tooltip text for this node.
    * @param aText reason for failure.
    * @param aSendEvent should we send an event when text is set?
    */
   void SetPreconditionTooltip(const std::string& aText, const bool aSendEvent = false);

   /** Set the tooltip text for this node.
    * @param aText reason for failure.
    * @param aSendEvent should we send an event when text is set?
    */
   void SetExecuteTooltip(const std::string& aText, const bool aSendEvent = false);

   /** Set the owning tree for this node.
    * @param aTreePtr the tree that will own(parent) this node.
    */
   virtual void SetTree(WsfAdvancedBehaviorTree* aTreePtr){};

   /** Tick forward and update the execution of this node. */
   virtual BT::ReturnStatus Tick();

   /** This is the functionality ran by each node, e.g how the
     * node handles its tick.
     */
   virtual BT::ReturnStatus TickFunction() { return BT::cIDLE; }

   /** Turns this node off. */
   virtual void TurnOff(){};

   /** Turns this node on. */
   virtual void TurnOn(){};

   bool ExecutedLastTick() const { return mExecutedLastTick; }

   /** Find the node based on the script-configured name.
    * @param aNodeName node name to find.
    * @return The pointer to the node is returned if found otherwise a nullptr is returned.
    */
   WsfAdvancedBehaviorTreeNode* FindNode(const std::string& aNodeName);

   /** Find and return all nodes exectued last tick.
    * @param aLastExecuted container to fill with node pointers.
    */
   void FindLastExecuted(std::vector<WsfAdvancedBehaviorTreeNode*>& aLastExecuted);

   /** Set if this node is the root node for a tree.
    * @param aState true to set the node as a root node, false otherwise.
    */
   void SetIsTreeRootNode(const bool aState) { mIsTreeRootNode = aState; }

   /** Get if this tree is a root node for a tree. */
   bool GetIsTreeRootNode() const { return mIsTreeRootNode; }

   WsfScriptStateMachine* ChildStateMachine() { return mFiniteStateMachinePtr; }

   /** Get the top-most root node (ignore nodes considered as tree root nodes). */
   WsfAdvancedBehaviorTreeNode* GetTopRootNode();

   /** Return a pointer to the execute script for this node. */
   UtScript* GetExecuteScript() { return mExecuteScriptPtr; }

   void SetInitScript(UtScript* aScriptPtr);

protected:
   /** Register an external variable.
    * @param aVariableType variable type to register.
    * @param aVariableName name of the variable.
    */
   bool RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName);

   /** Return the context from the script. */
   UtScriptContext* GetScriptAccessibleContext() const override;

   /** Set the file path and last mod time for this node.
    * @param aPath file path to use.
    * @param aModTime last modified time of the file.
    */
   void SetFileInfo(const std::string& aPath, const time_t& aModTime);

   /** Set the ReturnStatus of this node.
    * @param aStatus ReturnStatus to set
    */
   void SetNodeStatus(const BT::ReturnStatus aStatus);

   //* Unique integer id of this node
   unsigned int mId{0};

   //* Scenario of this node
   const WsfScenario& mScenario;

   //* Blackboard for writing and reading script variables
   std::shared_ptr<WsfAttributeContainer> mBlackboard;

   //* Pointer to the script context of this node
   WsfScriptContext* mContextPtr{nullptr};

   //* Pointer to the script message handler for this node
   WsfScriptMessageHandler* mMessageHandlerPtr{nullptr};

   //* Pointer to the parent node of this node
   WsfAdvancedBehaviorTreeNode* mParentPtr{nullptr};

   //* Pointer to the tree that parents this node
   WsfAdvancedBehaviorTree* mParentTreePtr{nullptr};

   //* Pointer to the finite state machine for this node.
   WsfScriptStateMachine* mFiniteStateMachinePtr{nullptr};

   //* Integer representing the current state the finite state machine is in
   int mStateIndex{0};

   //* Timestamp representing the last time the script file this node was declared in was modified
   time_t mFileLastModTime;

   //* True if debugging is turned on for this node, false otherwise
   bool mDebug{false};

   //* True if this node is initialized, false otherwise
   bool mInitialized{false};

   //* True if this node is turned on, false otherwise
   bool mTurnedOn{true};

   //* Consider this node as another tree?
   bool mIsTreeRootNode{false};

   //* True if this node has a parent, false otherwise
   bool mHasParent{false};

   //* True if the node ran the execute block last tick.  This is used for on_new_failure and on_new_execute.
   bool mExecutedLastTick{false};

   //* True if the node is in its precondition block, false otherwise.  Used to determine which tooltip from script funcs.
   mutable bool mInPrecondition{false};

   //* True if the precondition has run for the node this tick, false otherwise.
   mutable bool mPreconditionRan{false};

   //* True if the node's precondition has been met(>0.0), false otherwise.  Used in determining whether or not to tick a node
   //* when it's precondition has been skipped for selectors, selectors with memory, and parallel composite nodes.
   mutable bool mPreconditionMet{false};

   //* ReturnStatus of the node
   BT::ReturnStatus mReturnStatus{BT::cIDLE};

   //* Full file path to the script file this node was declared in
   std::string mFullPathFileName{""};

   //* Name of the node
   std::string mName{""};

   //* Description of the node
   std::string mDescription{""};

   //* Tooltip text for the precondition script block
   std::string mPreconditionTooltip{""};

   //* Tooltip text fsor the execute script block
   std::string mExecuteTooltip{""};

   //* Time representing when the tooltip text was set
   double mPreconditionTooltipTimestamp{-1.0};

   //* Time representing when the tooltip text was set
   double mExecuteTooltipTimestamp{-1.0};

   //* Pointer to the initialization script for this node
   UtScript* mOnInitScriptPtr{nullptr};

   //* Pointer to a new execute script
   UtScript* mNewExecuteScriptPtr{nullptr};

   //* Pointer to a new fail script
   UtScript* mNewFailScriptPtr{nullptr};

   //* Pointer to the precondition script of this node
   UtScript* mPreconditionScriptPtr{nullptr};

   //* Pointer to the execute script for this node
   UtScript* mExecuteScriptPtr{nullptr};

   //* The color to set Platform History trace lines
   UtColor mColor;

   //* True if the color was defined through user input, false otherwise
   bool mHasDefinedColor;

private:
   //* Counter for unique node ids
   static unsigned int mNextUniqueId;
};

class WsfAdvancedBehaviorTreeNodeTypes : public WsfObjectTypeList<WsfAdvancedBehaviorTreeNode>
{
public:
   WsfAdvancedBehaviorTreeNodeTypes(WsfScenario& aScenario);

   /** Load a new type object from an input stream.
    * @param aInput input to read from.
    */
   LoadResult LoadType(UtInput& aInput) override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeLeafNode : public WsfAdvancedBehaviorTreeNode
{
public:
   WsfAdvancedBehaviorTreeLeafNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeLeafNode(const WsfAdvancedBehaviorTreeLeafNode& aSrc);
   WsfAdvancedBehaviorTreeLeafNode& operator=(const WsfAdvancedBehaviorTreeLeafNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeLeafNode* Clone() const override;

   /** Return the depth of this node. Leaf nodes have no children, so depth will always be 0. */
   const int GetDepth() const override { return 0; }

   /** Halt the execution of this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Returns a fitness value representing how well the precondition ran.
    * This will be used to determine which node has the highest
    * priority to execute or which node has the highest weight.
    */
   const double PreconditionValue() const override;

   /** Process input from a script.
    * @param aInput input to parse.
    */
   bool ProcessInput(UtInput& aInput) override;

   /** Register a file path and last modified time from an input.
    * @param aInput input to parse.
    */
   void RegisterInput(UtInput& aInput) override;

   /** Turns this node off. */
   void TurnOff() override;

   /** Turns this node on. */
   void TurnOn() override;

   /** Reset the precondition ran and met var to false at
     * the beginning of the tree tick.
     */
   void ResetPreconditionVars() override;

protected:
   /** Set the owning tree for this node.
    * @param aTreePtr the tree that will own(parent) this node.
    */
   void SetTree(WsfAdvancedBehaviorTree* aTreePtr) override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeActionNode : public WsfAdvancedBehaviorTreeLeafNode
{
public:
   explicit WsfAdvancedBehaviorTreeActionNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeActionNode& operator=(const WsfAdvancedBehaviorTreeActionNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeActionNode* Clone() const override;

   /** Halt this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Set the owning tree for this node.
    * @param aTreePtr the tree that will own(parent) this node.
    */
   void SetTree(WsfAdvancedBehaviorTree* aTreePtr) override;

   /** Ticks the node. */
   BT::ReturnStatus Tick() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeConditionNode : public WsfAdvancedBehaviorTreeLeafNode
{
public:
   explicit WsfAdvancedBehaviorTreeConditionNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeConditionNode& operator=(const WsfAdvancedBehaviorTreeConditionNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeConditionNode* Clone() const override;

   /** Halt this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Tick this node.  Condition nodes only return success and failure (no running state). */
   BT::ReturnStatus Tick() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeCompositeNode : public WsfAdvancedBehaviorTreeNode
{
public:
   WsfAdvancedBehaviorTreeCompositeNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeCompositeNode(const WsfAdvancedBehaviorTreeCompositeNode& aSrc);
   ~WsfAdvancedBehaviorTreeCompositeNode() override = default;
   WsfAdvancedBehaviorTreeCompositeNode& operator=(const WsfAdvancedBehaviorTreeCompositeNode&) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeCompositeNode* Clone() const override;

   /** Add a child to this node.
    * @param aChildPtr child node to add.
    */
   void AddChild(std::shared_ptr<WsfAdvancedBehaviorTreeNode> aChildPtr) override;

   /** Add a tree child to this node.
    * @param aTreePtr tree to add.
    */
   void AddTree(ut::CloneablePtr<WsfAdvancedBehaviorTree>& aTreePtr) override;

   /** Return a list of current children held by this node. */
   std::vector<std::shared_ptr<WsfAdvancedBehaviorTreeNode>>& GetChildren() override { return mChildren; }

   /** Return a list of current children trees held by this node. */
   std::map<size_t, ut::CloneablePtr<WsfAdvancedBehaviorTree>>& GetTrees() override { return mTrees; }

   /** Output this nodes children trees structures to the event pipe. */
   void OutputTreeStructures(const double aSimTime) override;

   /** Output this nodes tree states to the event pipe if they are outputting. */
   void OutputTreeStates(const double aSimTime) override;

   /** Get the depth of this node recursively through it's children. */
   const int GetDepth() const override;

   /** Halt the execution of this node and it's children.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Get if this node can have children. */
   const bool HasChildrenContainer() const override { return true; };

   /** Initializes this node.
    * @param aSimTime time to initialize the node.
    * @param aParentContextPtr script context pointer that will populate data for the node.
    */
   bool Initialize(double aSimTime, WsfScriptContext* aParentContextPtr) override;

   /** Returns a fitness value representing how important this node is.
    * For a composite node, by default this returns the first child's weight.
    * Sequence and most decorators will use this.
    */
   const double PreconditionValue() const override;

   /** Set the owning tree for this node.
    * @param aTreePtr the tree that will own(parent) this node.
    */
   void SetTree(WsfAdvancedBehaviorTree* aTreePtr) override;

   /** Turns this node off, and it's children recursively. */
   void TurnOff() override;

   /** Turns this node on, and it's children recursively. */
   void TurnOn() override;

   /** Reset the precondition ran and met var to false at
     * the beginning of the tree tick, recursing through
     * all children nodes.
     */
   void ResetPreconditionVars() override;

protected:
   /** Get if any children are running. */
   const bool AreChildrenRunning() const;

   /** Get the last(rightmost) enabled child in this node's children. */
   const unsigned int GetLastEnabledChild() const;

   /** Get the number of enabled children. */
   const unsigned int GetNumEnabledChildren() const;

   /** Halt the execution of this node's children starting at an index. Helper function for Halt().
    * @param aIndex index to halt up to.
    * @param aReason reason for halting.
    */
   void HaltChildren(unsigned int aIndex, const std::string& aReason) override;

   /** Halt the execution of this node's children that are running.
    * @param aReason reason for halting.
    */
   void HaltRunning(const std::string& aReason);

   //* List of children nodes
   std::vector<std::shared_ptr<WsfAdvancedBehaviorTreeNode>> mChildren;

   //* Map of indices to trees, where the index is the slot of the root node in the children.
   std::map<size_t, ut::CloneablePtr<WsfAdvancedBehaviorTree>> mTrees;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeDecoratorNode : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeDecoratorNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeDecoratorNode& operator=(const WsfAdvancedBehaviorTreeDecoratorNode& aRhs) = delete;

   /** Add a child to this node. Decorator nodes may only have one child.
    * @param aChildPtr child node to add.
    */
   void AddChild(std::shared_ptr<WsfAdvancedBehaviorTreeNode> aChildPtr) override;

   /** Add a tree child to this node.
    * @param aTreePtr tree to add.
    */
   void AddTree(ut::CloneablePtr<WsfAdvancedBehaviorTree>& aTreePtr) override;

   /** Output this nodes children trees structures to the event pipe. */
   void OutputTreeStructures(const double aSimTime) override;

   /** Output this nodes tree states to the event pipe if they are outputting. */
   void OutputTreeStates(const double aSimTime) override;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeDecoratorNode* Clone() const override;

   /** Tick forward and update the execution of this node and its child. */
   BT::ReturnStatus TickFunction() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeInverterNode : public WsfAdvancedBehaviorTreeDecoratorNode
{
public:
   explicit WsfAdvancedBehaviorTreeInverterNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeInverterNode& operator=(const WsfAdvancedBehaviorTreeInverterNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeInverterNode* Clone() const override;

   /** Tick forward and update the execution of this node and its child. */
   BT::ReturnStatus TickFunction() override;

private:
   /** Invert success to failure or vice versa, do nothing otherwise.
    * @param aStatus Status to invert.
    */
   BT::ReturnStatus Convert(const BT::ReturnStatus aStatus);
};

class WSF_EXPORT WsfAdvancedBehaviorTreeSucceederNode : public WsfAdvancedBehaviorTreeDecoratorNode
{
public:
   explicit WsfAdvancedBehaviorTreeSucceederNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeSucceederNode& operator=(const WsfAdvancedBehaviorTreeSucceederNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeSucceederNode* Clone() const override;

   /** Tick forward and update the execution of this node, and its child.  Succeeder nodes always return success. */
   BT::ReturnStatus TickFunction() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeNegatorNode : public WsfAdvancedBehaviorTreeDecoratorNode
{
public:
   explicit WsfAdvancedBehaviorTreeNegatorNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeNegatorNode& operator=(const WsfAdvancedBehaviorTreeNegatorNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeNegatorNode* Clone() const override;

   /** Returns a fitness value representing how important this node is.
    * For a Negator, the node below doesn't matter, return a weight of 0.0.
    */
   const double PreconditionValue() const override { return 0.0; }

   /** Tick forward and update the execution of this node, and its child. Negator nodes always return failure. */
   BT::ReturnStatus TickFunction() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeRepeaterNode : public WsfAdvancedBehaviorTreeDecoratorNode
{
public:
   /** Run Type of the node.
    * Repeat - Execute script will run a specified number of times
    * ForTime - Execute script will keep running for an amount of time
    * UntilDone - Execute script will run until finished
    */
   enum RunType
   {
      Repeat,
      ForTime,
      UntilDone
   };

   explicit WsfAdvancedBehaviorTreeRepeaterNode(const WsfScenario& aScenario);
   WsfAdvancedBehaviorTreeRepeaterNode& operator=(const WsfAdvancedBehaviorTreeRepeaterNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeRepeaterNode* Clone() const override;

   /** Returns a reference to the number of repeats. */
   int& GetRepeats() { return mRepeats; }

   /** Returns a reference to the run duration. */
   double& GetRunDuration() { return mRunDuration; }

   /** Get the run type for this repeater node. */
   RunType& GetRunType() { return mRunType; }

   /** Sets the number of repeats for a repeating run type.
    * @param aRepeatCount number of times to repeat.
    */
   void SetRepeats(const int aRepeatCount);

   /** Set the run type for this repeater node.
    * @param aRunType run type to set.
    */
   void SetRunType(const RunType aRunType);

   /** Tick forward and update the execution of this node, and its child. Negator nodes always return failure. */
   BT::ReturnStatus TickFunction() override;

protected:
   //* How many times to repeat this node
   int mRepeats;

   //* Number of repeats left before defaulting to failure
   int mRepeatsLeft;

   //* How long to run this node before defaulting to failure
   double mRunDuration;

   //* Time representing when the node started running (used for checking time)
   double mRunStartTime;

   //* Run type of this node (Repeater Decorator)
   RunType mRunType;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeParallelNode : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeParallelNode(const WsfScenario& aScenario);
   explicit WsfAdvancedBehaviorTreeParallelNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
                                                BT::SuccessPolicy  aSuccessPolicy = BT::cSUCCEED_ON_ONE,
                                                const unsigned int aThreshold     = 0);
   WsfAdvancedBehaviorTreeParallelNode& operator=(const WsfAdvancedBehaviorTreeParallelNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeParallelNode* Clone() const override;

   /** Halt the execution of this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Returns a fitness value representing how important this node is.
    * For a Parallel node, all children get executed at once, so we
    * should check all children for their weights, returning the sum.
    */
   const double PreconditionValue() const override;

   /** Process input from a script.
    * @param aInput input to parse.
    */
   bool ProcessInput(UtInput& aInput) override;

   /** Tick forward and update the execution of this node and its children. */
   BT::ReturnStatus TickFunction() override;

private:
   /** Get the success policy for this node. */
   BT::SuccessPolicy GetSuccessPolicy() const { return mPolicy; }

   /** Get the threshold(number) of children that must succeed for this node to succeed. */
   const unsigned int& GetThreshold() const { return mThreshold; }

   /** Set the success policy for this node.
    * @param aPolicy policy to use when evaluating for success
    */
   void SetSuccessPolicy(const BT::SuccessPolicy aPolicy);

   /** Set the threshold(number) of children that must succeed for this node to succeed.
    * @param aThreshold the threshold that must be hit for this node to succeed.
    */
   void SetThreshold(const unsigned int aThreshold);

   //* Number of children that have failed
   unsigned int mNumChildrenFailure = 0;

   //* Number of children that have succeeded
   unsigned int mNumChildrenSuccess = 0;

   //* Policy for handling when a parallel node should return success
   BT::SuccessPolicy mPolicy = BT::cTHRESHOLD;

   //* Threshold needed for this node to succeed
   unsigned int mThreshold = 1;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeSelectorNode : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeSelectorNode(const WsfScenario& aScenario);
   explicit WsfAdvancedBehaviorTreeSelectorNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode);
   WsfAdvancedBehaviorTreeSelectorNode& operator=(const WsfAdvancedBehaviorTreeSelectorNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeSelectorNode* Clone() const override;

   /** Returns a fitness value representing how important this node is.
    * For a Selector, check all children from left to right for a weight
    * above 0.0, and return it.  Otherwise, return 0.0.
    */
   const double PreconditionValue() const override;

   /** Tick forward and update the execution of this node, and its children. */
   BT::ReturnStatus TickFunction() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeSelectorNodeWithMemory : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeSelectorNodeWithMemory(const WsfScenario& aScenario, BT::ResetPolicy aResetPolicy);
   explicit WsfAdvancedBehaviorTreeSelectorNodeWithMemory(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
                                                          BT::ResetPolicy                             aResetPolicy);
   WsfAdvancedBehaviorTreeSelectorNodeWithMemory& operator=(const WsfAdvancedBehaviorTreeSelectorNodeWithMemory& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeSelectorNodeWithMemory* Clone() const override;

   /** Halt the execution of this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Returns a fitness value representing how important this node is.
    * For a Selector, check all children from left to right for a weight
    * above 0.0, and return it.  Otherwise, return 0.0.
    */
   const double PreconditionValue() const override;

   /** Tick forward and update the execution of this node, and its children. */
   BT::ReturnStatus TickFunction() override;

private:
   //* Current child we are skipping to
   unsigned int mCurrentChildIndex;

   //* Reset policy for this Sequence
   BT::ResetPolicy mResetPolicy;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeSequenceNode : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeSequenceNode(const WsfScenario& aScenario);
   explicit WsfAdvancedBehaviorTreeSequenceNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode);
   WsfAdvancedBehaviorTreeSequenceNode& operator=(const WsfAdvancedBehaviorTreeSequenceNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeSequenceNode* Clone() const override;

   /** Tick forward and update the execution of this node, and its children. */
   BT::ReturnStatus TickFunction() override;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeSequenceNodeWithMemory : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeSequenceNodeWithMemory(const WsfScenario& aScenario, BT::ResetPolicy aResetPolicy);
   explicit WsfAdvancedBehaviorTreeSequenceNodeWithMemory(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode,
                                                          BT::ResetPolicy                             aResetPolicy);
   WsfAdvancedBehaviorTreeSequenceNodeWithMemory& operator=(const WsfAdvancedBehaviorTreeSequenceNodeWithMemory& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeSequenceNodeWithMemory* Clone() const override;

   /** Halt the execution of this node.
    * @param aReason reason for halting.
    */
   void Halt(const std::string& aReason) override;

   /** Tick forward and update the execution of this node, and it's children. */
   BT::ReturnStatus TickFunction() override;

private:
   //* Current child we are skipping to
   unsigned int mCurrentChildIndex;

   //* Reset policy for this Sequence
   BT::ResetPolicy mResetPolicy;
};

class WSF_EXPORT WsfAdvancedBehaviorTreePrioritySelectorNode : public WsfAdvancedBehaviorTreeSelectorNode
{
public:
   explicit WsfAdvancedBehaviorTreePrioritySelectorNode(const WsfScenario& aScenario);
   explicit WsfAdvancedBehaviorTreePrioritySelectorNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode);
   WsfAdvancedBehaviorTreePrioritySelectorNode& operator=(const WsfAdvancedBehaviorTreePrioritySelectorNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreePrioritySelectorNode* Clone() const override;

   /** Returns a fitness value representing how important this node is.
    * For a Priority Selector, since this node can execute any of its
    * children, we will need to check all children for their weights,
    * returning only the highest.
    */
   const double PreconditionValue() const override;

   /** Tick forward and update the execution of this node, and its children. */
   BT::ReturnStatus TickFunction() override;

private:
   /** Returns a pointer to the currently selected node. */
   WsfAdvancedBehaviorTreeNode* GetCurrentSelection() const { return mCurrentSelectionPtr; }

   /** Sets a pointer to the currently selected node.
    * @param aNodePtr node to set as currently selected
    */
   void SetCurrentSelection(WsfAdvancedBehaviorTreeNode* aNodePtr) { mCurrentSelectionPtr = aNodePtr; }

   //* Pointer to the currently selected node.  If a node is selected, the priority selector won't check weights on tick.
   mutable WsfAdvancedBehaviorTreeNode* mCurrentSelectionPtr;
};

class WSF_EXPORT WsfAdvancedBehaviorTreeWeightedRandomNode : public WsfAdvancedBehaviorTreeCompositeNode
{
public:
   explicit WsfAdvancedBehaviorTreeWeightedRandomNode(const WsfScenario& aScenario);
   explicit WsfAdvancedBehaviorTreeWeightedRandomNode(const WsfAdvancedBehaviorTreeCompositeNode& aCompositeNode);
   WsfAdvancedBehaviorTreeWeightedRandomNode& operator=(const WsfAdvancedBehaviorTreeWeightedRandomNode& aRhs) = delete;

   /** Clone this node. */
   WsfAdvancedBehaviorTreeWeightedRandomNode* Clone() const override;

   /** Returns a fitness value representing how important this node is.
    * For a Weighted Random, since this node can execute any of its
    * children, we will need to check all children for their weights,
    * returning an average of all weights.
    */
   const double PreconditionValue() const override;

   /** Tick forward and update the execution of this node, and its children. */
   BT::ReturnStatus TickFunction() override;

private:
   /** Return a pointer to a node chosen during a random draw. */
   const std::pair<double, WsfAdvancedBehaviorTreeNode*> GetRandomNode() const;

   //* Pointer to the weighted random node that was chosen on draw during the PreconditionValue() call
   mutable WsfAdvancedBehaviorTreeNode* mCurrentSelectionPtr;
};
#endif
