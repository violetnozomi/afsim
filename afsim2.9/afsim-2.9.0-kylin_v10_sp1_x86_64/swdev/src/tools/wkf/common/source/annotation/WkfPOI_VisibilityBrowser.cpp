// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfPOI_VisibilityBrowser.hpp"

#include <QFontMetrics>
#include <QToolButton>

#include "UtQtMemory.hpp"
#include "VaEntity.hpp"
#include "WkfAnnotationDecorator.hpp"
#include "WkfAttachmentRangeRing.hpp"
#include "WkfBullseye.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

Annotation::POI_VisibilityWidget::POI_VisibilityWidget(QWidget*        aParent /*= nullptr*/,
                                                       Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QDockWidget(aParent, aFlags)
{
   mUI.setupUi(this);

   // Force the buttons to actually be the same size, since layouts and size policy manipulation doesn't appear to do it
   QFontMetrics fm(QApplication::font());
   int          cWIDTH_STRING = fm.horizontalAdvance("Show ") + 4;
   mUI.mShowBullseyeButton->setFixedWidth(cWIDTH_STRING);
   mUI.mHideBullseyeButton->setFixedWidth(cWIDTH_STRING);
   mUI.mShowPOI_Button->setFixedWidth(cWIDTH_STRING);
   mUI.mHidePOI_Button->setFixedWidth(cWIDTH_STRING);

   mTogglePOI_Action = ut::qt::make_qt_ptr<wkf::Action>(QString("Toggle Point of Interest Visibility"),
                                                        this,
                                                        QKeySequence("CTRL+T, CTRL+P"));
   mToggleBullseyeAction =
      ut::qt::make_qt_ptr<wkf::Action>(QString("Toggle Bullseye Visibility"), this, QKeySequence("CTRL+T, CTRL+B"));

   connect(mTogglePOI_Action, &QAction::triggered, this, [this]() { SetPOI_Visibility(!mPOI_Visible); });
   connect(mToggleBullseyeAction, &QAction::triggered, this, [this]() { SetBullseyeVisibility(!mBullseyeVisible); });

   connect(mUI.mShowPOI_Button, &QToolButton::clicked, this, [this]() { SetPOI_Visibility(true); });
   connect(mUI.mHidePOI_Button, &QToolButton::clicked, this, [this]() { SetPOI_Visibility(false); });
   connect(mUI.mShowBullseyeButton, &QToolButton::clicked, this, [this]() { SetBullseyeVisibility(true); });
   connect(mUI.mHideBullseyeButton, &QToolButton::clicked, this, [this]() { SetBullseyeVisibility(false); });
}

QList<wkf::Action*> Annotation::POI_VisibilityWidget::GetActions() const
{
   QList<wkf::Action*> list;
   list << mTogglePOI_Action << mToggleBullseyeAction;
   return list;
}

void Annotation::POI_VisibilityWidget::SetPOI_Visibility(bool aVisible)
{
   vespa::VaEntityList list;
   vaEnv.GetEntityListOfType<wkf::PointOfInterest>(list);
   for (auto& it : list)
   {
      if (!it->IsA_TypeOf<wkf::Bullseye>())
      {
         UpdateVisibility(it, aVisible);
      }
   }
   mPOI_Visible = aVisible;
}

void Annotation::POI_VisibilityWidget::SetBullseyeVisibility(bool aVisible)
{
   vespa::VaEntityList list;
   vaEnv.GetEntityListOfType<wkf::Bullseye>(list);
   for (auto& it : list)
   {
      UpdateVisibility(it, aVisible);
   }
   mBullseyeVisible = aVisible;
}

void Annotation::POI_VisibilityWidget::UpdateVisibility(vespa::VaEntity* aEnt, bool aVisible)
{
   wkf::Viewer* viewer = vaEnv.GetStandardViewer();
   aEnt->SetVisibilityContribution(aVisible, vespa::VaEntity::UserOptionHidden, viewer);
   aEnt->UpdateVisibility();
   if (!aVisible && aEnt->GetSelected(viewer))
   {
      aEnt->SetSelected(viewer, false);
   }
}
