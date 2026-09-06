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

#ifndef WSFCYBERCOMPOSITETRIGGER_HPP
#define WSFCYBERCOMPOSITETRIGGER_HPP

#include "wsf_cyber_export.h"

#include <memory>
#include <vector>

#include "WsfCyberTrigger.hpp"

namespace wsf
{
namespace cyber
{
namespace trigger
{

class WSF_CYBER_EXPORT CompositeTrigger : public Trigger
{
public:
   enum class Policy
   {
      cAND,
      cOR
   };

   using TriggerFactory = std::function<std::unique_ptr<Trigger>(const std::string&)>;
   CompositeTrigger(WsfScenario& aScenario, TriggerFactory aFactory);
   CompositeTrigger(const CompositeTrigger& aSrc);
   CompositeTrigger(CompositeTrigger&&) = default;
   CompositeTrigger& operator=(const CompositeTrigger&) = delete;
   CompositeTrigger& operator=(CompositeTrigger&&) = default;
   ~CompositeTrigger() override                    = default;

   CompositeTrigger* Clone() const override;
   CompositeTrigger* CloneComponent() const override;
   const char*       GetScriptClassName() const override { return "WsfCyberTriggerComposite"; }

   bool Initialize(double aSimTime) override;
   bool InitializeGlobal(double aSimTime, WsfSimulation& aSim) override;

   bool ProcessInput(UtInput& aInput) override;
   bool Evaluate(double aSimTime) override;

   bool   IsPolicyAND() const { return (mPolicy == Policy::cAND); }
   bool   IsPolicyOR() const { return (mPolicy == Policy::cOR); }
   void   SetPolicy(Policy aPolicy) { mPolicy = aPolicy; }
   size_t NumTriggers() const { return mTriggers.size(); }

   bool     IsTriggerNOT(size_t aIndex) const { return mTriggers.at(aIndex).IsNOT(); }
   void     SetTriggerNOT(size_t aIndex, bool aNOT) { mTriggers.at(aIndex).SetNOT(aNOT); }
   Trigger* GetTrigger(size_t aIndex) const { return mTriggers.at(aIndex).GetTrigger(); }

   //! @name AddTrigger Methods
   //@{
   //! This version of AddTrigger uses the factory. Because of this,
   //! it is only applicable for triggers defined via input that exist
   //! as types able to be instantiated.
   bool AddTrigger(double aSimTime, const std::string& aTriggerName, WsfSimulation& aSim, bool aNOT = false);

   //! This version of AddTrigger uses any existing trigger, including those defined in the
   //! script context and not managed by the simulation.
   bool AddTrigger(double aSimTime, std::unique_ptr<Trigger> aTrigger, WsfSimulation& aSim, bool aNOT = false);
   //@}

   bool RemoveTrigger(const std::string& aTriggerName, bool aNOT = false);
   bool RemoveTrigger(size_t aIndex);
   void ClearTriggers();

private:
   class TriggerWrapper
   {
   public:
      TriggerWrapper(const std::string& aTriggerInput, bool aNot, std::shared_ptr<Trigger> aTrigger = nullptr)
         : mTrigger(std::move(aTrigger))
         , mNot(aNot)
         , mTriggerInput(aTriggerInput)
      {
      }

      TriggerWrapper(const TriggerWrapper& aSrc) = default;
      Trigger* operator->() const { return mTrigger.get(); }
      bool     Evaluate(double aSimTime) const
      {
         if (mNot)
         {
            return !mTrigger->Evaluate(aSimTime);
         }
         return mTrigger->Evaluate(aSimTime);
      }

      bool        IsNOT() const { return mNot; }
      void        SetNOT(bool aNOT) { mNot = aNOT; }
      std::string GetName() const;
      Trigger*    GetTrigger() const { return mTrigger.get(); }
      std::string GetTriggerInput() const { return mTriggerInput; }
      void        SetTrigger(std::shared_ptr<Trigger> aTrigger) { mTrigger = std::move(aTrigger); }

   private:
      std::shared_ptr<Trigger> mTrigger{nullptr};
      bool                     mNot{false};
      std::string              mTriggerInput{};
   };
   Policy                      mPolicy{Policy::cAND};
   TriggerFactory              mFactory{};
   std::vector<TriggerWrapper> mTriggers{};
};

class WSF_CYBER_EXPORT CompositeScriptClass : public ScriptClass
{
public:
   CompositeScriptClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~CompositeScriptClass() override = default;

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(IsPolicyAND);
   UT_DECLARE_SCRIPT_METHOD(IsPolicyOR);
   UT_DECLARE_SCRIPT_METHOD(SetPolicyAND);
   UT_DECLARE_SCRIPT_METHOD(SetPolicyOR);

   UT_DECLARE_SCRIPT_METHOD(GetTriggersSize);
   UT_DECLARE_SCRIPT_METHOD(GetTriggerEntry);
   UT_DECLARE_SCRIPT_METHOD(IsTriggerEntryNOT);

   UT_DECLARE_SCRIPT_METHOD(AddTrigger_1);
   UT_DECLARE_SCRIPT_METHOD(AddTrigger_2);
   UT_DECLARE_SCRIPT_METHOD(RemoveTriggerEntry);
   UT_DECLARE_SCRIPT_METHOD(EraseTriggers);
   UT_DECLARE_SCRIPT_METHOD(SetTriggerEntryNOT);
};

} // namespace trigger
} // namespace cyber
} // namespace wsf

#endif
