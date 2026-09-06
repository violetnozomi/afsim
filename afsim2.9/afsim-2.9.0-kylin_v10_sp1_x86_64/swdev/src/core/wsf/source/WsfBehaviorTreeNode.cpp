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

#include "WsfBehaviorTreeNode.hpp"

#include <iomanip>
#include <limits>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>

#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtRandom.hpp"
#include "WsfBehaviorObserver.hpp"
#include "WsfBehaviorTree.hpp"
#include "WsfMessage.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"
#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachine.hpp"

unsigned int WsfBehaviorTreeNode::mNextUniqueId = 0;

WsfBehaviorTreeNodeTypes::WsfBehaviorTreeNodeTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfBehaviorTreeNode>(aScenario, cSINGULAR_BASE_TYPE, "behavior")
{
   AddCoreType("WSF_BEHAVIOR", ut::make_unique<WsfBehaviorTreeLeafNode>(aScenario));
}

// =================================================================================================
//! Load a new 'type object' from an input stream.
//! Process the current command if it is one that defines a new 'type object' of this class.
//! @param aInput The input stream.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
// static
WsfBehaviorTreeNodeTypes::LoadResult WsfBehaviorTreeNodeTypes::LoadType(UtInput& aInput)
{
   LoadResult result;
   if (aInput.GetCommand() == "behavior")
   {
      result.mIsCommandProcessed = true;

      std::string behaviorName;
      std::string behaviorType;
      aInput.ReadValue(behaviorName);
      aInput.ReadValue(behaviorType);

      aInput.PushBack(behaviorName);

      // Specifying the type is optional. If it isn't specified, WSF_BEHAVIOR is used.
      if (!IsType(behaviorType))
      {
         // In this case the behavior is a behavior command, so push it back on to be handled in 'ProcessInput'
         aInput.PushBack(behaviorType);

         // Use the default behavior type
         behaviorType = "WSF_BEHAVIOR";
      }

      std::unique_ptr<WsfBehaviorTreeNode> behaviorPtr(Clone(behaviorType));
      if (behaviorPtr.get() != nullptr)
      {
         behaviorPtr->RegisterInput(aInput);
         behaviorPtr->SetType(behaviorType);
         behaviorPtr->ProcessInput(aInput);
         if (!Add(behaviorName, std::move(behaviorPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate behavior name: " + behaviorName);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown behavior type: " + behaviorType);
      }
   }
   return result;
}

WsfBehaviorTreeNode::WsfBehaviorTreeNode(const WsfScenario& aScenario)
   : mScenario(aScenario)
   , mInitialized(false)
   , mTurnedOn(true)
   , mParentTreePtr(nullptr)
   , mParentPtr(nullptr)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfBehaviorTreeNode"))
   , mPreconditionScriptPtr(nullptr)
   , mNewExecuteScriptPtr(nullptr)
   , mNewFailScriptPtr(nullptr)
   , mFailureReason()
   , mFailureTime(-1.0)
   , mLastExecute(false)
   , mRunType(Repeat)
   , mRepeats(1)
   , mRunDuration(0.0)
   , mLastNodeSelectedPtr(nullptr)
   , mRepeatsLeft(0)
   , mRunStartTime(-1)
   , mSelectType(Continous)
   , mSelectLimit(1)
   , mSelectCount(0)
   , mFiniteStateMachinePtr(nullptr)
   , mStateIndex(0)
   , mMessageHandlerPtr(nullptr)
   , mDebug(false)
{
   mId = GetUniqueNodeId();
   WsfObject::SetType(WsfStringId("undefined"));
   WsfObject::SetName(WsfStringId("undefined"));
}

WsfBehaviorTreeNode::WsfBehaviorTreeNode(const WsfBehaviorTreeNode& aSrc)
   : WsfObject(aSrc)
   , mScenario(aSrc.mScenario)
   , mInitialized(aSrc.mInitialized)
   , mTurnedOn(aSrc.mTurnedOn)
   , mParentTreePtr(nullptr)
   , mParentPtr(nullptr)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mPreconditionScriptPtr(nullptr)
   , mNewExecuteScriptPtr(nullptr)
   , mNewFailScriptPtr(nullptr)
   , mFailureReason(aSrc.mFailureReason)
   , mFailureTime(aSrc.mFailureTime)
   , mLastExecute(aSrc.mLastExecute)
   , mId(0)
   , mFullPathFileName(aSrc.mFullPathFileName)
   , mFileLastModTime(aSrc.mFileLastModTime)
   , mRunType(aSrc.mRunType)
   , mRepeats(aSrc.mRepeats)
   , mRunDuration(aSrc.mRunDuration)
   , mLastNodeSelectedPtr(aSrc.mLastNodeSelectedPtr)
   , mRepeatsLeft(aSrc.mRepeatsLeft)
   , mRunStartTime(aSrc.mRunStartTime)
   , mSelectType(aSrc.mSelectType)
   , mSelectLimit(aSrc.mSelectLimit)
   , mSelectCount(aSrc.mSelectCount)
   , mFiniteStateMachinePtr(nullptr)
   , // set below if aSrc has one to clone
   mStateIndex(aSrc.mStateIndex)
   , mMessageHandlerPtr(nullptr)
   , mDebug(aSrc.mDebug)
{
   mId = GetUniqueNodeId(); // each instance (copy) of a behavior node is unique

   mPreconditionScriptPtr = mContextPtr->FindScript("precondition");
   mNewExecuteScriptPtr   = mContextPtr->FindScript("on_new_execute");
   mNewFailScriptPtr      = mContextPtr->FindScript("on_new_fail");

   // clone the children from the source
   this->mChildren.clear();
   std::vector<WsfBehaviorTreeNode*>::const_iterator cit = aSrc.mChildren.begin();
   for (; cit != aSrc.mChildren.end(); ++cit)
   {
      WsfBehaviorTreeNode* nodePtr = *cit;
      this->AddChild(static_cast<WsfBehaviorTreeNode*>(nodePtr->Clone()));
   }
   if (aSrc.mFiniteStateMachinePtr != nullptr)
   {
      mFiniteStateMachinePtr = new WsfScriptStateMachine(*aSrc.mFiniteStateMachinePtr, *mContextPtr);
   }
   if (aSrc.mMessageHandlerPtr != nullptr)
   {
      mMessageHandlerPtr = new WsfScriptMessageHandler(*aSrc.mMessageHandlerPtr, *mContextPtr);
   }
}

WsfBehaviorTreeNode::~WsfBehaviorTreeNode()
{
   std::vector<WsfBehaviorTreeNode*>::iterator it = mChildren.begin();
   for (; it != mChildren.end(); ++it)
   {
      WsfBehaviorTreeNode* nodePtr = *it;
      delete nodePtr;
   }
   mChildren.clear();

   delete mFiniteStateMachinePtr;

   delete mContextPtr; // deleting this handles all script blocks compiled with it (init, execute, precondition, etc...)

   delete mMessageHandlerPtr; // TODO - delete before or after the context?
}

bool WsfBehaviorTreeNode::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (aInput.GetCommand() == "debug")
   {
      mDebug = true;
   }
   else if (aInput.GetCommand() == "precondition")
   {
      // use return type "Object" because some preconditions can return double values, and others return Boolean values
      mPreconditionScriptPtr = mContextPtr->Compile("precondition", "Object", aInput, "end_precondition");
   }
   else if (aInput.GetCommand() == "on_new_execute")
   {
      mNewExecuteScriptPtr = mContextPtr->Compile("on_new_execute", "void", aInput, "end_on_new_execute");
   }
   else if (aInput.GetCommand() == "on_new_fail")
   {
      mNewFailScriptPtr = mContextPtr->Compile("on_new_fail", "void", aInput, "end_on_new_fail");
   }
   else if (aInput.GetCommand() == "run_selection")
   {
      std::string trans;
      aInput.ReadCommand(trans);
      if (trans == "for")
      {
         mRunType = ForTime;
         aInput.ReadValueOfType(mRunDuration, UtInput::cTIME);
      }
      else if (trans == "repeat")
      {
         mRunType = Repeat;
         aInput.ReadValue(mRepeats);
         mRepeats = std::max(mRepeats, 1); // minimum value of one
      }
      else if (trans == "until_done")
      {
         mRunType = UntilDone;
      }
      else
      {
         std::string msg =
            "unknown run_selection: " + trans + "! Valid uses: [until_done, repeat <num>, for <num> <time-unit>]";
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else if (aInput.GetCommand() == "make_selection")
   {
      std::string trans;
      aInput.ReadCommand(trans);
      if (trans == "continuous")
      {
         mSelectType = Continous;
      }
      else if (trans == "count")
      {
         mSelectType = Finite;
         aInput.ReadValue(mSelectLimit);
         mSelectLimit = std::max(mSelectLimit, 1); // minimum value of one
      }
      else
      {
         std::string msg = "unknown make_selection: " + trans + "! Valid uses: [continuous, count <num>]";
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else if (aInput.GetCommand() == "state" || aInput.GetCommand() == "show_state_evaluations" ||
            aInput.GetCommand() == "show_state_transitions")
   {
      if (mFiniteStateMachinePtr == nullptr)
      {
         mFiniteStateMachinePtr = new WsfScriptStateMachine(*mContextPtr);
      }
      myCommand = mFiniteStateMachinePtr->ProcessInput(aInput);
   }
   else if (aInput.GetCommand() == "on_message")
   {
      if (mMessageHandlerPtr == nullptr)
      {
         mMessageHandlerPtr = new WsfScriptMessageHandler(*mContextPtr);
      }
      myCommand = mMessageHandlerPtr->ProcessInput(aInput); // on_message script handling
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

bool WsfBehaviorTreeNode::PreconditionsMet()
{
   bool preconditionsMet = true;
   if (mPreconditionScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal(preconditionsMet);
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mPreconditionScriptPtr, scriptRetVal, scriptArgs);
      if (scriptRetVal.GetType() == ut::script::Data::Type::cDOUBLE)
      {
         preconditionsMet = (scriptRetVal.GetDouble() > 0);
      }
      else
      {
         preconditionsMet = scriptRetVal.GetBool(); // mPreconditionScriptPtr is compiled with return type of Object,
                                                    // anything could be returned, hopefully they used a primitive type
      }
   }
   return preconditionsMet;
}

double WsfBehaviorTreeNode::PreconditionValue()
{
   double retVal = 0.0;
   if (mPreconditionScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal(retVal);
      UtScriptDataList scriptArgs;
      mContextPtr->ExecuteScript(mPreconditionScriptPtr, scriptRetVal, scriptArgs);
      if (scriptRetVal.GetType() == ut::script::Data::Type::cBOOL)
      {
         retVal = (scriptRetVal.GetBool()) ? (1.0) : (-std::numeric_limits<double>::max());
      }
      else
      {
         retVal = scriptRetVal.GetDouble(); // mPreconditionScriptPtr is compiled with return type of Object, anything
                                            // could be returned, hopefully they used a primitive type
      }
   }
   return retVal;
}

void WsfBehaviorTreeNode::AddChild(WsfBehaviorTreeNode* aNodePtr)
{
   if (!aNodePtr)
   {
      throw std::invalid_argument("nullptr passed as argument");
   }
   mChildren.push_back(aNodePtr);
   aNodePtr->SetParent(this);
}

WsfBehaviorTreeNode* WsfBehaviorTreeNode::ChildEntry(int aIndex)
{
   if (aIndex < 0 || aIndex >= (int)mChildren.size())
   {
      return nullptr;
   }
   return mChildren[aIndex];
}

bool WsfBehaviorTreeNode::Initialize(double aSimTime, WsfScriptContext* aParentContext)
{
   mInitialized = true;
   mContextPtr->SetParent(aParentContext); // set my parent (either the processor context, or another behavior context)

   // this class member context can get at "PROCESSOR" and "PLATFORM" variables from parent contexts
   mInitialized &= mContextPtr->Initialize(aSimTime, *aParentContext, this); // initialize before all my children try to
   if (OwningPlatform() != nullptr && mMessageHandlerPtr != nullptr)
   {
      mInitialized &= mMessageHandlerPtr->Initialize(aSimTime, OwningPlatform());
   }

   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      mInitialized &= (*it)->Initialize(aSimTime, this->mContextPtr);
   }
   if (mFiniteStateMachinePtr != nullptr)
   {
      mFiniteStateMachinePtr->SetProcessor(OwningProcessor());
      mFiniteStateMachinePtr->Initialize(aSimTime);
      mStateIndex = mFiniteStateMachinePtr->GetInitialStateIndex();
      mFiniteStateMachinePtr->EnterState(mStateIndex); // this forces the "on_entry" block to be ran for the initial state
   }
   return mInitialized;
}

bool WsfBehaviorTreeNode::Execute(double aSimTime, bool aSkipCheck)
{
   std::string lastFailureReason = mFailureReason;
   double      lastFailureTime   = mFailureTime;

   bool executed = false;
   if (mTurnedOn == false)
   {
      mFailureReason = std::string("node turned off");
   }
   else if (aSkipCheck || PreconditionsMet())
   {
      if (TryRunLast(aSimTime))
      {
         SetExecutedLastRun(true);
         return true;
      }
      // Update last run state and run on_new_execute or on_new_fail blocks if necessary
      executed = ShouldRunNext(aSimTime);
      SetExecutedLastRun(executed);
      // Run the execute block. Will only fail on intermediate nodes
      executed = (executed && LocalExecute(aSimTime));

      if (executed && (mFiniteStateMachinePtr != nullptr))
      {
         mStateIndex = mFiniteStateMachinePtr->EvaluateState(mStateIndex); // only does one state transition (if necessary)
      }
   }

   if (executed == false)
   {
      // reason for failure string should be set here (either node off or by precondition)
      // save it in temp variable, and feed it into SetExecutedLastRun() method
      // so we can let that method do the event output (when detecting differences)
      std::string temp = mFailureReason;
      mFailureReason   = lastFailureReason;
      mFailureTime     = lastFailureTime;
      // this sets failure reason back to whatever precondition script had it at
      SetExecutedLastRun(false, temp); // updates mFailureReason if its different
      SetExecutedLastRunRecursive(false);
   }
   return ExecutedLastRun();
}

void WsfBehaviorTreeNode::SetFailureReason(const std::string& aReason, bool aSendEvent)
{
   double simTime = mContextPtr->GetTIME_NOW(mContextPtr->GetContext());
   if (mFailureReason != aReason)
   {
      if (simTime > mFailureTime)
      {
         mFailureReason = aReason;
         if (aSendEvent)
         {
            WsfObserver::BehaviorTreeNodeExec(GetSimulation())(simTime, this);
         }
      }
   }
   mFailureTime = simTime;
}

void WsfBehaviorTreeNode::SetExecutedLastRun(bool aVal)
{
   if (mLastExecute != aVal)
   {
      WsfSimulation* simPtr = GetSimulation();
      mLastExecute          = aVal;
      double simTime        = simPtr->GetSimTime();
      WsfObserver::BehaviorTreeNodeExec(simPtr)(simTime, this);

      if (mLastExecute) // new execute
      {
         if (mNewExecuteScriptPtr != nullptr)
         {
            mContextPtr->ExecuteScript(simTime, mNewExecuteScriptPtr);
         }
      }
      else // new fail
      {
         if (mNewFailScriptPtr != nullptr)
         {
            mContextPtr->ExecuteScript(simTime, mNewFailScriptPtr);
         }
      }
   }
}

void WsfBehaviorTreeNode::SetExecutedLastRunRecursive(bool aVal)
{
   this->SetExecutedLastRun(aVal);
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      (*it)->SetExecutedLastRunRecursive(aVal);
   }
}

void WsfBehaviorTreeNode::SetExecutedLastRun(bool aVal, const std::string& aReason)
{
   // if the reason for failure changes, then force an event
   //(but do not force the script callbacks for new execute/fail)
   SetFailureReason(aReason, false);
   SetExecutedLastRun(aVal);
}

void WsfBehaviorTreeNode::SetExecutedLastRunRecursive(bool aVal, const std::string& aReason)
{
   this->SetExecutedLastRun(aVal, aReason);
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      (*it)->SetExecutedLastRunRecursive(aVal, aReason);
   }
}

WsfBehaviorTree* WsfBehaviorTreeNode::OwningTree()
{
   return mParentTreePtr;
}

void WsfBehaviorTreeNode::SetTree(WsfBehaviorTree* aTreePtr)
{
   mParentTreePtr = aTreePtr;
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      (*it)->SetTree(mParentTreePtr);
   }
}

WsfScriptProcessor* WsfBehaviorTreeNode::OwningProcessor()
{
   if (OwningTree() != nullptr)
   {
      return OwningTree()->ParentProcessor();
   }
   return nullptr;
}

WsfPlatform* WsfBehaviorTreeNode::OwningPlatform()
{
   if (OwningProcessor() != nullptr)
   {
      return OwningProcessor()->GetPlatform();
   }
   return nullptr;
}

bool WsfBehaviorTreeNode::TryRunLast(double aSimTime)
{
   if (mLastNodeSelectedPtr != nullptr)
   {
      switch (mRunType)
      {
      case UntilDone:
      {
         if (mLastNodeSelectedPtr->Execute(aSimTime))
         {
            return true;
         }
      }
      break;
      case ForTime:
      {
         if ((aSimTime - mRunStartTime) < mRunDuration)
         {
            if (mLastNodeSelectedPtr->Execute(aSimTime))
            {
               return true;
            }
         }
      }
      break;
      case Repeat:
      default:
      {
         if (mRepeatsLeft > 0)
         {
            mRepeatsLeft--;
            if (mLastNodeSelectedPtr->Execute(aSimTime))
            {
               return true;
            }
         }
      }
      break;
      }
   }
   mLastNodeSelectedPtr = nullptr;
   return false;
}

bool WsfBehaviorTreeNode::ShouldRunNext(double aSimTime)
{
   if (mSelectType == Finite)
   {
      if (mSelectCount == mSelectLimit)
      {
         ++mSelectCount;
         std::vector<WsfBehaviorTreeNode*>::iterator it;
         for (it = mChildren.begin(); it != mChildren.end(); ++it)
         {
            (*it)->SetExecutedLastRunRecursive(false, "whole branch done, finite selection!");
         }
         SetFailureReason("whole branch done, finite selection!");
      }
      if (mSelectCount >= mSelectLimit)
      {
         return false;
      }
      ++mSelectCount;
      mRepeatsLeft = mRepeats - 1;
   }
   return true; // if count left, or mSelectType == Continuous
}

void WsfBehaviorTreeNode::LogNodeStructure(double aSimTime, bool aRecurse)
{
   // throw an event, logging the children of this node
   WsfSimulation* simPtr = GetSimulation();
   WsfObserver::BehaviorTreeNodeChildren(simPtr)(simPtr->GetSimTime(), this);

   // cause all children to log their structure too???
   if (aRecurse)
   {
      std::vector<WsfBehaviorTreeNode*>::iterator it;
      for (it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         (*it)->LogNodeStructure(aSimTime, aRecurse);
      }
   }
}

void WsfBehaviorTreeNode::SetFilePath(const std::string& aPath)
{
   // get last file modified time, and set it along with the new name
   struct stat results;
   stat(aPath.c_str(), &results); // if fails, then "results.st_mtime" should hold default time_t value...
   SetFileInfo(aPath, results.st_mtime);
}

void WsfBehaviorTreeNode::SetFileInfo(const std::string& aPath, time_t aModTime)
{
   mFullPathFileName = aPath;
   mFileLastModTime  = aModTime;
}

WsfBehaviorTreeNode* WsfBehaviorTreeNode::FindNode(const std::string& aNodeName)
{
   if (aNodeName == WsfObject::GetType())
   {
      return this;
   }

   WsfBehaviorTreeNode*                        nodePtr = nullptr;
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); ((it != mChildren.end()) && (nodePtr == nullptr)); ++it)
   {
      nodePtr = (*it)->FindNode(aNodeName);
   }
   return nodePtr;
}

void WsfBehaviorTreeNode::FindLastExecuted(std::vector<WsfBehaviorTreeNode*>& aLastExecuted)
{
   if (ExecutedLastRun() && (this->ChildCount() == 0)) // Leaf node
   {
      // Only want leaf nodes
      aLastExecuted.push_back(this);
   }

   if (ChildCount() > 0)
   {
      std::vector<WsfBehaviorTreeNode*>::iterator it;
      for (it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         (*it)->FindLastExecuted(aLastExecuted);
      }
   }
}

UtScriptContext* WsfBehaviorTreeNode::GetScriptAccessibleContext() const
{
   return &mContextPtr->GetContext();
}

const char* WsfBehaviorTreeNode::GetScriptClassName() const
{
   return "WsfBehaviorTreeNode";
}

WsfSimulation* WsfBehaviorTreeNode::GetSimulation()
{
   WsfPlatform* platformPtr = OwningPlatform();
   return (platformPtr != nullptr) ? platformPtr->GetSimulation() : mContextPtr->GetSimulation();
}

// virtual
bool WsfBehaviorTreeNode::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (mMessageHandlerPtr != nullptr)
   {
      return mMessageHandlerPtr->ProcessMessage(aSimTime, aMessage);
   }
   return false;
}

WsfScriptBehaviorTreeNodeClass::WsfScriptBehaviorTreeNodeClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptObjectClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfBehaviorTreeNode");
   AddMethod(ut::make_unique<Id>());
   AddMethod(ut::make_unique<Failure>());
   AddMethod(ut::make_unique<Executed>());
   AddMethod(ut::make_unique<Parent>());
   AddMethod(ut::make_unique<ChildCount>());
   AddMethod(ut::make_unique<ChildEntry>());
   AddMethod(ut::make_unique<TurnOff>());
   AddMethod(ut::make_unique<TurnOn>());
}

UtScriptContext* WsfScriptBehaviorTreeNodeClass::GetContext(void* aObjectPtr)
{
   UtScriptContext*     contextPtr = nullptr;
   WsfBehaviorTreeNode* nodePtr    = static_cast<WsfBehaviorTreeNode*>(aObjectPtr);
   if (nodePtr != nullptr)
   {
      contextPtr = &(nodePtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, Id, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Id());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, Failure, 1, "bool", "string")
{
   std::string failureReason = aVarArgs[0].GetString();
   aObjectPtr->SetFailureReason(failureReason);
   aReturnVal.SetBool(false);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, Executed, 0, "bool", "")
{
   bool executedLastUpdate = aObjectPtr->ExecutedLastRun();
   aReturnVal.SetBool(executedLastUpdate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, Parent, 0, "WsfBehaviorTreeNode", "")
{
   WsfBehaviorTreeNode* nodePtr = aObjectPtr->Parent();
   aReturnVal.SetPointer(new UtScriptRef(nodePtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, ChildCount, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->ChildCount());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, ChildEntry, 1, "WsfBehaviorTreeNode", "int")
{
   int                  index   = aVarArgs[0].GetInt();
   WsfBehaviorTreeNode* nodePtr = aObjectPtr->ChildEntry(index);
   aReturnVal.SetPointer(new UtScriptRef(nodePtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, TurnOff, 0, "void", "")
{
   aObjectPtr->TurnOff();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptBehaviorTreeNodeClass, WsfBehaviorTreeNode, TurnOn, 0, "void", "")
{
   aObjectPtr->TurnOn();
}

WsfBehaviorTreeLeafNode::WsfBehaviorTreeLeafNode(WsfScenario& aScenario,
                                                 bool         aRegisterPlatformVariable,
                                                 bool         aRegisterProcessorVariable)
   : WsfBehaviorTreeNode(aScenario)
   , mOnInitScriptPtr(nullptr)
   , mExecuteScriptPtr(nullptr)
{
   WsfObject::SetType(WsfStringId("WSF_BEHAVIOR"));
   WsfObject::SetName(WsfStringId("behavior"));

   // setup extern variables so any scripts compiled with this member context wont barf at us
   if (aRegisterPlatformVariable)
   {
      RegisterExternVariable("WsfPlatform", "PLATFORM");
   }
   if (aRegisterProcessorVariable)
   {
      RegisterExternVariable("WsfProcessor", "PROCESSOR");
   }
}

WsfBehaviorTreeLeafNode::WsfBehaviorTreeLeafNode(const WsfBehaviorTreeLeafNode& aSrc)
   : WsfBehaviorTreeNode(aSrc)
   , mOnInitScriptPtr(nullptr)
   , mExecuteScriptPtr(nullptr)
{
   mOnInitScriptPtr  = mContextPtr->FindScript("on_init");
   mExecuteScriptPtr = mContextPtr->FindScript("execute");
}

WsfObject* WsfBehaviorTreeLeafNode::Clone() const
{
   return new WsfBehaviorTreeLeafNode(*this);
}

bool WsfBehaviorTreeLeafNode::ProcessInput(UtInput& aInput)
{
   if (aInput.GetCommand() == "behavior") // gets last command read
   {
      UtInputBlock block(aInput);
      std::string  name;
      if (block.ReadCommand(name)) // reads next command, should be the behavior name
      {
         WsfObject::SetName(name);

         std::string cmd;
         while (block.ReadCommand(cmd))
         {
            if (WsfBehaviorTreeNode::ProcessInput(aInput))
            {
               // precondition, on_new_execute, or on_new_fail
            }
            else if (cmd == "execute")
            {
               mExecuteScriptPtr = mContextPtr->Compile(cmd, "void", aInput, "end_execute");
            }
            else if (cmd == "on_init")
            {
               mOnInitScriptPtr = mContextPtr->Compile(cmd, "void", aInput, "end_on_init");
            }
            else if (cmd == "sequence" || cmd == "selector" || cmd == "parallel" || cmd == "priority_selector" ||
                     cmd == "weighted_random" || cmd == "behavior_node")
            {
               if (mParentTreePtr == nullptr)
               {
                  // create a temporary tree for creating child types
                  mParentTreePtr = new WsfBehaviorTree(GetScenario());
               }
               WsfBehaviorTreeNode* newChildPtr = mParentTreePtr->CreateNode(aInput);
               if (newChildPtr != nullptr)
               {
                  AddChild(newChildPtr);
               }
            }
            else if (mContextPtr->ProcessInput(aInput))
            {
               // still good
            }
            else
            {
               throw UtInput::UnknownCommand(aInput);
            }
         }
      }
      return true; // this input should be & was processed here
   }
   return false; // this input is not to be read here
}

// virtual
void WsfBehaviorTreeLeafNode::RegisterInput(UtInput& aInput)
{
   std::string fileName = aInput.GetCurrentFileName();
   if (fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\'))
   {
      fileName = fileName.substr(2); // strip off first two characters of filename
   }
   std::string rootPath     = UtPath::WorkingDirectory().GetNormalizedPath();
   std::string fullPathName = rootPath + "/" + fileName;
   SetFilePath(fullPathName); // this sets the last modified time too
}

bool WsfBehaviorTreeLeafNode::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   bool ok = true;
   mContextPtr->SetParent(aParentContextPtr); // set my parent (either the processor, or another behavior)
   ok &= this->mContextPtr->Initialize(aSimTime, *aParentContextPtr, this); // initialize before all my children try to
   if (OwningPlatform() != nullptr && mMessageHandlerPtr != nullptr)
   {
      ok &= mMessageHandlerPtr->Initialize(aSimTime, OwningPlatform());
   }

   if (mOnInitScriptPtr != nullptr)
   {
      mContextPtr->ExecuteScript(aSimTime, mOnInitScriptPtr);
   }

   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      ok &= (*it)->Initialize(aSimTime, this->mContextPtr);
   }

   if (mFiniteStateMachinePtr != nullptr)
   {
      mFiniteStateMachinePtr->SetProcessor(OwningProcessor());
      mFiniteStateMachinePtr->Initialize(aSimTime);
      mStateIndex = mFiniteStateMachinePtr->GetInitialStateIndex();
      mFiniteStateMachinePtr->EnterState(mStateIndex); // this forces the "on_entry" block to be ran for the initial state
   }

   return ok;
}

bool WsfBehaviorTreeLeafNode::LocalExecute(double aSimTime)
{
   if (mExecuteScriptPtr != nullptr)
   {
      mContextPtr->ExecuteScript(aSimTime, mExecuteScriptPtr);
   }

   bool retVal = true;
   if (mChildren.size() > 0)
   {
      // A "leaf" node treats its children nodes just as a Parallel Node would
      // It performs each of it's children during execution, no matter what.
      // returns true if any children returned true
      retVal = false;
      std::vector<WsfBehaviorTreeNode*>::iterator it;
      for (it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         if ((*it)->Execute(aSimTime))
         {
            retVal = true;
         }
      }
   }
   return retVal;
}

void WsfBehaviorTreeLeafNode::SetTree(WsfBehaviorTree* aTreePtr)
{
   mParentTreePtr = aTreePtr;
   mParentTreePtr->RegisterBehaviorNode(this);
}

bool WsfBehaviorTreeLeafNode::RegisterExternVariable(const std::string& aVariableType, const std::string& aVariableName)
{
   bool registered = false;
   if (mContextPtr != nullptr)
   {
      registered =
         (mContextPtr->GetContext().ModifyScope()->RegisterExternVariable(aVariableName, aVariableType) != nullptr);
   }
   return registered;
}

// Sequence Nodes perform each of their children during execution until
// one of them returns failure.  Opposite of Selector Nodes.
WsfBehaviorTreeSequenceNode::WsfBehaviorTreeSequenceNode(WsfScenario& aScenario)
   : WsfBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("sequence"));
   WsfObject::SetName(WsfStringId("sequence"));
}

WsfObject* WsfBehaviorTreeSequenceNode::Clone() const
{
   return new WsfBehaviorTreeSequenceNode(*this);
}

// Sequence Nodes perform each of their children during execution until
// one of them returns failure.  Opposite of Selector Nodes.
// returns true if any children were traversed
bool WsfBehaviorTreeSequenceNode::LocalExecute(double aSimTime)
{
   bool                                        retVal = false;
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      if (!(*it)->Execute(aSimTime))
      {
         for (++it; it != mChildren.end(); ++it)
         {
            (*it)->SetExecutedLastRunRecursive(false, "not considered");
         }
         break;
      }
      else
      {
         // if even one child executes, this is considered a pass
         retVal = true;
      }
   }

   if (!retVal)
   {
      SetFailureReason("no child nodes were executed!");
   }
   return retVal;
}

// Selector Nodes perform each of their children during execution until
// one of them returns success.  Opposite of Sequence Nodes.
WsfBehaviorTreeSelectorNode::WsfBehaviorTreeSelectorNode(WsfScenario& aScenario)
   : WsfBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("selector"));
   WsfObject::SetName(WsfStringId("selector"));
}

WsfObject* WsfBehaviorTreeSelectorNode::Clone() const
{
   return new WsfBehaviorTreeSelectorNode(*this);
}

// Selector Nodes perform each of their children during execution until
// one of them returns success.  Opposite of Sequence Nodes.
bool WsfBehaviorTreeSelectorNode::LocalExecute(double aSimTime)
{
   bool                                        retVal = false;
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      if ((*it)->Execute(aSimTime))
      {
         mLastNodeSelectedPtr = *it;
         mRunStartTime        = aSimTime;
         mRepeatsLeft         = mRepeats - 1;

         for (++it; it != mChildren.end(); ++it)
         {
            (*it)->SetExecutedLastRunRecursive(false, "not considered");
         }

         retVal = true;
         break;
      }
   }

   if (!retVal)
   {
      SetFailureReason("no child nodes were executed!");
   }
   return retVal;
}

// Parallel Nodes perform each of their children during execution, no matter what.
// returns true if any children returned true
WsfBehaviorTreeParallelNode::WsfBehaviorTreeParallelNode(const WsfScenario& aScenario)
   : WsfBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("parallel"));
   WsfObject::SetName(WsfStringId("parallel"));
}

WsfObject* WsfBehaviorTreeParallelNode::Clone() const
{
   return new WsfBehaviorTreeParallelNode(*this);
}

// Parallel Nodes perform each of their children during execution, no matter what.
// returns true if any children returned true
bool WsfBehaviorTreeParallelNode::LocalExecute(double aSimTime)
{
   bool                                        retVal = false;
   std::vector<WsfBehaviorTreeNode*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      if ((*it)->Execute(aSimTime))
      {
         retVal = true;
      }
   }

   if (!retVal)
   {
      SetFailureReason("no child nodes were executed!");
   }
   return retVal;
}

WsfBehaviorTreePrioritySelectorNode::WsfBehaviorTreePrioritySelectorNode(WsfScenario& aScenario)
   : WsfBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("priority_selector"));
   WsfObject::SetName(WsfStringId("priority_selector"));
}

WsfObject* WsfBehaviorTreePrioritySelectorNode::Clone() const
{
   return new WsfBehaviorTreePrioritySelectorNode(*this);
}

// PrioritySelector Nodes perform one child, according to whichever
// one returns the highest value greater than zero.  Ties go to the
// first attached node in the tie. Returns true if any child performed.
bool WsfBehaviorTreePrioritySelectorNode::LocalExecute(double aSimTime)
{
   // options is initialized with write() instead of AddNote() due to weird initialization order.
   auto options = ut::log::write({}) << "Options: ";
   options << std::setprecision(4);
   double               max     = -std::numeric_limits<double>::max() / 2.0;
   bool                 retVal  = false;
   WsfBehaviorTreeNode* nodePtr = nullptr;
   for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      double val = (*it)->PreconditionValue();
      if (mDebug)
      {
         options.AddNote() << (*it)->GetType() << " = " << val;
      }
      if (val > max)
      {
         max     = val;
         nodePtr = (*it);
      }
   }
   if (mDebug)
   {
      auto out = ut::log::debug() << "Priority Selector: Made selection.";
      if (nodePtr != nullptr)
      {
         out.AddNote() << "Selected: " << nodePtr->GetType();
      }
      else
      {
         out.AddNote() << "Selected: <nothing>";
      }
      out.InsertNote(options.TakeMessage());
   }

   mLastNodeSelectedPtr = nodePtr; // zero or a valid ptr value

   if (nodePtr != nullptr)
   {
      retVal = nodePtr->Execute(aSimTime, true); // do not need to recheck the precondition, we know here it passes
      mRunStartTime = aSimTime;
      mRepeatsLeft  = mRepeats - 1;
      for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         if ((*it)->Id() != nodePtr->Id())
         {
            (*it)->SetExecutedLastRunRecursive(false, "not chosen");
         }
      }
   }
   else
   {
      for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         (*it)->SetExecutedLastRunRecursive(false, "not chosen");
      }
   }

   if (!retVal)
   {
      SetFailureReason("no child nodes were executed!");
   }
   return retVal;
}

WsfBehaviorTreeWeightedRandomNode::WsfBehaviorTreeWeightedRandomNode(WsfScenario& aScenario)
   : WsfBehaviorTreeNode(aScenario)
{
   WsfObject::SetType(WsfStringId("weighted_random"));
   WsfObject::SetName(WsfStringId("weighted_random"));
}

WsfObject* WsfBehaviorTreeWeightedRandomNode::Clone() const
{
   return new WsfBehaviorTreeWeightedRandomNode(*this);
}

// WeightedRandom Nodes perform one child, according to whichever
// one is randomly selected.  The random selection is weighted
// according to each child node's value.
bool WsfBehaviorTreeWeightedRandomNode::LocalExecute(double aSimTime)
{
   bool                 retVal  = false;
   WsfBehaviorTreeNode* nodePtr = nullptr;
   std::vector<double>  weights;
   for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
   {
      double val = (*it)->PreconditionValue();
      val        = (val < 0.0) ? (0.0) : (val);
      weights.push_back(val);
   }
   double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
   if (sum > 0.0)
   {
      // can make a choice, at least one child returned a value
      double cumulative = 0.0;
      double R          = GetSimulation()->GetRandom().Uniform<double>();

      for (unsigned int i = 0; i < mChildren.size(); ++i)
      {
         cumulative += (weights[i] / sum);
         if (cumulative >= R)
         {
            nodePtr = mChildren[i];
            break;
         }
      }

      if (nodePtr)
      {
         retVal = nodePtr->Execute(aSimTime, true); // do not need to recheck the precondition, we know here it passes
      }
      mLastNodeSelectedPtr = nodePtr;
      mRunStartTime        = aSimTime;

      if (nodePtr)
      {
         for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
         {
            if ((*it)->Id() != nodePtr->Id())
            {
               (*it)->SetExecutedLastRunRecursive(false, "not chosen");
            }
         }
      }
   }
   else
   {
      mLastNodeSelectedPtr = nullptr;
      for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
      {
         (*it)->SetExecutedLastRunRecursive(false, "not chosen");
      }
   }

   if (!retVal)
   {
      SetFailureReason("no child nodes were executed!");
   }
   return retVal;
}
