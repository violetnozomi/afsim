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

#include "WsfSensorModeList.hpp"

#include "WsfSensor.hpp"
#include "WsfSensorBeam.hpp"
#include "WsfSensorMode.hpp"

// =================================================================================================
WsfSensorModeList::WsfSensorModeList(WsfSensorMode* aTemplatePtr)
   : WsfModeList(aTemplatePtr)
   , mSensorPtr(nullptr)
{
}

// =================================================================================================
WsfSensorModeList::WsfSensorModeList(const WsfSensorModeList& aSrc)
   : WsfModeList(aSrc)
   , mSensorPtr(nullptr)
{
}

// =================================================================================================
//! A special copy constructor used by WsfCompositeSensor.
WsfSensorModeList::WsfSensorModeList(const WsfSensorModeList&           aSrc,
                                     WsfSensorMode*                     aTemplatePtr,
                                     const std::vector<WsfSensorMode*>& aModeList)
   : WsfModeList(aTemplatePtr)
   , mSensorPtr(nullptr)
{
   // We can't use the standard copy constructor as we are using the input modes directly.

   mModeList.resize(aModeList.size());
   for (size_t i = 0; i < aModeList.size(); ++i)
   {
      mModeList[i] = static_cast<WsfMode*>(aModeList[i]);
   }

   mExplicitModeUsed    = aSrc.mExplicitModeUsed;
   mImplicitModeUsed    = aSrc.mImplicitModeUsed;
   mCanMultiSelect      = aSrc.mCanMultiSelect;
   mInitialModeNameId   = aSrc.mInitialModeNameId;
   mIsInitialized       = false;
   mIsTurnedOn          = false;
   mCurrentModeIndex    = 0;
   mModeSelectDelay     = aSrc.mModeSelectDelay;
   mModeChangeScheduled = aSrc.mModeChangeScheduled;
}

// =================================================================================================
void WsfSensorModeList::AddMode(WsfMode* aModePtr)
{
   WsfModeList::AddMode(aModePtr);
   WsfSensorMode* modePtr = dynamic_cast<WsfSensorMode*>(aModePtr);
   if (modePtr != nullptr)
   {
      modePtr->mSensorPtr = mSensorPtr;
      for (unsigned int beamIndex = 0; beamIndex < modePtr->GetBeamCount(); ++beamIndex)
      {
         WsfSensorBeam* beamPtr(modePtr->GetBeamEntry(beamIndex));
         if (beamPtr != nullptr)
         {
            beamPtr->mSensorModePtr = modePtr;
            beamPtr->mBeamIndex     = beamIndex;
         }
      }
   }
}

// =================================================================================================
void WsfSensorModeList::SetSensor(WsfSensor* aSensorPtr)
{
   mSensorPtr = aSensorPtr;

   WsfSensorMode* templatePtr = dynamic_cast<WsfSensorMode*>(mTemplatePtr);
   if (templatePtr != nullptr)
   {
      templatePtr->mSensorPtr = aSensorPtr;
      for (unsigned int beamIndex = 0; beamIndex < templatePtr->GetBeamCount(); ++beamIndex)
      {
         WsfSensorBeam* beamPtr(templatePtr->GetBeamEntry(beamIndex));
         if (beamPtr != nullptr)
         {
            beamPtr->mSensorModePtr = templatePtr;
            beamPtr->mBeamIndex     = beamIndex;
         }
      }
   }

   for (size_t i = 0; i < GetModeCount(); ++i)
   {
      WsfSensorMode* modePtr = dynamic_cast<WsfSensorMode*>(GetModeEntry(i));
      if (modePtr != nullptr)
      {
         modePtr->mSensorPtr = aSensorPtr;
         for (size_t beamIndex = 0; beamIndex < modePtr->GetBeamCount(); ++beamIndex)
         {
            WsfSensorBeam* beamPtr(modePtr->GetBeamEntry(beamIndex));
            if (beamPtr != nullptr)
            {
               beamPtr->mSensorModePtr = modePtr;
               beamPtr->mBeamIndex     = beamIndex;
            }
         }
      }
   }
}
