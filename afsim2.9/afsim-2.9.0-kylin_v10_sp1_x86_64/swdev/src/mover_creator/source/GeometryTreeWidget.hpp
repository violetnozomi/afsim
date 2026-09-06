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

#ifndef GEOMETRY_TREE_WIDGET_HPP
#define GEOMETRY_TREE_WIDGET_HPP

#include <QTreeWidget>

namespace Designer
{
   class GeometryTreeWidget : public QTreeWidget
   {
      public:
         explicit GeometryTreeWidget(QWidget* aParent);
         ~GeometryTreeWidget();

         void keyPressEvent(QKeyEvent* aEvent);
   };
}

#endif // !GEOMETRY_TREE_WIDGET_HPP