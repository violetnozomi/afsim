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

#include "script/WsfScriptContext.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <sstream>
#include <vector>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtLog.hpp"
#include "UtRandom.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptParser.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"
#include "WsfEvent.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfVariable.hpp"
#include "script/WsfScriptManager.hpp"

// =================================================================================================
//! Construct the global script context.
//! The global script context is like any other script context except that it is not
//! associated with a specific platform or processor.
WsfScriptContext::WsfScriptContext(UtScriptEnvironment* aTypesPtr)
   : mContextPtr(ut::make_unique<UtScriptContext>(*aTypesPtr->GetGlobalInstancePrototype()))
{
   aTypesPtr->SetGlobalInstance(mContextPtr.get());

   // Register the common script variables.

   // Register the Random interface.
   // Note: Only static methods are used in WsfRandom, so we don't need or want an instance

   mContextPtr->RegisterVariable("RANDOM", UtStringIdLiteral("WsfRandom"))->GetPointer()->SetAppObject(nullptr);

   mContextPtr->RegisterReadOnlyVariableFunction("TIME_NOW", UtStringIdLiteral("double"));

   // TODO: The following is pretty dangerous because using PLATFORM for non-static methods will probably
   // TODO: cause a crash. We really need to implement a Simulation script class object that has access
   // TODO: uses the static methods.
   //
   // The PLATFORM variable exists only to access the 'static' methods in WsfScriptPlatformClass.
   // (to access the platform list!!)
   mContextPtr->RegisterVariable("PLATFORM", UtStringIdLiteral("WsfPlatform"));

   mContextPtr->RegisterVariable("__SIMULATION", UtStringIdLiteral("WsfSimulation"));
   mContextPtr->RegisterVariable("__SCENARIO", UtStringIdLiteral("WsfScenario"));
   mContextPtr->RegisterVariable("__EVENTPIPE", UtStringIdLiteral("WsfEventPipe"));
}

// =================================================================================================
//! Construct a script context for a platform or a platform part.
//! @param aParentContext The script context of the parent object.
//! @param aPartClassName The script class name (e.g.: WsfProcessor) of the host platform part.
//!                       If the string is empty then no host script variable will be defined.
//! @param aPartVarName   The script variable name (e.g.: PROCESSOR) through which the script
//!                       object will be referenced.
WsfScriptContext::WsfScriptContext(WsfScriptContext& aParentContext, WsfStringId aPartClassName, const std::string& aPartVarName)
   : mContextPtr(ut::make_unique<UtScriptContext>(aParentContext.GetEnvironment()))
   , mPartClassName(aPartClassName)
   , mPartVarName(aPartVarName)
{
   // Register the common script variables.
   mContextPtr->RegisterVariable("PLATFORM", UtStringIdLiteral("WsfPlatform"));

   // Register the script variable that defines the platform part.
   if (!mPartClassName.IsNull())
   {
      mContextPtr->RegisterThisPointer(mPartClassName);
      if (!mPartVarName.IsNull())
      {
         mContextPtr->RegisterVariable(aPartVarName, mPartClassName);
      }
   }
   SetParent(&aParentContext);
}

// =================================================================================================
//! Construct a script context for a global object (i.e.: not associated with a platform or platform part).
//! @param aParentContext The script context of the parent object.
//! @param aObjectClassName The script class name (e.g.: WsfPlatform) of the object.
WsfScriptContext::WsfScriptContext(WsfScriptContext& aParentContext, WsfStringId aObjectClassName)
   : mContextPtr(ut::make_unique<UtScriptContext>(aParentContext.GetEnvironment()))
   , mObjectClassName(aObjectClassName)
{
   // Register the script variable that defines the object.
   if (!mObjectClassName.IsNull())
   {
      mContextPtr->RegisterThisPointer(mObjectClassName);
   }
   SetParent(&aParentContext);
}

// =================================================================================================
//! Copy constructor.
WsfScriptContext::WsfScriptContext(const WsfScriptContext& aSrc)
   : mParentPtr(nullptr)
   , mContextPtr(ut::make_unique<UtScriptContext>(*aSrc.mContextPtr))
   , mTimedEvents(aSrc.mTimedEvents)
   , mSimulationPtr(nullptr)
   , mPlatformPtr(nullptr)
   , mPlatformPartPtr(nullptr)
   , mPartClassName(aSrc.mPartClassName)
   , mPartVarName(aSrc.mPartVarName)
   , mObjectClassName(aSrc.mObjectClassName)
   , mTimeNowFunctionPtr(nullptr)
   , mOnUpdateScriptPtr(nullptr)
   , mScriptCallTrace(aSrc.mScriptCallTrace)
   , mUserDefinedScriptCallTrace(aSrc.mUserDefinedScriptCallTrace)
   , mScriptPrintCode(aSrc.mScriptPrintCode)
   , mScriptCheckError(aSrc.mScriptCheckError)
   , mScriptExpectError(aSrc.mScriptExpectError)
{
}

// =================================================================================================
WsfScriptContext& WsfScriptContext::operator=(const WsfScriptContext& aRhs)
{
   if (this != &aRhs)
   {
      mParentPtr                  = nullptr;
      mContextPtr                 = ut::make_unique<UtScriptContext>(*aRhs.mContextPtr);
      mTimedEvents                = aRhs.mTimedEvents;
      mSimulationPtr              = nullptr;
      mPlatformPtr                = nullptr;
      mPlatformPartPtr            = nullptr;
      mPartClassName              = aRhs.mPartClassName;
      mPartVarName                = aRhs.mPartVarName;
      mObjectClassName            = aRhs.mObjectClassName;
      mTimeNowFunctionPtr         = nullptr;
      mOnUpdateScriptPtr          = nullptr;
      mScriptCallTrace            = aRhs.mScriptCallTrace;
      mUserDefinedScriptCallTrace = aRhs.mUserDefinedScriptCallTrace;
      mScriptPrintCode            = aRhs.mScriptPrintCode;
      mScriptCheckError           = aRhs.mScriptCheckError;
      mScriptExpectError          = aRhs.mScriptExpectError;
   }

   return *this;
}

WsfScriptContext::~WsfScriptContext() = default;

// =================================================================================================
//! Compile a script and add it to the list of managed scripts.
//! @param aInput         The input stream containing the script.
//! @param aEndBlockToken The string that indicates the end of the script (e.g.: "end_script").
//! @param aNamespace     A name that is prepended to the script name.
//! This is useful to deconflict script's with the same name.
//! @throws An exception if a compile error occurs.
UtScript* WsfScriptContext::Compile(UtInput& aInput, const std::string& aEndBlockToken, const std::string& aNamespace)
{
   // Create the script input stream and pass it to the script parser for compilation.
   UtScriptStream stream(aInput, aEndBlockToken);
   UtScript*      rawScriptPtr = mContextPtr->Parse(&stream, aNamespace);
   PostCompile(aInput, rawScriptPtr);
   return rawScriptPtr;
}

// =================================================================================================
//! Compiles the script using the provided script context and script name and return type.
//! @param aScriptName       The name to give the script.
//! @param aScriptReturnType The return type to use when compiling the script.
//! @param aInput            The input stream containing the script.
//! @param aEndBlockToken    The string that indicates the end of the script (e.g.: "end_script").
//! @param aNamespace        A name that is prepended to the script name.
//! @param aIsVariableBlock  True if this block is a 'script_variables' block
//! This is useful to deconflict script's with the same name.
//! @throws An exception if a compile error occurs.
UtScript* WsfScriptContext::Compile(const std::string& aScriptName,
                                    WsfStringId        aScriptReturnType,
                                    UtInput&           aInput,
                                    const std::string& aEndBlockToken,
                                    const std::string& aNamespace,
                                    bool               aIsVariableBlock)
{
   // Create the script input stream and pass it to the script parser for compilation.
   UtScriptStream stream(aInput, aEndBlockToken);
   UtScript* rawScriptPtr = mContextPtr->Parse(aScriptName, aScriptReturnType, &stream, aNamespace, aIsVariableBlock);
   PostCompile(aInput, rawScriptPtr);
   return rawScriptPtr;
}

// =================================================================================================
//! Compile a implicit script and add it to the list of managed scripts.
//! An 'implicit' script is one that is defined without using an explicit script/end_script block. Implicit
//! scripts are things like 'on_update', 'on_initiate', 'next_state', etc. They take no arguments and the
//! return type if defined implicitly.
//!
//! @param aInput      The input stream containing the script to be compiled.
//! @param aName       The name of the implicit script.
//! @param aReturnType The return type from the script ("void", "bool", etc.)
//! @returns A pointer to the compiled script. An exception will be thrown if a compile error occurs.
UtScript* WsfScriptContext::CompileImplicitScript(UtInput& aInput, const std::string& aName, WsfStringId aReturnType)
{
   // If the next word is 'script' then it is assumed that the old form is being used:
   //    <name> script <body> end_script.
   // If the next word is not 'script' then it is assumed the new form is being used:
   //    <name> <body> on_<name>

   std::string endBlockToken("end_script");
   std::string nextWord;
   aInput.ReadValue(nextWord);
   if (nextWord != "script")
   {
      aInput.PushBack(nextWord);
      endBlockToken = "end_" + aName;
   }

   return Compile(aName, aReturnType, aInput, endBlockToken);
}

// =================================================================================================
//! Compiles the variable list, which may be any combination of variable declarations and assignments.
//! @param aInput The input stream containing the script.
//! @throws An exception if a compile error occurs.
void WsfScriptContext::CompileVariables(UtInput& aInput)
{
   std::string endBlockToken("end_script_variables");

   static int        varDefCount = 0;
   std::stringstream ss;
   ss << "__var_defs_" << ++varDefCount;

   UtScript* rawScriptPtr = Compile(ss.str(), UtStringIdLiteral("void"), aInput, endBlockToken, "", true);
   if (rawScriptPtr == nullptr)
   {
      throw UtInput::BadValue(aInput, "Unable to compile the provided variable list");
   }
}

// =================================================================================================
//! Declare a variable in the script context if it already hasn't been declared.
//! @param aType The variable type.
//! @param aName The name of the variable.
//! @return Returns the variable's memory address (-1 if unsuccessful).
void WsfScriptContext::DeclareVariable(WsfStringId aType, const std::string& aName)
{
   // If the variable has already been declared then don't redeclare it.
   // If it does exist we should probably ensure they are the same type.
   mContextPtr->RegisterVariable(aName, aType);
}

// =================================================================================================
//! Execute a script.
//! @param aSimTime    The current simulation time.
//! @param aScriptName The name of the script to be executed.
//! @returns true if successful (i.e.: the requested script was defined) or false if the
//! if the requested script was not defined.
bool WsfScriptContext::ExecuteScript(double aSimTime, WsfStringId aScriptName)
{
   UtScript* scriptPtr = mContextPtr->FindScript(aScriptName);
   bool      success   = false;
   if (scriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(scriptPtr, scriptRetVal, scriptArgs);
      success = true;
   }
   return success;
}

// =================================================================================================
//! Execute a script.
//! @param aSimTime      The current simulation time.
//! @param aScriptPtr    The pointer to the script to execute.
void WsfScriptContext::ExecuteScript(double aSimTime, UtScript* aScriptPtr)
{
   UtScriptData     scriptRetVal;
   UtScriptDataList scriptArgs;
   CallScript(aScriptPtr, scriptRetVal, scriptArgs);
}

// =================================================================================================
//! Execute a script.
//! @param aSimTime      The current simulation time.
//! @param aScriptName   The name of the script to be executed.
//! @param aScriptRetVal The return value from the script.
//! @param aScriptArgs   The arguments to be passed to script.
//! @returns true if successful (i.e.: the requested script was defined) or false if the
//! if the requested script was not defined.
bool WsfScriptContext::ExecuteScript(double                  aSimTime,
                                     WsfStringId             aScriptName,
                                     UtScriptData&           aScriptRetVal,
                                     const UtScriptDataList& aScriptArgs)
{
   bool      success   = false;
   UtScript* scriptPtr = FindScript(aScriptName);
   if (scriptPtr != nullptr)
   {
      CallScript(scriptPtr, aScriptRetVal, aScriptArgs);
      success = true;
   }
   return success;
}

// =================================================================================================
//! Execute a script.
//! @param aSimTime      The current simulation time.
//! @param aScriptPtr    The pointer to the script to execute.
//! @param aScriptRetVal The return value from the script.
//! @param aScriptArgs   The arguments to be passed to script.
void WsfScriptContext::ExecuteScript(double                  aSimTime,
                                     UtScript*               aScriptPtr,
                                     UtScriptData&           aScriptRetVal,
                                     const UtScriptDataList& aScriptArgs)
{
   CallScript(aScriptPtr, aScriptRetVal, aScriptArgs);
}

// =================================================================================================
//! Execute a script, using the current definition of the simulation time.
//! This form is called by classes such as WsfScriptMessageHandler and WsfScriptStateMachine
//! which have already set the simulation time. This avoids having to reset it.
//!
//! @param aScriptPtr    The pointer to the script to execute.
//! @param aScriptRetVal The return value from the script.
//! @param aScriptArgs   The arguments to be passed to script.
void WsfScriptContext::ExecuteScript(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs)
{
   CallScript(aScriptPtr, aScriptRetVal, aScriptArgs);
}

// =================================================================================================
//! Execute a script
//! @param aSimTime      The current simulation time.
//! @param aScriptRetVal The return value from the script.
//! @param aScriptArgs   The arguments to be passed to script.
//! @param aScriptSource The source of the script in the format:
//!          \<return-type\>  \<name\>(<arg-list>)
//!              ... script commands ...
//!          end_script
bool WsfScriptContext::ExecuteScript(double                  aSimTime,
                                     UtScriptData&           aScriptRetVal,
                                     const UtScriptDataList& aScriptArgs,
                                     const std::string&      aScriptSource)
{
   bool scriptExecuted = false;

   UtInput input;
   input.PushInput(ut::make_unique<UtInputString>(aScriptSource));
   UtScript* scriptPtr = nullptr;
   try
   {
      scriptPtr = Compile(input, "end_script", "__temporary__");
   }
   catch (UtInput::ExceptionBase& e)
   {
      auto out = ut::log::error() << "Failed to parse script.";
      out.AddNote() << e.GetMessage();
   }
   if (scriptPtr != nullptr)
   {
      scriptExecuted = true;
      CallScript(scriptPtr, aScriptRetVal, aScriptArgs);
      mContextPtr->ModifyScope()->RemoveScript(scriptPtr->GetName());
   }
   return scriptExecuted;
}

// =================================================================================================
//! Find a script with the specified name (string form).
//! @param aScriptName The name of the script to be located.
//! @returns A pointer to the script or 0 if the script could not be found.
// virtual
UtScript* WsfScriptContext::FindScript(WsfStringId aScriptName) const
{
   return mContextPtr->FindScript(aScriptName);
}

// =================================================================================================
//! Initialize the script context.
//! This method performs all the necessary initialization of the script system and
//! - Sets the PLATFORM script variable.
//! - Sets the PROCESSOR (or whatever platform part) script variable (if not at the platform level).
//! - Sets the TIME_NOW script variable.
//! - Invokes the 'on_initialize' method if it is defined.
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr The pointer to the platform with which the script context is associated.
//! @param aPartPtr     The pointer to the platform part with which the script context is associated.
//!                     This must be zero if the script context is associated with the platform.
//! @returns 'true' if successful or 'false' if there was some error.
// virtual
bool WsfScriptContext::Initialize(double aSimTime, WsfPlatform* aPlatformPtr, WsfPlatformPart* aPartPtr)
{
   // Ensure consistency of platform and part pointers.

   if ((aPlatformPtr == nullptr) || ((!mPartClassName.IsNull()) && (aPartPtr == nullptr)))
   {
      ut::log::error() << "Invalid call to WsfScriptContext::Initialize().";
      return false;
   }

   mPlatformPartPtr   = aPartPtr;
   mPlatformPtr       = aPlatformPtr;
   WsfSimulation& sim = *aPlatformPtr->GetSimulation();
   mSimulationPtr     = &sim;

   // Register this context as a child of the correct parent context.

   if (aPartPtr != nullptr)
   {
      // Register this context as a child of the platform context.
      mParentPtr = &(mPlatformPtr->GetScriptContext());
   }
   else
   {
      // Register this context as a child of the global context
      mParentPtr = &(sim.GetScriptContext());
   }

   if (mPlatformPartPtr != nullptr)
   {
      mContextPtr->SetContextName(mPlatformPtr->GetName() + "/" + mPlatformPartPtr->GetName());
   }
   else if (mPlatformPtr != nullptr)
   {
      mContextPtr->SetContextName(mPlatformPtr->GetName());
   }

   if (!mContextPtr->Initialize())
   {
      ut::log::error() << "Failed to initialize the script context.";
      return false;
   }

   // Set the variables for PLATFORM and the platform part (PROCESSOR, etc.) - if it is defined.
   // The next two lines of code must stay before the for loop below that initializes the script variables.
   // This allows script variables to be initialized using these variables.

   mContextPtr->Var(UtStringIdLiteral("PLATFORM")).GetPointer()->SetExternalManaged(aPlatformPtr);

   // Set the 'this' pointer and the optional platform part variable.
   if (!mPartClassName.IsNull())
   {
      mContextPtr->SetThisPointer(aPartPtr);
      if (!mPartVarName.IsNull())
      {
         mContextPtr->Var(mPartVarName) = UtScriptRef::Ref(aPartPtr, mContextPtr->GetTypes());
      }
   }

   // Set the value of the script_call_trace and script_debug_writes if they weren't explicitly
   // defined. It will use the value from the global context unless this is a platform part and
   // the parent platform has explicitly defined a value.

   if (!mUserDefinedScriptCallTrace)
   {
      mScriptCallTrace = sim.GetScriptContext().mScriptCallTrace;
      if ((mPlatformPartPtr != nullptr) && mParentPtr->mUserDefinedScriptCallTrace)
      {
         mScriptCallTrace = mParentPtr->mScriptCallTrace;
      }
   }

   // Run each of the script_variable initialize scripts.
   mContextPtr->ExecuteInitScripts(GetExecutor());

   // Run the on_initialize script if it is defined.
   UtScript* initializeScriptPtr = FindScript(UtStringIdLiteral("on_initialize"));
   if (initializeScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(initializeScriptPtr, scriptRetVal, scriptArgs);
   }

   // Add events to execute timed scripts at the desired time.
   bool ok = AddTimedEvents(aSimTime);

   // Capture the 'on_update' script pointer to avoid constant lookups.
   mOnUpdateScriptPtr = FindScript(UtStringIdLiteral("on_update"));

   return ok;
}

// =================================================================================================
//! Initialize the script context for a non-platform oriented object.
//! This method performs all the necessary initialization of the script system and
//! - Sets the TIME_NOW script variable.
//! - Invokes the 'on_initialize' method if it is defined.
//! @param aSimulation    The simulation .
//! @param aObjectPtr  The pointer to the object with which the script context is associated.
//! @returns 'true' if successful or 'false' if there was some error.
// virtual
bool WsfScriptContext::Initialize(WsfSimulation& aSimulation, void* aObjectPtr)
{
   return Initialize(aSimulation.GetSimTime(), aSimulation.GetScriptContext(), aObjectPtr);
}

// =================================================================================================
bool WsfScriptContext::Initialize(double aSimTime, WsfScriptContext& aParentContext, void* aObjectPtr)
{
   if (aObjectPtr == nullptr)
   {
      ut::log::error() << "Invalid call to WsfScriptContext::Initialize().";
      return false;
   }
   mSimulationPtr = aParentContext.GetSimulation();
   SetParent(&aParentContext);

   if (!mContextPtr->Initialize())
   {
      ut::log::error() << "Failed to initialize the script context.";
      return false;
   }

   // Set the 'this' pointer and the optional platform part variable.
   if (!mObjectClassName.IsNull())
   {
      mContextPtr->SetThisPointer(aObjectPtr);
   }

   // Set the value of the script_call_trace and script_debug_writes if they weren't explicitly defined.

   if (!mUserDefinedScriptCallTrace)
   {
      mScriptCallTrace = aParentContext.mScriptCallTrace;
   }

   // Run each of the script_variable initialize scripts.
   mContextPtr->ExecuteInitScripts(GetExecutor());

   // Run the on_initialize script if it is defined.
   UtScript* initializeScriptPtr = FindScript(UtStringIdLiteral("on_initialize"));
   if (initializeScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(initializeScriptPtr, scriptRetVal, scriptArgs);
   }

   // Add events to execute timed scripts at the desired time.
   bool ok = AddTimedEvents(aSimTime);

   return ok;
}

// =================================================================================================
//! Perform phase 2 initialization.
//! This currently only invokes the 'on_initialize2' script if it is defined.
//! @param aSimTime    The current simulation time.
//! @returns 'true' if successful or 'false' if there was some error.
bool WsfScriptContext::Initialize2(double aSimTime)
{
   // Run the on_initialize2 script if it is defined.
   UtScript* initializeScriptPtr = FindScript(UtStringIdLiteral("on_initialize2"));
   if (initializeScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(initializeScriptPtr, scriptRetVal, scriptArgs);
   }
   return true;
}

class WsfScriptContext::ReadTIME_NOW : public UtScriptContext::VariableReadFunction
{
public:
   WsfSimulation* mSimulationPtr;
   void           Read(UtScriptData& aData) override { aData = mSimulationPtr->GetSimTime(); }
};

// =================================================================================================
//! Initialize the global script context.
// virtual
bool WsfScriptContext::InitializeGlobalContext(WsfSimulation& aSimulation)
{
   if (!mContextPtr->Initialize())
   {
      ut::log::error() << "Failed to initialize the script context.";
      return false;
   }
   mSimulationPtr = &aSimulation;

   // when read, TIME_NOW will call ReadTIME_NOW::Read
   {
      assert(mTimeNowFunctionPtr == nullptr);
      mTimeNowFunctionPtr                 = ut::make_unique<ReadTIME_NOW>();
      mTimeNowFunctionPtr->mSimulationPtr = mSimulationPtr;
      mContextPtr->SetReadOnlyVariableFunction(UtStringIdLiteral("TIME_NOW"), mTimeNowFunctionPtr.get());
   }

   // Run each of the script_variable initialize scripts.
   mContextPtr->ExecuteInitScripts(GetExecutor());

   // Run the on_initialize script if it is defined.
   UtScript* initializeScriptPtr = FindScript(UtStringIdLiteral("on_initialize"));
   if (initializeScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(initializeScriptPtr, scriptRetVal, scriptArgs);
   }

   // Add events to execute timed scripts at the desired time.
   bool ok = AddTimedEvents(0.0);

   return ok;
}

// =================================================================================================
//! Return a pointer to the UtScriptTypes object
UtScriptTypes* WsfScriptContext::GetTypes() const
{
   return mContextPtr->GetTypes();
}

// =================================================================================================
//! Return a pointer to the UtScriptEnvironment object
UtScriptEnvironment* WsfScriptContext::GetEnvironment() const
{
   return mContextPtr->GetEnvironment();
}

// =================================================================================================
//! Return a pointer to the script class object given its class name.
UtScriptClass* WsfScriptContext::GetClass(const std::string& aClassName) const
{
   return mContextPtr->GetTypes()->GetClass(aClassName);
}

// =================================================================================================
//! Return a pointer to the script class object given its class name (ID).
UtScriptClass* WsfScriptContext::GetClass(WsfStringId aClassName) const
{
   return mContextPtr->GetTypes()->GetClass(aClassName);
}

// =================================================================================================
//! Return a pointer to the script class object given its class name.
UtScriptClass* WsfScriptContext::GetClass(const char* aClassName) const
{
   return GetClass(std::string(aClassName));
}

//! 'execute at time' scripts.
struct WsfScriptContext::TimedEvent
{
   enum TimeType
   {
      cABSOLUTE,
      cRELATIVE,
      cINTERVAL
   };
   WsfVariable<double> mTimeValue;    //!< The absolute, relative or interval time.
   TimeType            mTimeType;     //!< The type of the time specification.
   WsfStringId         mScriptNameId; //!< The name(ID) of the script to execute.
};

// =================================================================================================
//! Determine if the current input command is a script-related command.
//! @param aInput The input stream.
//! @return 'true' if the command was script-related or 'false' if not.
// virtual
bool WsfScriptContext::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "script")
   {
      Compile(aInput);
   }
   else if (command == "script_variables")
   {
      CompileVariables(aInput);
   }
   else if ((command == "script_silent") || (command == "script_silent_mode"))
   {
      bool silentModeEnabled;
      aInput.ReadValue(silentModeEnabled);
      mContextPtr->SetWritesEnabled(!silentModeEnabled);
   }
   else if (command == "script_debug_writes")
   {
      bool scriptWritesEnabled;
      aInput.ReadValue(scriptWritesEnabled);
      mContextPtr->SetWritesEnabled(scriptWritesEnabled);
   }
   else if (command == "script_call_trace")
   {
      aInput.ReadValue(mScriptCallTrace);
      mUserDefinedScriptCallTrace = true;
   }
   else if (command == "script_expect_error") // NO_DOC | INTERNAL USE
   {
      aInput.ReadValue(mScriptExpectError);
      mScriptCheckError = true;
   }
   else if (command == "script_print_code") // NO_DOC | INTERNAL USE
   {
      aInput.ReadValue(mScriptPrintCode);
   }
   else if (command == "on_initialize")
   {
      CompileImplicitScript(aInput, "on_initialize", UtStringIdLiteral("void"));
   }
   else if (command == "on_initialize2")
   {
      if (mContextPtr->GetParent() == nullptr)
      {
         throw UtInput::OutOfContext(aInput, "on_initialize2 is not valid in the global script context");
      }
      CompileImplicitScript(aInput, "on_initialize2", UtStringIdLiteral("void"));
   }
   else if (command == "on_update")
   {
      if (mContextPtr->GetParent() == nullptr)
      {
         throw UtInput::OutOfContext(aInput, "on_update is not valid in the global script context");
      }
      CompileImplicitScript(aInput, "on_update", UtStringIdLiteral("void"));
   }
   else if (command == "execute")
   {
      TimedEvent  timedEvent;
      std::string timeType;
      aInput.ReadValue(timeType);
      if (timeType == "at_time")
      {
         timedEvent.mTimeValue.ReadValueOfType(aInput, UtInput::cTIME);
         timedEvent.mTimeValue.ValueGreater(aInput, 0.0);
         std::string timeReference;
         aInput.ReadValue(timeReference);
         if (timeReference == "relative")
         {
            timedEvent.mTimeType = TimedEvent::cRELATIVE;
         }
         else if (timeReference == "absolute")
         {
            timedEvent.mTimeType = TimedEvent::cABSOLUTE;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown time reference for at_time: " + timeReference);
         }
      }
      else if (timeType == "at_interval_of")
      {
         timedEvent.mTimeValue.ReadValueOfType(aInput, UtInput::cTIME);
         timedEvent.mTimeValue.ValueGreater(aInput, 0.0);
         timedEvent.mTimeType = TimedEvent::cINTERVAL;
      }
      else
      {
         throw UtInput::BadValue(aInput, "'at_time' or 'at_interval_of' must follow 'execute':" + timeType);
      }

      std::ostringstream oss;
      oss << "_" << command << "_" << mTimedEvents.size() + 1;
      std::string scriptName(oss.str());
      UtScript*   scriptPtr    = Compile(scriptName, UtStringIdLiteral("void"), aInput, "end_execute");
      timedEvent.mScriptNameId = scriptPtr->GetName();
      mTimedEvents.push_back(timedEvent);
   }
   else if (command == "at_time")
   {
      // RAII block
      {
         auto out = ut::log::warning() << "'at_time' has been deprecated.";
         out.AddNote() << "Please use 'execute at_time ... end_execute'";
      }
      TimedEvent timedEvent;
      timedEvent.mTimeValue.ReadValueOfType(aInput, UtInput::cTIME);
      timedEvent.mTimeValue.ValueGreater(aInput, 0.0);
      std::string timeReference;
      aInput.ReadValue(timeReference);
      if (timeReference == "relative")
      {
         timedEvent.mTimeType = TimedEvent::cRELATIVE;
      }
      else if (timeReference == "absolute")
      {
         timedEvent.mTimeType = TimedEvent::cABSOLUTE;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown time reference for at_time: " + timeReference);
      }

      std::ostringstream oss;
      oss << "_" << command << "_" << mTimedEvents.size() + 1;
      std::string scriptName(oss.str());
      UtScript*   scriptPtr    = Compile(scriptName, UtStringIdLiteral("void"), aInput, "end_at_time");
      timedEvent.mScriptNameId = scriptPtr->GetName();
      mTimedEvents.push_back(timedEvent);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Set the parent script context.
//! @param aParentPtr The desired parent context for this context.
void WsfScriptContext::SetParent(WsfScriptContext* aParentPtr)
{
   mContextPtr->SetParent(aParentPtr->mContextPtr.get());
}

// =================================================================================================
//! Set the parent script context.
//! @param aParentPtr The desired parent context for this context.
void WsfScriptContext::SetParent(UtScriptContext* aParentPtr)
{
   mContextPtr->SetParent(aParentPtr);
}

// =================================================================================================
//! A helper method, which simplifies the process of extracting
//! the value stored in the 'TIME_NOW' variable.
//@param aContext The script context that contains the 'TIME_NOW' variable.
//@return Returns the current value of stored in 'TIME_NOW'.
// static
double WsfScriptContext::GetTIME_NOW(const UtScriptContext& aContext)
{
   UtScriptContext::VariableReadFunction* fnPtr = aContext.FindReadOnlyVariableFunction(UtStringIdLiteral("TIME_NOW"));
   if (fnPtr != nullptr)
   {
      UtScriptData timeNow;
      fnPtr->Read(timeNow);
      return timeNow.GetDouble();
   }
   return 0.0;
}

// =================================================================================================
//! A helper method, which simplifies the process of extracting
//! the value stored in the 'PLATFORM' variable.
//! @param aContext The script context that contains the 'PLATFORM' variable.
//! @return Returns a pointer to the current platform stored in 'PLATFORM'.
// static
WsfPlatform* WsfScriptContext::GetPLATFORM(const UtScriptContext& aContext)
{
   WsfPlatform*  platformPtr = nullptr;
   UtScriptData* platVarPtr  = aContext.FindGlobalVar(UtStringIdLiteral("PLATFORM"));
   if (platVarPtr != nullptr)
   {
      UtScriptRef* refPtr = platVarPtr->GetPointer();
      if (refPtr != nullptr)
      {
         platformPtr = static_cast<WsfPlatform*>(refPtr->GetAppObject());
      }
   }
   return platformPtr;
}

// =================================================================================================
//! A helper method, which simplifies the process of extracting
//! the value stored in the 'PROCESSOR' variable.
//! @param aContext The script context that contains the 'PROCESSOR' variable.
//! @return Returns a pointer to the current processor stored in 'PROCESSOR'.
// static
WsfProcessor* WsfScriptContext::GetPROCESSOR(const UtScriptContext& aContext)
{
   WsfProcessor* processorPtr = nullptr;
   UtScriptData* varPtr       = aContext.FindVar(UtStringIdLiteral("PROCESSOR"));
   if (varPtr != nullptr)
   {
      UtScriptRef* refPtr = varPtr->GetPointer();
      if (refPtr != nullptr)
      {
         processorPtr = static_cast<WsfProcessor*>(refPtr->GetAppObject());
      }
   }
   return processorPtr;
}

// =================================================================================================
//! A helper method, which simplifies the process of extracting
//! the value stored in the 'MESSAGE' variable.
//! @param aContext The script context that contains the 'MESSAGE' variable.
//! @return Returns a pointer to the current processor stored in 'MESSAGE'.
// static
WsfMessage* WsfScriptContext::GetMESSAGE(const UtScriptContext& aContext)
{
   WsfMessage*   messagePtr = nullptr;
   UtScriptData* varPtr     = aContext.FindVar(UtStringIdLiteral("MESSAGE"));
   if (varPtr != nullptr)
   {
      UtScriptRef* refPtr = varPtr->GetPointer();
      if (refPtr != nullptr)
      {
         messagePtr = static_cast<WsfMessage*>(refPtr->GetAppObject());
      }
   }
   return messagePtr;
}

// =================================================================================================
//! A helper method, which simplifies the process of extracting
//! the value stored in the 'TRACK' variable.
//! @param aContext The script context that contains the 'TRACK' variable.
//! @return Returns a pointer to the current processor stored in 'TRACK'.
// static
WsfTrack* WsfScriptContext::GetTRACK(const UtScriptContext& aContext)
{
   WsfTrack*     trackPtr = nullptr;
   UtScriptData* varPtr   = aContext.FindVar(UtStringIdLiteral("TRACK"));
   if (varPtr != nullptr)
   {
      UtScriptRef* refPtr = varPtr->GetPointer();
      if (refPtr != nullptr)
      {
         trackPtr = static_cast<WsfTrack*>(refPtr->GetAppObject());
      }
   }
   return trackPtr;
}

// =================================================================================================
WsfSimulation* WsfScriptContext::GetSIMULATION(const UtScriptContext& aContext)
{
   WsfSimulation* simPtr = nullptr;
   UtScriptData*  varPtr = aContext.FindGlobalVar(UtStringIdLiteral("__SIMULATION"));
   if (varPtr != nullptr)
   {
      simPtr = (WsfSimulation*)varPtr->GetPointer()->GetAppObject();
   }
   return simPtr;
}

// =================================================================================================
WsfScenario* WsfScriptContext::GetSCENARIO(const UtScriptContext& aContext)
{
   WsfScenario*  scenarioPtr = nullptr;
   UtScriptData* varPtr      = aContext.FindGlobalVar(UtStringIdLiteral("__SCENARIO"));
   if (varPtr != nullptr)
   {
      scenarioPtr = (WsfScenario*)varPtr->GetPointer()->GetAppObject();
   }
   return scenarioPtr;
}

// =================================================================================================
WsfEventPipeInterface* WsfScriptContext::GetEVENTPIPE(const UtScriptContext& aContext)
{
   WsfEventPipeInterface* eventPipePtr = nullptr;
   UtScriptData*          varPtr       = aContext.FindGlobalVar(UtStringIdLiteral("__EVENTPIPE"));
   if (varPtr != nullptr)
   {
      eventPipePtr = (WsfEventPipeInterface*)varPtr->GetPointer()->GetAppObject();
   }
   return eventPipePtr;
}

// =================================================================================================
//! Invoke the 'on_update' script if it is defined.
void WsfScriptContext::Update(double aSimTime)
{
   if (mOnUpdateScriptPtr != nullptr)
   {
      mPlatformPtr->Update(aSimTime); // Ensure the platform location is current.
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;
      CallScript(mOnUpdateScriptPtr, scriptRetVal, scriptArgs);
   }
}

// =================================================================================================
//! Validate the script return type and argument list against the expected values.
//! @param aScriptPtr          The point to the script that is to be checked. If this is zero then
//!                            nothing is compared and true is returned.
//! @param aExpectedReturnType The expected return type.
//! @param aExpectedArgumentTypes The expected argument types as a comma-separated list.
//! @returns true if the expected values and actual values match (or if the script pointer was zero).
//!          'false' if any of the actual values do not match their expected value.
bool WsfScriptContext::ValidateScript(UtScript*          aScriptPtr,
                                      WsfStringId        aExpectedReturnType,
                                      const std::string& aExpectedArgumentTypes) const
{
   bool valid = true;
   if (aScriptPtr != nullptr)
   {
      const UtScriptFunction* expectedProtoPtr =
         GetEnvironment()->GetPrototype(aExpectedReturnType, aExpectedArgumentTypes);
      if (expectedProtoPtr != nullptr)
      {
         if (expectedProtoPtr != aScriptPtr->mPrototype)
         {
            WriteErrorHeader(aScriptPtr);
            auto out = ut::log::error() << "Unexpected script signature.";
            out.AddNote() << "Expected: " << GetEnvironment()->GetPrototypeDescription(expectedProtoPtr);
            out.AddNote() << "Received: " << GetEnvironment()->GetPrototypeDescription(aScriptPtr->mPrototype);
            valid = false;
         }
      }
      else
      {
         valid = false;
         assert(!"Invalid expected signature.");
      }
   }
   return valid;
}

// =================================================================================================
//! Add events to invoke scripts at the requested time.
// private
bool WsfScriptContext::AddTimedEvents(double aSimTime)
{
   bool ok = true;
   // The following will be 0 for the global context, the platform for a platform context or the
   // platform part for a platform part context.
   WsfComponent* objectPtr = mPlatformPartPtr;
   if (objectPtr == nullptr)
   {
      objectPtr = mPlatformPtr;
   }
   for (auto& timedEvent : mTimedEvents)
   {
      std::string varName = "execute at_time (absolute)";
      if (timedEvent.mTimeType == TimedEvent::cRELATIVE)
      {
         varName = "execute at_time (relative)";
      }
      else if (timedEvent.mTimeType == TimedEvent::cINTERVAL)
      {
         varName = "execute at_interval_of";
      }
      if (!timedEvent.mTimeValue.Initialize("time", objectPtr, this, GetGlobalContext()))
      {
         ok = false;
         continue;
      }
      UtScriptDataList scriptArgs;
      double           simTime   = 0.0;
      double           interval  = 0.0;
      double           timeValue = timedEvent.mTimeValue;
      if (timedEvent.mTimeType == TimedEvent::cRELATIVE)
      {
         simTime = aSimTime + timeValue;
      }
      else if (timedEvent.mTimeType == TimedEvent::cABSOLUTE)
      {
         simTime = std::max(timeValue, aSimTime); // Don't allow the time to be in the past!
      }
      else if (timedEvent.mTimeType == TimedEvent::cINTERVAL)
      {
         interval = timeValue;
         simTime  = aSimTime + GetSimulation()->GetScriptRandom().Uniform(0.0, interval);
      }
      else
      {
         ut::log::error() << "Internal error in WsfScriptContext::AddTimedEvents.";
         ok = false;
         continue;
      }
      GetSimulation()->AddEvent(
         ut::make_unique<ExecuteTimedEventScript>(simTime, interval, mPlatformPtr, this, timedEvent.mScriptNameId));
   }
   return ok;
}

// =================================================================================================
//! Calls a script, with optional call tracing
// private
void WsfScriptContext::CallScript(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs)
{
   UtScriptExecutor* executorPtr = GetExecutor();
   assert(executorPtr != nullptr);
   if (executorPtr == nullptr)
   {
      return;
   }
   if (mScriptCallTrace)
   {
      { // RAII block
         auto out = ut::log::debug() << "{SC} " << aScriptPtr->GetName() << " in ";
         if (mPlatformPtr != nullptr)
         {
            out << mPlatformPtr->GetName();
            if (mPlatformPartPtr != nullptr)
            {
               out << '.' << mPlatformPartPtr->GetName();
            }
         }
         else
         {
            out << "<global>";
         }
         out.AddNote() << "T = " << GetSimTimeP();
      }

      mContextPtr->Execute(executorPtr, aScriptPtr, aScriptRetVal, aScriptArgs);

      switch (aScriptRetVal.GetType())
      {
      case ut::script::Data::Type::cUNDEFINED:
         ut::log::debug() << "{SR} void";
         break;
      case ut::script::Data::Type::cBOOL:
         ut::log::debug() << "{SR} bool: " << (aScriptRetVal.GetBool() ? "true" : "false");
         break;
      case ut::script::Data::Type::cINT:
         ut::log::debug() << "{SR} int: " << aScriptRetVal.GetInt();
         break;
      case ut::script::Data::Type::cDOUBLE:
         ut::log::debug() << "{SR} double: " << aScriptRetVal.GetDouble();
         break;
      case ut::script::Data::Type::cSTRING:
         ut::log::debug() << "{SR} string: '" << aScriptRetVal.GetString() << "'";
         break;
      case ut::script::Data::Type::cPOINTER:
      {
         auto         out    = ut::log::debug() << "{SR} pointer";
         UtScriptRef* refPtr = aScriptRetVal.GetPointer();
         if (refPtr != nullptr)
         {
            UtScriptClass* classPtr = refPtr->GetScriptClass();
            if (classPtr != nullptr)
            {
               out << " -> " << classPtr->GetClassName();
            }
         }
      }
      break;
      default:
         break;
      }
   }
   else
   {
      mContextPtr->Execute(executorPtr, aScriptPtr, aScriptRetVal, aScriptArgs);
   }
}

// =================================================================================================
//! Perform common operations after attempted compilation
// private
void WsfScriptContext::PostCompile(UtInput& aInput, UtScript* aRawScriptPtr)
{
   if (aRawScriptPtr == nullptr)
   {
      if (!mScriptCheckError)
      {
         WriteErrorHeader(nullptr);
         throw UtInput::BadValue(aInput, "Unable to compile script.");
      }
      if (!mScriptExpectError)
      {
         ut::log::info("test_fail") << "-FAIL- Unexpected script compilation error.";
      }
   }
   else
   {
      if (mScriptCheckError && mScriptExpectError)
      {
         ut::log::info("test_fail") << "-FAIL- Expected script compilation error.";
      }
      if (mScriptPrintCode)
      {
         // Revisit this after addressing UtScript
         auto out = ut::log::info() << "Script Post-Compile:";
         aRawScriptPtr->Print(out);
      }
   }
   mScriptCheckError = false; // This is a one-shot operation (see the header)
}

// =================================================================================================
// private
void WsfScriptContext::WriteErrorHeader(UtScript* aScriptPtr) const
{
   auto out = ut::log::error() << "Error in script.";

   if (aScriptPtr != nullptr)
   {
      out.AddNote() << "Script: " << aScriptPtr->GetName();
   }

   if (mPlatformPtr != nullptr)
   {
      out.AddNote() << "Platform: " << mPlatformPtr->GetName();
   }
   if (mPlatformPartPtr != nullptr)
   {
      out.AddNote() << "Part: " << mPlatformPartPtr->GetName();
   }

   if (mPlatformPtr == nullptr && mPlatformPartPtr == nullptr)
   {
      out.AddNote() << "Global context";
   }
}

// =================================================================================================
WsfScriptContext& WsfScriptContext::GetGlobalContext()
{
   WsfScriptContext* contextPtr = this;
   while (contextPtr->mParentPtr != nullptr)
   {
      contextPtr = contextPtr->mParentPtr;
   }
   return *contextPtr;
}

// =================================================================================================
WsfSimulation* WsfScriptContext::GetSimulation() const
{
   return mSimulationPtr;
}

// =================================================================================================
const WsfScenario* WsfScriptContext::GetScenario() const
{
   if (mSimulationPtr != nullptr)
   {
      return &mSimulationPtr->GetScenario();
   }
   return GetSCENARIO(*mContextPtr);
}

// =================================================================================================
UtScriptExecutor* WsfScriptContext::GetExecutor() const
{
   WsfSimulation* simPtr = GetSimulation();
   if (simPtr != nullptr)
   {
      return &simPtr->GetScriptExecutor();
   }
   return nullptr;
}

// =================================================================================================
UtScriptData WsfScriptContext::GetSimTimeP()
{
   return UtScriptData(mSimulationPtr ? mSimulationPtr->GetSimTime() : 0.0);
}

// =================================================================================================
WsfScriptContext::ExecuteTimedEventScript::ExecuteTimedEventScript(double            aSimTime,
                                                                   double            aTimeInterval,
                                                                   WsfPlatform*      aPlatformPtr,
                                                                   WsfScriptContext* aContextPtr,
                                                                   WsfStringId       aScriptName)
   : WsfEvent(aSimTime)
   , mTimeInterval(aTimeInterval)
   , mPlatformIndex(0)
   , mContextPtr(aContextPtr)
   , mScriptName(aScriptName)
{
   // If a null platform pointer is supplied then it is executing in the global context.
   if (aPlatformPtr != nullptr)
   {
      mPlatformIndex = aPlatformPtr->GetIndex();
   }
}

// =================================================================================================
WsfEvent::EventDisposition WsfScriptContext::ExecuteTimedEventScript::Execute()
{
   EventDisposition disposition = cDELETE;
   if ((mPlatformIndex == 0) ||                         // in global context
       GetSimulation()->PlatformExists(mPlatformIndex)) // in platform (part) context and platform exists
   {
      double simTime = GetTime();
      mContextPtr->ExecuteScript(simTime, mScriptName);
      if (mTimeInterval > 0.0)
      {
         SetTime(simTime + mTimeInterval);
         disposition = cRESCHEDULE;
      }
   }
   return disposition;
}
