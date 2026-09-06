// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRIGGER_HPP
#define WSFCYBERTRIGGER_HPP

#include "wsf_cyber_export.h"

#include "WsfComponent.hpp"
#include "WsfCyberComponentRoles.hpp"
#include "WsfCyberTriggerInterval.hpp"
#include "WsfObject.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"

//! DO NOT REMOVE this include from the header. Without full knowledge of the
//! template instantiation (WsfPlatformComponent - WsfComponentT<WsfPlatform>),
//! the compiler will not be able to properly inline derived methods for binaries,
//! resulting in multiple equivalent definitions that error upon linking.
#include "WsfPlatform.hpp"

class WsfSimulation;

namespace wsf
{
namespace cyber
{
namespace trigger
{

class WSF_CYBER_EXPORT Trigger : public WsfObject, public WsfPlatformComponent
{
public:
   Trigger(WsfScenario& aScenario);
   ~Trigger() override = default;
   Trigger(const Trigger& aSrc);
   Trigger& operator=(const Trigger& aRhs);

   Trigger* Clone() const override;

   //! @name Component infrastructure methods
   //@{
   WsfComponent* CloneComponent() const override { return Clone(); }
   WsfStringId   GetComponentName() const override { return GetNameId(); }
   const int*    GetComponentRoles() const override;
   void*         QueryInterface(int aRole) override;
   void          ComponentParentChanged(WsfPlatform* aPlatformPtr) override { mPlatformPtr = aPlatformPtr; }
   //@}

   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! The standard virtual Initialize method, for typical use cases where the
   //! trigger is a platform component.
   bool Initialize(double aSimTime) override;

   //! A specialized initialization routine for global triggers.
   virtual bool InitializeGlobal(double aSimTime, WsfSimulation& aSim);

   bool ProcessInput(UtInput& aInput) override;

   bool                         IsDebug() const { return mDebug; }
   bool                         IsExtrapolated() const { return mExtrapolatedTrigger; }
   bool                         HasTriggered() const { return mTriggerActivated; }
   bool                         ShouldInstance() const { return mShouldInstance; }
   void                         SetDebug(bool aDebug) { mDebug = aDebug; }
   void                         SetExtrapolated(bool aExtrapolated) { mExtrapolatedTrigger = aExtrapolated; }
   void                         SetInstanced(bool aShouldInstance) { mShouldInstance = aShouldInstance; }
   void                         AddInterval(const Interval& aInterval) { mIntervals.push_back(aInterval); }
   void                         ClearIntervals() { mIntervals.clear(); }
   Interval::IntervalVec&       GetIntervals() { return mIntervals; }
   const Interval::IntervalVec& GetIntervals() const { return mIntervals; }

   //! @name Script Overload Mutators
   //! @note These methods are only intended to set the script method overloads for this
   //! class externally. These methods require that any named script method indicated in this
   //! way has the correct signature and is located in the global context. If these methods
   //! fail, any currently assigned UtScript reference is not modified.
   //@{
   bool SetOnEvaluate(const std::string& aScriptName);
   bool SetOnExecute(const std::string& aScriptName);
   //@}

   WsfScriptContext& GetScriptContext() { return mContext; }
   UtScriptContext*  GetScriptAccessibleContext() const override { return &mContext.GetContext(); }
   const char*       GetScriptClassName() const override { return "WsfCyberTrigger"; }

   std::pair<bool, double> NextIntervalTime(double aSimTime) const;

   virtual bool Evaluate(double aSimTime);
   virtual void Execute(double aSimTime);

   //! The default behavior of the Evaluate and Execute methods return false if no method has been
   //! specified. For some derived trigger use cases, the usage may be dependent upon the actual
   //! existence of such functions. These methods should be used to identify if such methods
   //! exist and are functional.
   //@{
   virtual bool HasEvaluate() const;
   virtual bool HasExecute() const;
   //@}

   std::string GetIdentifier() const { return mIdentifier; }
   void        SetIdentifier(const std::string& aIdentifier) { mIdentifier = aIdentifier; }

protected:
   WsfPlatform*          mPlatformPtr;
   bool                  mDebug;
   bool                  mExtrapolatedTrigger;
   bool                  mTriggerActivated;
   bool                  mShouldInstance;
   Interval::IntervalVec mIntervals;
   WsfScriptContext      mContext;
   WsfScenario*          mScenarioPtr;

   UtScript*   mOnEvaluate;
   std::string mOnEvaluateGlobalName;
   UtScript*   mOnExecute;
   std::string mOnExecuteGlobalName;

   //! An identifier for use with script instanced triggers with
   //! an unknown use case of being a global or platform trigger.
   std::string mIdentifier;
};

class WSF_CYBER_EXPORT ScriptClass : public WsfScriptObjectClass
{
public:
   ScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   //! Get an existing trigger, either from a platform or from global (typelist).
   //! These are references to the current triggers managed by the simulation.
   UT_DECLARE_SCRIPT_METHOD(GetTrigger_1);
   UT_DECLARE_SCRIPT_METHOD(GetTrigger_2);

   //! Creates a derived instance of a trigger from an existing trigger.
   //! This trigger instance is local to the script context, and will
   //! be deleted after leaving the script scope.
   UT_DECLARE_SCRIPT_METHOD(CreateDerivedTrigger);

   //! Creates a clone of the script trigger, and adds that clone
   //! to the simulation to be managed. Because a clone is used,
   //! the original trigger is still valid, but will not affect
   //! the simulation managed copy. Any subsequent changes will
   //! require the user to call GetTrigger.
   UT_DECLARE_SCRIPT_METHOD(AddGlobalTrigger);
   UT_DECLARE_SCRIPT_METHOD(AddPlatformTrigger);

   //! Accessors for triggers.
   UT_DECLARE_SCRIPT_METHOD(IsExtrapolated);
   UT_DECLARE_SCRIPT_METHOD(HasTriggered);
   UT_DECLARE_SCRIPT_METHOD(IsExecutingTrigger);
   UT_DECLARE_SCRIPT_METHOD(NextIntervalTime_1);
   UT_DECLARE_SCRIPT_METHOD(NextIntervalTime_2);
   UT_DECLARE_SCRIPT_METHOD(GetIntervals);
   UT_DECLARE_SCRIPT_METHOD(GetIdentifier);

   //! Mutators available for triggers via script.
   //! Be warned, modifying the intervals for existing triggers
   //! will only take effect AFTER the first occurring interval
   //! update with the old interval(s). For example, clearing
   //! the intervals for a trigger will not prevent the next occurring
   //! interval evaluation from executing, but will prevent any future
   //! interval evaluations after that point. The same is true for
   //! adding intervals. This also implies that if a trigger does not
   //! have a valid future interval evaluation, changing the intervals
   //! will have NO affect.
   UT_DECLARE_SCRIPT_METHOD(SetExtrapolated);
   UT_DECLARE_SCRIPT_METHOD(AddInterval);
   UT_DECLARE_SCRIPT_METHOD(ClearIntervals);
   UT_DECLARE_SCRIPT_METHOD(SetIdentifier);

   //! Sets the script methods used by the trigger. These must be
   //! declared in the global script context, and have the correct
   //! signature.
   UT_DECLARE_SCRIPT_METHOD(SetOnEvaluate);
   UT_DECLARE_SCRIPT_METHOD(SetOnExecute);
};

} // namespace trigger
} // namespace cyber
} // namespace wsf

WSF_DECLARE_COMPONENT_ROLE_TYPE(wsf::cyber::trigger::Trigger, cWSF_COMPONENT_CYBER_TRIGGER)

#endif
