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
*   File: WsfBMTrackScriptCallbackHandler.hpp
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
*   Abstract: This class is used for dispatching track handler callbacks from
*    an external source to a registered processor script function. This is to
*    get around a limitation where the WsfPerceptionProcessor is silently deleting
*    tracks when it gets a delete vs. notifying users of the processor.
*
*    Beware of a race condition between this invocation and the any other processors.
*    It is not safe to assume you can reference the perception processor for this
*    track because you don't know the order of the callback invocations.
*
*******************************************************************************/

#ifndef WSF_BM_TRACK_SCRIPT_CALLBACK_HANDLER_HPP
#define WSF_BM_TRACK_SCRIPT_CALLBACK_HANDLER_HPP

#include <WsfBMCommon.hpp>
#include <script/WsfScriptContext.hpp>
#include <script/WsfScriptProcessor.hpp>
#include <UtScript.hpp>
#include <WsfLocalTrack.hpp>

class WsfBMTrackScriptCallbackHandler
{
   public:
      WsfBMTrackScriptCallbackHandler();

      WSF_IADS_C2_LIB_EXPORT virtual bool Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun, const std::string& ret_type, const std::string& arguments_signature);

      WSF_IADS_C2_LIB_EXPORT virtual bool Initialized() const;

   protected:
      WsfScriptContext& GetContext();

      bool                m_initialized;
      WsfScriptProcessor* m_proc;
      UtScript*           m_script;
};

// Can be used for callbacks that get invokes with only a local track (track drop is an example)
class WsfBMTrackScriptLocalHandler : public WsfBMTrackScriptCallbackHandler
{
   public:
      WsfBMTrackScriptLocalHandler();

      bool Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun);

      bool Invoke(double aSimTime, const WsfLocalTrack* aLocalTrackPtr);
};

// Used for callbacks that include a local and remote track (local track initiated and updated for instance)
class WsfBMTrackScriptRemoteHandler : public WsfBMTrackScriptCallbackHandler
{
   public:
      WsfBMTrackScriptRemoteHandler();

      bool Initialize(WsfScriptProcessor* proc, const std::string& script_handler_fun);

      bool Invoke(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrack* aRawTrackPtr);
};

#endif
