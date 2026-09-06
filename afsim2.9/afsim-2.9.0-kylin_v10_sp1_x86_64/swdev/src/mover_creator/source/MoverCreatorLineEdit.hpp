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
#ifndef MOVER_CREATOR_LINE_EDIT_HPP
#define MOVER_CREATOR_LINE_EDIT_HPP

#include <QLineEdit>

class QTreeWidgetItem;

namespace Designer
{
   class MoverCreatorLineEdit : public QLineEdit
   {
         Q_OBJECT
      public:
         explicit MoverCreatorLineEdit(QTreeWidgetItem* aTreeWidgetItemPtr);
         ~MoverCreatorLineEdit();

         void mouseReleaseEvent(QMouseEvent* aEvent);

      signals:
         void clicked();

      private:
         QTreeWidgetItem* mTreeWidgetItemPtr;
   };
}

#endif //MOVER_CREATOR_LINE_EDIT_HPP
