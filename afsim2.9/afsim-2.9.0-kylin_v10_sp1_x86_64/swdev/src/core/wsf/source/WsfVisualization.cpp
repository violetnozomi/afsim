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

#include "WsfVisualization.hpp"

#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptDataPack.hpp"
#include "WsfPlatform.hpp"
#include "script/WsfScriptContext.hpp"

UtCallbackListN<void(double, WsfVisualization::Behavior)> WsfVisualization::BehaviorChange;
UtCallbackListN<void(double, WsfPlatform*, bool)>         WsfVisualization::VisibleChange;
WsfVisualization*                                         WsfVisualization::mInstancePtr = nullptr;


WsfVisualization& WsfVisualization::GetInstance()
{
   if (mInstancePtr == nullptr)
   {
      mInstancePtr = new WsfVisualization();
   }
   return *mInstancePtr;
}

// static
void WsfVisualization::Destroy()
{
   delete mInstancePtr;
   mInstancePtr = nullptr;
}

//! Returns the current behavior type for the platform with the given index.
//! Returns -1 if no behavior was given.
int WsfVisualization::GetBehavior(size_t aPlatformIndex)
{
   int                   behavior = -1;
   BehaviorMap::iterator iter     = mCurrentBehaviors.find(aPlatformIndex);
   if (iter != mCurrentBehaviors.end())
   {
      behavior = iter->second.mBehaviorType;
   }
   return behavior;
}

//! Sets the behavior to be visualized for a platform.
void WsfVisualization::SetBehavior(double aSimTime, Behavior aBehavior)
{
   mCurrentBehaviors[aBehavior.mPlatformPtr->GetIndex()] = aBehavior;
   BehaviorChange(aSimTime, aBehavior);
}

WsfVisualization::Behavior::Behavior()
   : mPlatformPtr()
   , mBehaviorType()
   , mAnimationOffsetTime()
   , mAnimationSpeed()
{
}

class WSF_EXPORT WsfScriptVisualizationClass : public UtScriptClass
{
public:
   WsfScriptVisualizationClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(SetVisible);
   UT_DECLARE_SCRIPT_METHOD(SetBehavior);
   UT_DECLARE_SCRIPT_METHOD(Behavior);
};

// static
std::unique_ptr<UtScriptClass> WsfVisualization::CreateScriptClass(UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptVisualizationClass>("WsfVisualization", aScriptTypesPtr);
}

WsfScriptVisualizationClass::WsfScriptVisualizationClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);
   mConstructible = false;
   mCloneable     = false;

   AddStaticMethod(ut::make_unique<SetVisible>());
   AddStaticMethod(ut::make_unique<SetBehavior>());
   AddStaticMethod(ut::make_unique<Behavior>());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptVisualizationClass, WsfVisualization, SetVisible, 2, "void", "WsfPlatform,bool")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   double       simTime     = WsfScriptContext::GetTIME_NOW(aContext);
   WsfVisualization::VisibleChange(simTime, platformPtr, aVarArgs[1].GetBool());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptVisualizationClass, WsfVisualization, SetBehavior, 4, "void", "WsfPlatform,int,double,double")
{
   double                     simTime = WsfScriptContext::GetTIME_NOW(aContext);
   WsfVisualization::Behavior b;
   UtScriptDataUnpack(aVarArgs, b.mPlatformPtr, b.mBehaviorType, b.mAnimationSpeed, b.mAnimationOffsetTime);
   WsfVisualization::GetInstance().SetBehavior(simTime, b);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptVisualizationClass, WsfVisualization, Behavior, 1, "int", "WsfPlatform")
{
   WsfPlatform* platformPtr;
   UtScriptDataUnpack(aVarArgs, platformPtr);
   aReturnVal.SetInt(WsfVisualization::GetInstance().GetBehavior(platformPtr->GetIndex()));
}
