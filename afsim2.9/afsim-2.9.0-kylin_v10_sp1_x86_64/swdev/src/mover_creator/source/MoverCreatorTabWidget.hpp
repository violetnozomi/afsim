// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MOVER_CREATOR_TAB_WIDGET_HPP
#define MOVER_CREATOR_TAB_WIDGET_HPP

#include <QTabWidget>

namespace Designer
{
   class MoverCreatorTabWidget : public QTabWidget
   {
      public:
         explicit MoverCreatorTabWidget(QWidget* aParent = 0);
         ~MoverCreatorTabWidget();

         bool eventFilter(QObject* aObj, QEvent* aEvent);
   };
}

#endif // MOVER_CREATOR_TAB_WIDGET_HPP
