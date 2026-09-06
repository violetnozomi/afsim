// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: WsfBMTrackScriptCallbackHandler.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: This class is used for dispatching drop track handler callbacks from
*    an external source to a registered processor script function. This is to
*    get around a limitation where the WsfPerceptionProcessor is silently deleting
*    tracks when it gets a delete vs. notifying users of the processor.
*
*    Beware of a race condition between this invocation and the any other processors.
*    It is not safe to assume you can reference the perception processor for this
*    track because you don't know the order of the callback invocations.
*
*******************************************************************************/

#include "WsfBMTrackScriptCallbackHandler.hpp"
#include <logger.hpp>
#include <WsfSimulation.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfBMTrackScriptCallbackHandler::WsfBMTrackScriptCallbackHandler()
   : m_initialized(false)
   , m_proc(nullptr)
   , m_script(nullptr)
{
}

extern "C" WSF_IADS_C2_LIB_EXPORT bool WsfBMTrackScriptCallbackHandler::Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun, const std::string& ret_type, const std::string& arguments_signature)
{
   bool success = true;

   auto globalLogger = IADSC2ScenarioExtension::GetGlobalLogger(proc->GetScenario());

   m_proc = proc;
   WsfScriptContext& context = m_proc->GetScriptContext();
   m_script = context.FindScript(script_handler_fun);
   if (m_script)
   {
      success &= context.ValidateScript(m_script, ret_type, arguments_signature);
   }
   else
   {
      HCL_FATAL_LOGGER(globalLogger) << "Error in WsfBMTrackScriptCallbackHandler::Initialize(), processor does not have a " << script_handler_fun << " function...models may not work properly.\n";
      success &= false;
   }

   HCL_DEBUG_LOGGER(globalLogger) << "WsfBMTrackScriptCallbackHandler::Initialize(): Processor context is tied to = " << WsfScriptContext::GetPROCESSOR(context.GetContext())->GetName() << std::endl
      << "WsfBMTrackScriptCallbackHandler::Initialize() invoked..." << success << std::endl;

   // mark as initialized based on result of attachment
   m_initialized = success;

   return success;
}

extern "C" WSF_IADS_C2_LIB_EXPORT bool WsfBMTrackScriptCallbackHandler::Initialized() const
{
   return m_initialized;
}

WsfScriptContext& WsfBMTrackScriptCallbackHandler::GetContext()
{
   return m_proc->GetScriptContext();
}

//////////////////////////////////
// WsfBMTrackScriptLocalHandler //
//////////////////////////////////
WsfBMTrackScriptLocalHandler::WsfBMTrackScriptLocalHandler()
   : WsfBMTrackScriptCallbackHandler()
{
}

bool WsfBMTrackScriptLocalHandler::Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun)
{
   return WsfBMTrackScriptCallbackHandler::Initialize(proc, script_handler_fun, "void", "double, WsfLocalTrack");
}

bool WsfBMTrackScriptLocalHandler::Invoke(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   bool success = true;
   if (!m_proc || !m_script)
   {
      return success = false;
   }

   static UtScriptClass* trackClassPtr = GetContext().GetClass("WsfLocalTrack");
   assert(trackClassPtr != 0);

   ut::script::Data retVal;
   ut::script::DataList argList;

   argList.emplace_back(aSimTime);

   WsfLocalTrack* nonconstTrack = const_cast<WsfLocalTrack*>(aLocalTrackPtr);
   argList.emplace_back(new UtScriptRef(nonconstTrack, trackClassPtr));

   GetContext().ExecuteScript(WsfScriptContext::GetSIMULATION(GetContext().GetContext())->GetSimTime(), m_script, retVal, argList);

   return success;
}

///////////////////////////////////
// WsfBMTrackScriptRemoteHandler //
///////////////////////////////////
WsfBMTrackScriptRemoteHandler::WsfBMTrackScriptRemoteHandler()
   : WsfBMTrackScriptCallbackHandler()
{
}

bool WsfBMTrackScriptRemoteHandler::Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun)
{
   return WsfBMTrackScriptCallbackHandler::Initialize(proc, script_handler_fun, "void", "double, WsfLocalTrack, WsfTrack");
}

bool WsfBMTrackScriptRemoteHandler::Invoke(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr)
{
   bool success = true;
   if (!m_proc || !m_script)
   {
      return success = false;
   }

   static UtScriptClass* localTrackClassPtr = GetContext().GetClass("WsfLocalTrack");
   assert(localTrackClassPtr != 0);
   static UtScriptClass* remoteTrackClassPtr = GetContext().GetClass("WsfTrack");
   assert(remoteTrackClassPtr != 0);

   ut::script::Data retVal;
   ut::script::DataList argList;

   argList.emplace_back(aSimTime);
   {
      WsfLocalTrack* nonconstTrack = const_cast<WsfLocalTrack*>(aLocalTrackPtr);
      argList.emplace_back(new UtScriptRef(nonconstTrack, localTrackClassPtr));
   }

   {
      WsfTrack* nonconstTrack = const_cast<WsfTrack*>(aRawTrackPtr);
      argList.push_back(ut::script::Data(new UtScriptRef(nonconstTrack, remoteTrackClassPtr)));
   }

   GetContext().ExecuteScript(WsfScriptContext::GetSIMULATION(GetContext().GetContext())->GetSimTime(), m_script, retVal, argList);

   return success;
}
