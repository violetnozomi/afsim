// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_COMPUTERPART_HPP
#define L16_COMPUTERPART_HPP

#include "wsf_l16_export.h"

#include <list>
#include <map>

#include "IntervalUpdater.hpp"
#include "SlotParameters.hpp"
class UtCallback;
#include "UtCallbackHolder.hpp"
#include "UtInput.hpp"
class UtScript;
class UtScriptClass;
class WsfCommandChain;
#include "WsfObject.hpp"
class WsfPlatform;
class WsfScriptContext;
class WsfScenario;

namespace wsf
{
namespace comm
{
class Comm;
}
} // namespace wsf

namespace WsfL16
{
class ComputerPart;
class ComputerProcessor;
namespace Messages
{
class InitialBase;
}

//! The computer part represents a function of the l16 computer (ComputerProcessor).
//! Typical functions include PPLI and Surveillance, system status, weapon coordination,
//! and scriptable (see associated classes that derive from ComputerPart).
class WSF_L16_EXPORT ComputerPart : public WsfObject
{
public:
   ComputerPart(WsfScenario& aScenario, const std::string& aClassName = "WsfProcessor");
   ComputerPart(const ComputerPart& aSrc);
   ~ComputerPart() override;

   ComputerPart* Clone() const override = 0;

   virtual bool Initialize(double aSimTime, ComputerProcessor& aComputer);

   //! Handle an incoming message
   virtual void HandleReceive(double aSimTime, const Messages::InitialBase& aMessage) = 0;

   //! This method is called when the a slot is open
   virtual void HandleSend(double /*aSimTime*/) {}

   void SendJMessage(double aSimTime, Messages::InitialBase* aMessage);

   bool         ProcessInput(UtInput& aInput) override;
   virtual bool IsNetworkEnabled() { return false; }

   void SetDebug(bool aDebugOn) { mDebug = aDebugOn; }
   bool Debug() { return mDebug; }

   void SetPlatform(WsfPlatform* aPlatformPtr);

   WsfCommandChain* GetCommandChain() const { return mCommandChainPtr; }

   ComputerProcessor* GetComputer() { return mComputer; }

   WsfPlatform*   GetPlatform() { return mPlatformPtr; }
   WsfSimulation* GetSimulation();

   const SlotParameters& GetSlotParameters() const { return mSlotParameters; }

   int DebugMore() { return mDebugMore; }

   void SetSendInterval(double aValue) { mSendInterval.SetUpdateInterval(aValue); }

protected:
   double ProcessSendInterval(UtInput& aInput) const;

protected:
   IntervalUpdater  mSendInterval;
   int              mDebugMore;
   UtCallbackHolder mCallbacks;
   SlotParameters   mSlotParameters;

   double GetSendInterval() { return mSendInterval.GetUpdateInterval(); }

   WsfScriptContext* mContextPtr;

   struct ScriptData
   {
      UtScript*      mScript;
      UtScriptClass* mMessageScriptClass;
   };

   typedef std::map<int, ScriptData> ScriptMap;
   ScriptMap                         mSendScripts;

private:
   ComputerProcessor* mComputer;
   wsf::comm::Comm*   mCommPtr;
   WsfStringId        mCommandChainId;
   WsfCommandChain*   mCommandChainPtr;
   UtCallback*        mCallbackPtr;
   WsfPlatform*       mPlatformPtr;
   bool               mDebug;
};
} // namespace WsfL16

#endif
