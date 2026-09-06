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

#include "MoverCreatorLineEdit.hpp"

#include <limits>

#include <QTreeWidgetItem>
#include <QDoubleValidator>

namespace Designer
{

MoverCreatorLineEdit::MoverCreatorLineEdit(QTreeWidgetItem* aTreeWidgetItemPtr)
   : QLineEdit()
   , mTreeWidgetItemPtr(aTreeWidgetItemPtr)
{
   setValidator(new QDoubleValidator(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 10));
}

MoverCreatorLineEdit::~MoverCreatorLineEdit()
{

}

void MoverCreatorLineEdit::mouseReleaseEvent(QMouseEvent* aEvent)
{
   if (mTreeWidgetItemPtr != nullptr)
   {
      if (mTreeWidgetItemPtr->treeWidget())
      {
         mTreeWidgetItemPtr->treeWidget()->setCurrentItem(nullptr);
      }
      mTreeWidgetItemPtr->setSelected(true);
   }
   emit clicked();
}

} // namespace Designer
