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

#ifndef WSFSCRIPTCONTEXT_HPP
#define WSFSCRIPTCONTEXT_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

class UtInput;
class UtScript;
class UtScriptClass;
class UtScriptContext;
class UtScriptEnvironment;
class UtScriptExecutor;
class UtScriptTypes;
#include "WsfEvent.hpp"
class WsfEventPipeInterface;
class WsfMessage;
class WsfPlatform;
class WsfPlatformPart;
class WsfProcessor;
class WsfSimulation;
class WsfScenario;
#include "WsfStringId.hpp"
class WsfTrack;

namespace ut
{
namespace script
{
class Data;
using DataList = std::vector<Data>;
} // namespace script
} // namespace ut
using UtScriptData     = ut::script::Data;
using UtScriptDataList = ut::script::DataList;
//! A common implementation for scripting on a platform or a platform part.
//!
//! This class encapsulates many of the conventions that need to be followed for implementing
//! scripting on a platform or a platform part. In particular, it provides the following:
//!
//! - Implements the command 'script/end_script' and 'script_variables/end_script_variables'.
//! - Implements the command 'on_initialize', which allows a script to be defined when the platform
//!   or platform part is initialized.
//! - Implements the command 'on_update', which allows a script to be defined that gets called
//!   when the Update() method is called.
//! - Internally provides all the necessary UtScript() and UtScriptContext() calls to maintain the
//!   script context, compile scripts and copy construct and delete as necessary.
//! - Maintains the script variables 'PLATFORM', 'TIME_NOW' and optionally an additional variable
//!   that points to the platform part (e.g.: 'PROCESSOR').
//!
//! The user of this class is responsible for the following:
//!
//! - Invoking the ProcessInput() method to process any of the commands defined above.
//! - Invoking the Initialize() method during the platform or platform part Initialize() method.
//! - Invoking the Update() method during the platform part Update() method.

class WSF_EXPORT WsfScriptContext
{
public:
   WsfScriptContext(UtScriptEnvironment* aTypesPtr);
   WsfScriptContext(WsfScriptContext& aParentContext, WsfStringId aPartClassName, const std::string& aPartVarName);
   WsfScriptContext(WsfScriptContext& aParentContext, WsfStringId aObjectClassName);

   WsfScriptContext(const WsfScriptContext& aSrc);
   WsfScriptContext& operator=(const WsfScriptContext& aRhs);

   virtual ~WsfScriptContext();

   virtual bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr, WsfPlatformPart* aPartPtr);

   virtual bool Initialize(WsfSimulation& aSimulation, void* aObjectPtr);
   virtual bool Initialize(double aSimTime, WsfScriptContext& aParentContext, void* aObjectPtr);

   virtual bool Initialize2(double aSimTime);

   virtual bool InitializeGlobalContext(WsfSimulation& aSimulation);

   virtual bool ProcessInput(UtInput& aInput);

   virtual void Update(double aSimTime);

   UtScript* Compile(UtInput& aInput, const std::string& aEndBlockToken = "end_script", const std::string& aNamespace = "");

   UtScript* Compile(const std::string& aScriptName,
                     WsfStringId        aScriptReturnType,
                     UtInput&           aInput,
                     const std::string& aEndBlockToken   = "end_script",
                     const std::string& aNamespace       = "",
                     bool               aIsVariableBlock = false);

   UtScript* CompileImplicitScript(UtInput& aInput, const std::string& aName, WsfStringId aReturnType);

   void CompileVariables(UtInput& aInput);

   void DeclareVariable(WsfStringId aType, const std::string& aName);

   bool ExecuteScript(double aSimTime, WsfStringId aScriptName);

   void ExecuteScript(double aSimTime, UtScript* aScriptPtr);

   bool ExecuteScript(double aSimTime, WsfStringId aScriptName, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs);

   void ExecuteScript(double aSimTime, UtScript* aScriptPtr, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs);

   void ExecuteScript(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs);

   bool ExecuteScript(double                  aSimTime,
                      UtScriptData&           aScriptRetVal,
                      const UtScriptDataList& aScriptArgs,
                      const std::string&      aScriptSource);

   virtual UtScript* FindScript(WsfStringId aScriptName) const;

   //! Return a reference to the real script context.
   UtScriptContext&     GetContext() const { return *mContextPtr; }
   WsfScriptContext&    GetGlobalContext();
   UtScriptTypes*       GetTypes() const;
   UtScriptEnvironment* GetEnvironment() const;
   UtScriptClass*       GetClass(const std::string& aClassName) const;
   UtScriptClass*       GetClass(WsfStringId aClassName) const;
   UtScriptClass*       GetClass(const char* aClassName) const;

   //! Return 'true' if an 'on_update' script is defined.
   bool HasUpdateHandler() const { return (mOnUpdateScriptPtr != nullptr); }

   void SetParent(WsfScriptContext* aParentPtr);
   void SetParent(UtScriptContext* aParentPtr);

   bool ValidateScript(UtScript* aScriptPtr, WsfStringId aReturnType, const std::string& aArgumentTypes) const;

   WsfPlatformPart* GetPart() { return mPlatformPartPtr; }

   static double                 GetTIME_NOW(const UtScriptContext& aContext);
   static WsfPlatform*           GetPLATFORM(const UtScriptContext& aContext);
   static WsfProcessor*          GetPROCESSOR(const UtScriptContext& aContext);
   static WsfMessage*            GetMESSAGE(const UtScriptContext& aContext);
   static WsfTrack*              GetTRACK(const UtScriptContext& aContext);
   static WsfSimulation*         GetSIMULATION(const UtScriptContext& aContext);
   static WsfScenario*           GetSCENARIO(const UtScriptContext& aContext);
   static WsfEventPipeInterface* GetEVENTPIPE(const UtScriptContext& aContext);

   //! An event to execute a script at a specified time.
   class ExecuteTimedEventScript : public WsfEvent
   {
   public:
      ExecuteTimedEventScript()
         : mTimeInterval(0.0)
         , mPlatformIndex(0)
         , mContextPtr(nullptr)
         , mScriptName()
      {
      }
      ExecuteTimedEventScript(double            aSimTime,
                              double            aTimeInterval,
                              WsfPlatform*      aPlatformPtr,
                              WsfScriptContext* aContextPtr,
                              WsfStringId       aScriptName);

      EventDisposition Execute() override;

   private:
      double            mTimeInterval;
      size_t            mPlatformIndex;
      WsfScriptContext* mContextPtr;
      WsfStringId       mScriptName;
   };

   WsfSimulation*     GetSimulation() const;
   const WsfScenario* GetScenario() const;
   UtScriptExecutor*  GetExecutor() const;

protected:
   UtScriptData GetSimTimeP();

   bool AddTimedEvents(double aSimTime);

   void CallScript(UtScript* aScriptPtr, UtScriptData& aScriptRetVal, const UtScriptDataList& aScriptArgs);

   void PostCompile(UtInput& aInput, UtScript* aRawScriptPtr);

   void WriteErrorHeader(UtScript* aScriptPtr) const;

   //! Pointer to my immediate parent WsfScriptContext (nullptr if the global context)
   WsfScriptContext* mParentPtr{nullptr};

   //! The real UtScriptContext that is associated with this object.
   std::unique_ptr<UtScriptContext> mContextPtr;

   //! 'execute at time' scripts.
   struct TimedEvent;
   std::vector<TimedEvent> mTimedEvents;

   WsfSimulation* mSimulationPtr{nullptr};

   //! If not the global context, this is the pointer to the associated platform.
   WsfPlatform* mPlatformPtr{nullptr};

   //! If the context is for a platform part, this is its pointer
   WsfPlatformPart* mPlatformPartPtr{nullptr};

   //! The platform part class name.
   WsfStringId mPartClassName{};

   //! The platform part variable name.
   WsfStringId mPartVarName{};

   //! The class name for a non-platform oriented object.
   WsfStringId mObjectClassName{};

   class ReadTIME_NOW;
   std::unique_ptr<ReadTIME_NOW> mTimeNowFunctionPtr;

   //! A captured pointer to the on_update script (if it exists). It will also be in the 'mScripts' lists.
   //! (NOT for the global context).
   UtScript* mOnUpdateScriptPtr{nullptr};

   //! 'true' if call tracing is enabled.
   bool mScriptCallTrace{false};

   //! 'true' if the user defined a value for script_call_trace
   bool mUserDefinedScriptCallTrace{false};

   //! 'true' if script_print_code is enabled (prints the generated code after compiled)
   bool mScriptPrintCode{false};

   //! 'true' if 'script_expect_error' is specified.
   //! Note that error checking is a 'one-shot' operation and is valid only for the next call to Compile.
   //! It is reset to false upon completion of the call.
   bool mScriptCheckError{false};

   //! The value of 'script_expect_error'.
   bool mScriptExpectError{false};
};

#endif
