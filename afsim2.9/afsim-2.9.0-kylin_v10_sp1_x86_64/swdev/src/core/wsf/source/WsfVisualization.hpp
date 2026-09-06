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

#ifndef WSFVISUALIZATION_HPP
#define WSFVISUALIZATION_HPP

#include "wsf_export.h"

#include <map>
#include <memory>

#include "UtCallback.hpp"
class UtScriptTypes;
class UtScriptClass;
class WsfPlatform;

class WSF_EXPORT WsfVisualization
{
public:
   //! Defines a platform's behavior.
   struct Behavior
   {
      Behavior();
      //! The platform the behavior is describing
      WsfPlatform* mPlatformPtr;
      //! Enumeration of the behavior.  This is visualization specific.
      int mBehaviorType;
      //! Offsets the starting frame of the animation
      float mAnimationOffsetTime;
      //! Multiplier of animation speed.  1.0 is normal speed.
      float mAnimationSpeed;
   };
   typedef std::map<size_t, Behavior> BehaviorMap;

   //! Callback list invoked when a platform changes behavior.
   static UtCallbackListN<void(double, Behavior)> BehaviorChange;
   //! Callback list invoked when a platform changes visibility.
   static UtCallbackListN<void(double, WsfPlatform*, bool)> VisibleChange;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(UtScriptTypes* aScriptTypesPtr);

   static WsfVisualization& GetInstance();
   static void              Destroy();

   int  GetBehavior(size_t aPlatformIndex);
   void SetBehavior(double aSimTime, Behavior aBehavior);

   const BehaviorMap& GetBehaviors() const { return mCurrentBehaviors; }

private:
   static WsfVisualization* mInstancePtr;
   BehaviorMap              mCurrentBehaviors;
};

#endif
