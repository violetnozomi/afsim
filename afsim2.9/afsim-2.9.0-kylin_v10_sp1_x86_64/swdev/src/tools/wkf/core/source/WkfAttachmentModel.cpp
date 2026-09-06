// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfAttachmentModel.hpp"

#include "UtMath.hpp"
#include "VaEnvironment.hpp"
#include "VaModelDatabase.hpp"
#include "VaTimeManager.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::AttachmentModel::AttachmentModel(vespa::VaEntity& aParent, vespa::VaViewer* aViewerPtr)
   : VaAttachmentModel(aParent, aViewerPtr)
   , mDeletionTime(-1.0)
   , mSaturateTime(-1.0)
{
}

void wkf::AttachmentModel::UpdateFrame(double aTime)
{
   VaAttachmentModel::UpdateFrame(aTime);

   // We don't know for sure if aTime is wall clock time or simulation time, so just grab wall clock to be sure
   double wallTime = vaEnv.GetTime()->GetWallClockTime();

   if (IsVisible())
   {
      if (mSaturateTime >= 0.0)
      {
         double t = wallTime - mSaturateTime;
         float  m = mTeamColorP * osg::Vec4(0.2989f, 0.5870f, 0.1140f, 0.0f); // weights from CCIR 601 spec
         float  s = 0.75f * expf(-t) + 0.25f;                                 // saturation
         float  a = s;                                                        // alpha
         if (mDeletionTime >= 0.0)
         {
            a = .25f * cosf(UtMath::cPI * (wallTime - mDeletionTime)) + 0.5f;
         }

         mTeamColor.set(s * mTeamColorP[0] + (1 - s) * m,
                        s * mTeamColorP[1] + (1 - s) * m,
                        s * mTeamColorP[2] + (1 - s) * m,
                        a);

         vaEnv.GetModelDatabase()->UpdateModel(mModelId, mModelMatrix, mViewMatrix, mTeamColor);
      }
   }
}

void wkf::AttachmentModel::SetColor(const UtColor& aColor)
{
   VaAttachmentModel::SetColor(aColor);
   mTeamColorP = mTeamColor;
}

void wkf::AttachmentModel::PrivateInitializeCallbacks()
{
   mCallbacks.Add(Observer::PlatformBroken.Connect(&AttachmentModel::PlatformBrokenCB, this));
   mCallbacks.Add(Observer::PlatformPendingDeletion.Connect(&AttachmentModel::PlatformPendingDeletionCB, this));
}

void wkf::AttachmentModel::PlatformBrokenCB(Platform* aPlatformPtr)
{
   if (aPlatformPtr->GetUniqueId() == GetParent().GetUniqueId())
   {
      if (mSaturateTime < 0.0)
      {
         mSaturateTime = vaEnv.GetTime()->GetWallClockTime();
      }
   }
}

void wkf::AttachmentModel::PlatformPendingDeletionCB(Platform* aPlatformPtr)
{
   if (aPlatformPtr->GetUniqueId() == GetParent().GetUniqueId())
   {
      double wallTime = vaEnv.GetTime()->GetWallClockTime();
      if (mDeletionTime < 0.0)
      {
         mDeletionTime = wallTime;
      }
      if (mSaturateTime < 0.0)
      {
         mSaturateTime = wallTime;
      }
   }
}
