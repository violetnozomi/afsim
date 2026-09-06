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

#include "WsfQuantumEvaluator.hpp"

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtScriptDataPack.hpp"
#include "UtScriptExecutor.hpp"
#include "UtStringUtil.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTask.hpp"
#include "script/WsfScriptContext.hpp"


WsfQuantumEvaluatorFactory::WsfQuantumEvaluatorFactory()
{
   Register(ut::make_unique<WsfQuantumEvaluatorSimple>());
   Register(ut::make_unique<WsfQuantumEvaluatorDistance>());
   Register(ut::make_unique<WsfQuantumEvaluatorInterceptTime>());
}

WsfQuantumEvaluatorFactory* WsfQuantumEvaluatorFactory::Instance()
{
   static WsfQuantumEvaluatorFactory instance;
   return &instance;
}

bool WsfQuantumEvaluatorFactory::Register(std::unique_ptr<WsfQuantumEvaluator> aEvaluatorPtr)
{
   if (aEvaluatorPtr != nullptr)
   {
      return mFactoryMap.emplace(aEvaluatorPtr->EvaluatorType(), std::move(aEvaluatorPtr)).second;
   }
   return false;
}

WsfQuantumEvaluator* WsfQuantumEvaluatorFactory::CreateEvaluator(const std::string& evaluatorName)
{
   auto it = mFactoryMap.find(evaluatorName);
   if (it != mFactoryMap.end())
   {
      return it->second->Clone();
   }
   return nullptr;
}

WsfQuantumEvaluatorScript::WsfQuantumEvaluatorScript(WsfScriptContext* aContextPtr, std::string& aScriptMethod)
   : WsfQuantumEvaluator()
   , mType(aScriptMethod)
   , mScriptPtr(nullptr)
   , mContextPtr(&aContextPtr->GetContext())
   , mExecutorPtr(aContextPtr->GetExecutor())
{
   mScriptPtr = mContextPtr->FindScript(mType);
   while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
   {
      mContextPtr = mContextPtr->GetParent();
      mScriptPtr  = mContextPtr->FindScript(mType);
   }

   if (mScriptPtr == nullptr)
   {
      auto out = ut::log::error() << "Unable to find script for quantum evaluator.";
      out.AddNote() << "Type: " << mType;
   }
   else if (!aContextPtr->ValidateScript(mScriptPtr, "double", "WsfQuantumTask, WsfAssetPerception"))
   {
      auto out = ut::log::error() << "Invalid inputs or outputs for script as quantum evaluator.";
      out.AddNote() << "Type: " << mType;
      mScriptPtr = nullptr;
   }
}

// virtual
void WsfQuantumEvaluatorScript::Initialize(double aSimTime, WsfScriptContext* aParentContextPtr)
{
   if (aParentContextPtr != nullptr)
   {
      mContextPtr  = &aParentContextPtr->GetContext();
      mExecutorPtr = aParentContextPtr->GetExecutor();
      mScriptPtr   = mContextPtr->FindScript(mType);
      while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
      {
         mContextPtr = mContextPtr->GetParent();
         mScriptPtr  = mContextPtr->FindScript(mType);
      }
   }
}

// virtual
WsfQuantumEvaluator* WsfQuantumEvaluatorScript::Clone() const
{
   return dynamic_cast<WsfQuantumEvaluator*>(new WsfQuantumEvaluatorScript(*this));
}

WsfQuantumEvaluatorScript::WsfQuantumEvaluatorScript(const WsfQuantumEvaluatorScript& aSrc)
   : mType(aSrc.mType)
   , mContextPtr(aSrc.mContextPtr)
   , mExecutorPtr(aSrc.mExecutorPtr)
{
   mScriptPtr = mContextPtr->FindScript(mType);
   while (mScriptPtr == nullptr && mContextPtr->GetParent() != nullptr)
   {
      mContextPtr = mContextPtr->GetParent();
      mScriptPtr  = mContextPtr->FindScript(mType);
   }
}

// virtual
double WsfQuantumEvaluatorScript::Evaluate(double              aSimTime,
                                           WsfTask*            aTaskPtr,
                                           WsfAssetPerception* aAssetPtr,
                                           WsfLocalTrack*      aTargetTrackPtr)
{
   double evaluation = 1.0; // TODO - should this be zero?
   if (mScriptPtr != nullptr)
   {
      UtScriptData       scriptRetVal(evaluation);
      UtScriptDataPacker scriptArgs(mContextPtr->GetTypes());
      scriptArgs << aTaskPtr << aAssetPtr; // double method( WsfTask, WsfAssetPerception )
      mContextPtr->Execute(mExecutorPtr, mScriptPtr, scriptRetVal, scriptArgs);
      evaluation = scriptRetVal.GetDouble();
   }
   return evaluation;
}

// virtual
double WsfQuantumEvaluatorDistance::Evaluate(double              aSimTime,
                                             WsfTask*            aTaskPtr,
                                             WsfAssetPerception* aAssetPtr,
                                             WsfLocalTrack*      aTargetTrackPtr)
{
   if ((aTargetTrackPtr != nullptr) && aTargetTrackPtr->LocationValid())
   {
      double wcs[3];
      aTargetTrackPtr->GetExtrapolatedLocationWCS(aSimTime, wcs);
      double relWCS[3];
      // relative location
      UtVec3d::Subtract(relWCS, wcs, aAssetPtr->mLocationWCS);
      double distance = UtVec3d::Magnitude(relWCS);

      if (distance > 0)
      {
         return 1.0 / distance;
      }
      else
      {
         return 100.0;
      }
   }
   return 0.0;
}

// virtual
double WsfQuantumEvaluatorInterceptTime::Evaluate(double              aSimTime,
                                                  WsfTask*            aTaskPtr,
                                                  WsfAssetPerception* aAssetPtr,
                                                  WsfLocalTrack*      aTargetTrackPtr)
{
   if ((aTargetTrackPtr != nullptr) && aTargetTrackPtr->LocationValid() && aTargetTrackPtr->VelocityValid())
   {
      double locwcs[3];
      aTargetTrackPtr->GetExtrapolatedLocationWCS(aSimTime, locwcs);
      double velwcs[3];
      aTargetTrackPtr->GetVelocityWCS(velwcs);
      double interceptWCS[3];
      double time = WsfIntercept::Intercept(aAssetPtr->mLocationWCS,
                                            UtVec3d::Magnitude(aAssetPtr->mVelocityWCS),
                                            locwcs,
                                            velwcs,
                                            interceptWCS);
      if (time > 0)
      {
         return 1.0 / time;
      }
   }
   return 0.0;
}
