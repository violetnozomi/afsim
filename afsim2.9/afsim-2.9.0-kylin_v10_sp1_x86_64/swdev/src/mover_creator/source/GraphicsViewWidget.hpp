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

#ifndef GRAPHICSVIEW_WIDGET_HPP
#define GRAPHICSVIEW_WIDGET_HPP

#include <QGraphicsView>

namespace Ui
{
   class GraphicsViewWidget;
}

namespace Designer
{
   class GraphicsViewWidget : public QGraphicsView
   {
         Q_OBJECT
      public:
         explicit GraphicsViewWidget(QWidget* aParent);
         ~GraphicsViewWidget();

      private:
         Ui::GraphicsViewWidget* mUIPtr;
   };
}

#endif // !GRAPHICSVIEW_WIDGET_HPP
