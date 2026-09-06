// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfViewer.hpp"

#include "UtMemory.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotion.hpp"
#include "VaEntity.hpp"
#include "VaUtils.hpp"
#include "WkfAttachmentRubberband.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::Viewer::Viewer(QWidget* aParent, int aViewerType, int aViewerOptions)
   : vespa::VaViewer(aViewerType, aViewerOptions)
   , mParentPtr(aParent)
{
   mEventFilterPtr = ut::make_unique<EventFilter>(this);
}

int wkf::Viewer::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   if (mActiveRubberbandEntityPtr)
   {
      mActiveRubberbandPtr->End();
      ClearRubberband();
      return DontPassOn;
   }
   else
   {
      return vespa::VaViewer::Btn1Click(aMouseX, aMouseY, aState);
   }
}

wkf::AttachmentRubberband* wkf::Viewer::BeginRubberband(const vespa::VaEntity&                            aReference,
                                                        const std::function<void(AttachmentRubberband&)>& aUpdateFunc,
                                                        const std::function<void(const AttachmentRubberband&)>& aFinishFunc,
                                                        const QString& aStatusMessage)
{
   if (mActiveRubberbandEntityPtr)
   {
      ClearRubberband();
   }
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      mActiveRubberbandEntityPtr = new vespa::VaEntity("ruler_anchor");
      scenarioPtr->AddEntity(mActiveRubberbandEntityPtr);
      mActiveRubberbandPtr =
         vespa::make_attachment<AttachmentRubberband>(*mActiveRubberbandEntityPtr, this, aUpdateFunc, aFinishFunc, aReference);
      vespa::VaAttachment::LoadAttachment(*mActiveRubberbandPtr);

      mActiveRubberbandPtr->SetProxy(true);
      mActiveRubberbandPtr->SetSelected(this, true, 0);

      mParentPtr->setFocus(); // Give keyboard focus to dock widget so 'Esc' key can be used to cancel
      mParentPtr->setCursor(Qt::PointingHandCursor);
      // catch right clicks and esc
      wkfEnv.GetMainWindow()->ShowStatusMessage(aStatusMessage + "  Press 'Esc' or right click to cancel...");
      mParentPtr->installEventFilter(mEventFilterPtr.get());
   }
   return mActiveRubberbandPtr;
}

void wkf::Viewer::ClearRubberband()
{
   if (mActiveRubberbandEntityPtr)
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         scenarioPtr->DeleteEntity(mActiveRubberbandEntityPtr);
         mActiveRubberbandEntityPtr = nullptr;
         mActiveRubberbandPtr       = nullptr;
         mParentPtr->unsetCursor();
         wkfEnv.GetMainWindow()->ShowStatusMessage("");
         mParentPtr->removeEventFilter(mEventFilterPtr.get());
      }
   }
}

wkf::PointOfInterest* wkf::Viewer::AddPointOfInterest(const vespa::VaLocationReference& aReference)
{
   wkf::Scenario* scenario = vaEnv.GetStandardScenario();
   if (mViewerPtr && scenario)
   {
      wkf::PointOfInterest* entity = vaEnv.CreateEntity<wkf::PointOfInterest>("pointOfInterest");
      entity->SetPosition(vespa::VaPosition(aReference.GetPosition().GetLat(), aReference.GetPosition().GetLon(), 0.0));
      scenario->AddEntity(entity);
      entity->Initialize();
      entity->Load();
      return entity;
   }
   return nullptr;
}

bool wkf::Viewer::EventFilter::eventFilter(QObject* obj, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::KeyPress)
   {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(aEvent);
      if (keyEvent->key() == Qt::Key_Escape)
      {
         mViewerPtr->ClearRubberband();
      }
   }
   else if (aEvent->type() == QEvent::ContextMenu)
   {
      mViewerPtr->ClearRubberband();
   }
   return false;
}

QString wkf::Viewer::GetViewLink() const
{
   vespa::VaCameraMotionData* md = GetCamera()->GetCameraMotion()->GetMotionData();
   return "{v:" + QString::number(md->mAED[1]) + "," + QString::number(md->mAED[0]) + "," + QString::number(md->mAED[2]) +
          "," + QString::number(90 - md->mYPR[1]) + "," + QString::number(md->mYPR[0]) + "," + "View}";
}
