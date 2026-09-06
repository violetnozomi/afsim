// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfVisibilityDockWidget.hpp"

wkf::VisibilityDockWidget::VisibilityDockWidget(QWidget* aParent, Qt::WindowFlags f)
   : QDockWidget(aParent, f)
{
   mUI.setupUi(this);

   setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

   QGridLayout* gridLayout = new QGridLayout;

   mTypeWidget = new TypeVisibilityWidget(mUI.mContainerWidget);
   mTeamWidget = new TeamVisibilityWidget(mUI.mContainerWidget);

   gridLayout->addWidget(mTypeWidget);
   gridLayout->addWidget(mTeamWidget);

   mTypeWidget->hide();

   gridLayout->setMargin(0);
   gridLayout->setSpacing(0);

   mUI.mContainerWidget->setLayout(gridLayout);

   connect(mUI.mComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           &VisibilityDockWidget::VisibilityWidgetChanged);
}

void wkf::VisibilityDockWidget::VisibilityWidgetChanged(int aIndex)
{
   if (aIndex == 0) // Team Visibility
   {
      if (mTypeWidget->isVisible())
      {
         mTypeWidget->hide();
      }
      mTeamWidget->setVisible(true);
   }
   else if (aIndex == 1) // Type Visibility
   {
      if (mTeamWidget->isVisible())
      {
         mTeamWidget->hide();
      }
      mTypeWidget->setVisible(true);
   }
}

void wkf::VisibilityDockWidget::Update()
{
   if (mTypeWidget->isVisible())
   {
      mTypeWidget->Update();
   }
   else if (mTeamWidget->isVisible())
   {
      mTeamWidget->Update();
   }
}
