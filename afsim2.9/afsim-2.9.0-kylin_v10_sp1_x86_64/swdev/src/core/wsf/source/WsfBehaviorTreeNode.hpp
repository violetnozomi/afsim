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

#ifndef WSFBEHAVIORTREENODE_HPP
#define WSFBEHAVIORTREENODE_HPP

#include <string>
#include <vector>

class UtInput;
class WsfBehaviorTree;
#include "wsf_export.h"
class WsfMessage;
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
class WsfPlatform;
class WsfSimulation;
class WsfScript;
class WsfScriptContext;
class WsfScriptMessageHandler;
#include "script/WsfScriptObjectClass.hpp"
class WsfScriptProcessor;
class WsfScriptStateMachine;

//! WsfBehaviorTreeNode is the base class for all nodes in a
//! WsfBehaviorTree, including connector nodes and leaf nodes
//! behavior names are WsfObject types; behavior types are WsfObject names
//! because behavior names are all unique, and types are all "behavior"
class WSF_EXPORT WsfBehaviorTreeNode : public WsfObject
{
   friend class WsfBehaviorTreeLeafNode;
   friend class WsfBehaviorTreeSequenceNode;
   friend class WsfBehaviorTreeSelectorNode;
   friend class WsfBehaviorTreeParallelNode;
   friend class WsfBehaviorTreePrioritySelectorNode;
   friend class WsfBehaviorTreeWeightedRandomNode;

public:
   enum RunType
   {
      Repeat,
      ForTime,
      UntilDone
   };

   enum SelectType
   {
      Continous,
      Finite
   };

   WsfBehaviorTreeNode(const WsfScenario& aScenario);
   WsfBehaviorTreeNode(const WsfBehaviorTreeNode& aSrc);
   WsfBehaviorTreeNode& operator=(const WsfBehaviorTreeNode&) = delete;
   ~WsfBehaviorTreeNode() override;
   WsfObject* Clone() const override = 0;

   bool         ProcessInput(UtInput& aInput) override;
   virtual void RegisterInput(UtInput& aInput) {}

   static unsigned int GetUniqueNodeId(void) { return ++mNextUniqueId; }
   static void         Reset() { mNextUniqueId = 0; }

   bool   PreconditionsMet();
   double PreconditionValue();

   void                               AddChild(WsfBehaviorTreeNode* aNodePtr);
   std::vector<WsfBehaviorTreeNode*>& Children() { return mChildren; }
   int                                ChildCount() const { return (int)mChildren.size(); }
   WsfBehaviorTreeNode*               ChildEntry(int aIndex);
   void                               SetParent(WsfBehaviorTreeNode* aNodePtr) { mParentPtr = aNodePtr; }
   WsfBehaviorTreeNode*               Parent() { return mParentPtr; }

   // to be done during standard initialization
   virtual bool Initialize(double aSimTime, WsfScriptContext* aParentContext);

   WsfScriptContext& GetScriptContext() { return *mContextPtr; }

   bool Execute(double aSimTime,
                bool   aSkipCheck = false); // does record keeping, calls LocalExecute()
   void TurnOn() { mTurnedOn = true; }
   void TurnOff() { mTurnedOn = false; }

   void        SetFailureReason(const std::string& aReason, bool aSendEvent = false);
   std::string FailureReason() const { return mFailureReason; }

   bool ExecutedLastRun() { return mLastExecute; }

   void SetExecutedLastRun(bool aVal);
   void SetExecutedLastRunRecursive(bool aVal);
   void SetExecutedLastRun(bool aVal, const std::string& aReason);
   void SetExecutedLastRunRecursive(bool aVal, const std::string& aReason);

   WsfBehaviorTree*    OwningTree();
   virtual void        SetTree(WsfBehaviorTree* aTreePtr);
   WsfScriptProcessor* OwningProcessor();
   WsfPlatform*        OwningPlatform();
   WsfSimulation*      GetSimulation();

   bool         TryRunLast(double aSimTime);
   bool         ShouldRunNext(double aSimTime);
   virtual bool LocalExecute(double aSimTime) = 0; // node type specific execution

   unsigned int Id() const { return mId; }

   void LogNodeStructure(double aSimTime, bool aRecurse);

   std::string FilePath() const { return mFullPathFileName; }
   time_t      FileLastModTime() const { return mFileLastModTime; }
   void        SetFilePath(const std::string& aPath);
   void        SetFileInfo(const std::string& aPath, time_t aModTime);

   WsfBehaviorTreeNode* FindNode(const std::string& aNodeName);
   void                 FindLastExecuted(std::vector<WsfBehaviorTreeNode*>& aLastExecuted);

   const char*        GetScriptClassName() const override;
   UtScriptContext*   GetScriptAccessibleContext() const override;
   const WsfScenario& GetScenario() const { return mScenario; }

   virtual bool ProcessMessage(double aSimTime, const WsfMessage& aMessage);

protected:
   const WsfScenario&                mScenario;
   bool                              mInitialized;
   bool                              mTurnedOn;
   WsfBehaviorTree*                  mParentTreePtr;
   WsfBehaviorTreeNode*              mParentPtr;
   std::vector<WsfBehaviorTreeNode*> mChildren;
   WsfScriptContext*                 mContextPtr;
   UtScript*                         mPreconditionScriptPtr;
   UtScript*                         mNewExecuteScriptPtr;
   UtScript*                         mNewFailScriptPtr;
   std::string                       mFailureReason;
   double                            mFailureTime;
   bool                              mLastExecute;
   unsigned int                      mId;
   std::string                       mFullPathFileName;
   time_t                            mFileLastModTime;
   RunType                           mRunType;
   int                               mRepeats;
   double                            mRunDuration;
   WsfBehaviorTreeNode*              mLastNodeSelectedPtr;
   int                               mRepeatsLeft;
   double                            mRunStartTime;
   SelectType                        mSelectType;
   int                               mSelectLimit;
   int                               mSelectCount;
   WsfScriptStateMachine*            mFiniteStateMachinePtr;
   int                               mStateIndex;
   WsfScriptMessageHandler*          mMessageHandlerPtr;
   bool                              mDebug;

private:
   static unsigned int mNextUniqueId;
};

class WsfBehaviorTreeNodeTypes : public WsfObjectTypeList<WsfBehaviorTreeNode>
{
public:
   WsfBehaviorTreeNodeTypes(WsfScenario& aScenario);

   LoadResult LoadType(UtInput& aInput) override;
};

class WSF_EXPORT WsfBehaviorTreeLeafNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreeLeafNode(WsfScenario& aScenario,
                           bool         aRegisterPlatformVariable  = true,
                           bool         aRegisterProcessorVariable = true);
   WsfBehaviorTreeLeafNode(const WsfBehaviorTreeLeafNode& aSrc);
   WsfBehaviorTreeLeafNode& operator=(const WsfBehaviorTreeLeafNode& aRhs) = delete;
   ~WsfBehaviorTreeLeafNode() override                                     = default;

   WsfObject* Clone() const override;
   bool       ProcessInput(UtInput& aInput) override;
   void       RegisterInput(UtInput& aInput) override; // allows behavior to know what file it was defined in
   bool       Initialize(double aSimTime, WsfScriptContext* aParentContextPtr) override;
   bool       LocalExecute(double aSimTime) override;
   void       SetTree(WsfBehaviorTree* aTreePtr) override;

protected:
   bool RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName);

private:
   UtScript* mOnInitScriptPtr;
   UtScript* mExecuteScriptPtr;
};

class WSF_EXPORT WsfScriptBehaviorTreeNodeClass : public WsfScriptObjectClass
{
public:
   WsfScriptBehaviorTreeNodeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   UtScriptContext* GetContext(void* aObjectPtr) override;
   //// Each of the exported methods are defined as function objects.
   UT_DECLARE_SCRIPT_METHOD(Id);
   UT_DECLARE_SCRIPT_METHOD(Failure);
   UT_DECLARE_SCRIPT_METHOD(Executed);
   UT_DECLARE_SCRIPT_METHOD(Parent);
   UT_DECLARE_SCRIPT_METHOD(ChildCount);
   UT_DECLARE_SCRIPT_METHOD(ChildEntry);
   UT_DECLARE_SCRIPT_METHOD(TurnOff);
   UT_DECLARE_SCRIPT_METHOD(TurnOn);
};

//############################################################################
//#
//#   Sequence Nodes perform each of their children until one of them
//#   returns failure.  Opposite of Selector Nodes.
//#
//############################################################################
class WSF_EXPORT WsfBehaviorTreeSequenceNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreeSequenceNode(WsfScenario& aScenario);
   WsfObject* Clone() const override;
   bool       LocalExecute(double aSimTime) override;
};

//############################################################################
//#
//#   Selector Nodes perform each of their children until one of them
//#   returns success.  Opposite of Sequence Nodes.
//#
//############################################################################
class WSF_EXPORT WsfBehaviorTreeSelectorNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreeSelectorNode(WsfScenario& aScenario);
   WsfObject* Clone() const override;
   bool       LocalExecute(double aSimTime) override;
};

//############################################################################
//#
//#   Parallel Nodes perform each of their children, no matter what.
//#
//############################################################################
class WSF_EXPORT WsfBehaviorTreeParallelNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreeParallelNode(const WsfScenario& aScenario);
   WsfObject* Clone() const override;
   bool       LocalExecute(double aSimTime) override;
};

//############################################################################
//#
//#   PrioritySelector Nodes perform their highest valued child.
//#   Ties are resolved by performing the 1st attached child in the tie.
//#
//############################################################################
class WSF_EXPORT WsfBehaviorTreePrioritySelectorNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreePrioritySelectorNode(WsfScenario& aScenario);
   WsfObject* Clone() const override;
   bool       LocalExecute(double aSimTime) override;
};

//############################################################################
//#
//#   WeightedRandom Nodes perform a randomly selected child.
//#   The random selection is weighted, according to the child values.
//#   If a child has value of zero, it is guaranteed not to run.
//#
//############################################################################
class WSF_EXPORT WsfBehaviorTreeWeightedRandomNode : public WsfBehaviorTreeNode
{
public:
   WsfBehaviorTreeWeightedRandomNode(WsfScenario& aScenario);
   WsfObject* Clone() const override;
   bool       LocalExecute(double aSimTime) override;
};

#endif
