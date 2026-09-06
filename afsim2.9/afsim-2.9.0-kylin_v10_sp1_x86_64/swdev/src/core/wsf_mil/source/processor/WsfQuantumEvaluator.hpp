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

#ifndef WSFQUANTUMEVALUATOR_HPP
#define WSFQUANTUMEVALUATOR_HPP

#include <map>
#include <memory>

#include "UtCloneablePtr.hpp"
class UtInput;
class UtScript;
class UtScriptExecutor;
#include "wsf_mil_export.h"
class WsfAssetPerception;
class WsfLocalTrack;
#include "WsfObject.hpp"
class WsfTask;
class WsfScriptContext;


class WSF_MIL_EXPORT WsfQuantumEvaluator : public WsfObject
{
public:
   virtual std::string  EvaluatorType()                                                            = 0;
   virtual void         Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) = 0;
   WsfQuantumEvaluator* Clone() const override                                                     = 0;
   virtual double       Evaluate(double              aSimTime,
                                 WsfTask*            aTaskPtr,
                                 WsfAssetPerception* aAssetPtr,
                                 WsfLocalTrack*      aTargetTrackPtr = nullptr)                         = 0;

protected:
   WsfQuantumEvaluator() = default;
};


class WSF_MIL_EXPORT WsfQuantumEvaluatorFactory
{
public:
   ~WsfQuantumEvaluatorFactory()                                 = default;
   WsfQuantumEvaluatorFactory(WsfQuantumEvaluatorFactory&& aSrc) = delete;
   WsfQuantumEvaluatorFactory& operator=(const WsfQuantumEvaluatorFactory&) = delete;
   WsfQuantumEvaluatorFactory& operator=(WsfQuantumEvaluatorFactory&&) = delete;

   static WsfQuantumEvaluatorFactory* Instance();
   bool                               Register(std::unique_ptr<WsfQuantumEvaluator> aEvaluatorPtr);
   WsfQuantumEvaluator*               CreateEvaluator(const std::string& evaluatorName);
   const std::map<std::string, ut::CloneablePtr<WsfQuantumEvaluator>>& Evaluators() const { return mFactoryMap; }

private:
   WsfQuantumEvaluatorFactory();
   WsfQuantumEvaluatorFactory(const WsfQuantumEvaluatorFactory& aSrc) = default;

   std::map<std::string, ut::CloneablePtr<WsfQuantumEvaluator>> mFactoryMap;
};


class WSF_MIL_EXPORT WsfQuantumEvaluatorScript : public WsfQuantumEvaluator
{
public:
   WsfQuantumEvaluatorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod);
   WsfQuantumEvaluatorScript& operator=(const WsfQuantumEvaluatorScript&) = delete;
   ~WsfQuantumEvaluatorScript() override                                  = default;
   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override;
   WsfQuantumEvaluator* Clone() const override;
   std::string          EvaluatorType() override { return mType; }
   virtual bool         Valid() { return (mScriptPtr != nullptr); }
   // run the user defined script to evaluate
   double Evaluate(double              aSimTime,
                   WsfTask*            aTaskPtr,
                   WsfAssetPerception* aAssetPtr,
                   WsfLocalTrack*      aTargetTrackPtr = nullptr) override;

private:
   WsfQuantumEvaluatorScript(const WsfQuantumEvaluatorScript& aSrc);
   std::string       mType;
   UtScript*         mScriptPtr;
   UtScriptContext*  mContextPtr;
   UtScriptExecutor* mExecutorPtr;
};


class WSF_MIL_EXPORT WsfQuantumEvaluatorSimple : public WsfQuantumEvaluator
{
public:
   WsfQuantumEvaluatorSimple()
      : WsfQuantumEvaluator()
   {
   }
   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumEvaluator* Clone() const override
   {
      return dynamic_cast<WsfQuantumEvaluator*>(new WsfQuantumEvaluatorSimple());
   } // no member data, just create a new one
   std::string EvaluatorType() override { return "simple"; }
   double      Evaluate(double              aSimTime,
                        WsfTask*            aTaskPtr,
                        WsfAssetPerception* aAssetPtr,
                        WsfLocalTrack*      aTargetTrackPtr = nullptr) override
   {
      return 1.0;
   }
};


class WSF_MIL_EXPORT WsfQuantumEvaluatorDistance : public WsfQuantumEvaluator
{
public:
   WsfQuantumEvaluatorDistance()
      : WsfQuantumEvaluator()
   {
   }
   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumEvaluator* Clone() const override
   {
      return dynamic_cast<WsfQuantumEvaluator*>(new WsfQuantumEvaluatorDistance());
   } // no member data, just create a new one
   std::string EvaluatorType() override { return "distance"; }
   double      Evaluate(double              aSimTime,
                        WsfTask*            aTaskPtr,
                        WsfAssetPerception* aAssetPtr,
                        WsfLocalTrack*      aTargetTrackPtr = nullptr) override;
};


class WSF_MIL_EXPORT WsfQuantumEvaluatorInterceptTime : public WsfQuantumEvaluator
{
public:
   WsfQuantumEvaluatorInterceptTime()
      : WsfQuantumEvaluator()
   {
   }
   void                 Initialize(double aSimTime, WsfScriptContext* aParentContextPtr = nullptr) override {}
   WsfQuantumEvaluator* Clone() const override
   {
      return dynamic_cast<WsfQuantumEvaluator*>(new WsfQuantumEvaluatorInterceptTime());
   } // no member data, just create a new one
   std::string EvaluatorType() override { return "intercept_time"; }
   double      Evaluate(double              aSimTime,
                        WsfTask*            aTaskPtr,
                        WsfAssetPerception* aAssetPtr,
                        WsfLocalTrack*      aTargetTrackPtr = nullptr) override;
};

#endif
