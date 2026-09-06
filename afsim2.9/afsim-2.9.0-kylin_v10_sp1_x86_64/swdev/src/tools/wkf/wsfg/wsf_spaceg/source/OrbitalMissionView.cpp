// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "OrbitalMissionView.hpp"

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QItemSelection>
#include <QMenu>

#include "AstrolabeConfig.hpp"
#include "OrbitalMissionModel.hpp"

namespace wsfg
{

namespace spaceg
{

//! Configure the view with the actions modifying the events in the mission sequence.
void OrbitalMissionView::Configure(const std::map<QString, QAction*>& aTransformActions,
                                   QAction*                           aMoveUpPtr,
                                   QAction*                           aMoveDownPtr,
                                   QAction*                           aRemovePtr)
{
   mMoveUpActionPtr   = aMoveUpPtr;
   mMoveDownActionPtr = aMoveDownPtr;
   mRemoveActionPtr   = aRemovePtr;

   for (const auto& iter : aTransformActions)
   {
      mTransformActions[iter.first] = iter.second;
   }
}

void OrbitalMissionView::setModel(QAbstractItemModel* aModelPtr)
{
   QTreeView::setModel(aModelPtr);
   if (aModelPtr)
   {
      header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   }
}

void OrbitalMissionView::currentChanged(const QModelIndex& aCurrent, const QModelIndex& aPrevious)
{
   emit CurrentChanged(aCurrent);
   QTreeView::currentChanged(aCurrent, aPrevious);
}

//! Create a context menu for modifying the event under the cursor.
void OrbitalMissionView::contextMenuEvent(QContextMenuEvent* aEvent)
{
   QModelIndex indexOfClick = indexAt(aEvent->pos());
   if (indexOfClick.isValid())
   {
      QMenu* menuPtr = CreateContextMenu(indexOfClick);
      if (menuPtr != nullptr)
      {
         menuPtr->exec(aEvent->globalPos());
         aEvent->accept();
         delete menuPtr;
      }
   }
}

QMenu* OrbitalMissionView::CreateContextMenu(const QModelIndex& aCurrent)
{
   QMenu* retvalPtr{nullptr};

   OrbitalMissionModel* modelPtr = dynamic_cast<OrbitalMissionModel*>(model());
   if (modelPtr && aCurrent != modelPtr->GetSequenceIndex())
   {
      retvalPtr = new QMenu{};

      QMenu* submenu = retvalPtr->addMenu(QString{"Transform into"});
      CreateTransformSubmenu(aCurrent, submenu);
      retvalPtr->addSeparator();

      retvalPtr->addAction(mMoveUpActionPtr);
      retvalPtr->addAction(mMoveDownActionPtr);
      retvalPtr->addAction(mRemoveActionPtr);
   }

   return retvalPtr;
}

void OrbitalMissionView::CreateTransformSubmenu(const QModelIndex& aCurrent, QMenu* aMenu)
{
   OrbitalMissionModel* modelPtr    = dynamic_cast<OrbitalMissionModel*>(model());
   QString              currentType = modelPtr->GetItemEventType(aCurrent);

   for (auto& iter : mTransformActions)
   {
      if (iter.first != currentType)
      {
         aMenu->addAction(iter.second);
      }
   }
}

} // namespace spaceg

} // namespace wsfg
