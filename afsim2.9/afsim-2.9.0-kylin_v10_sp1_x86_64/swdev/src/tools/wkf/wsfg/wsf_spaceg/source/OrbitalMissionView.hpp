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

#ifndef ORBITALMISSIONVIEW_HPP
#define ORBITALMISSIONVIEW_HPP

#include <QTreeView>

namespace wsfg
{

namespace spaceg
{

//! A view onto the mission sequence.
class OrbitalMissionView : public QTreeView
{
   Q_OBJECT

public:
   explicit OrbitalMissionView(QWidget* aParentPtr)
      : QTreeView(aParentPtr)
      , mMoveUpActionPtr{nullptr}
      , mMoveDownActionPtr{nullptr}
      , mRemoveActionPtr{nullptr}
      , mTransformActions{}
   {
   }

   ~OrbitalMissionView() override = default;

   void Configure(const std::map<QString, QAction*>& aTransformActions,
                  QAction*                           aMoveUpPtr,
                  QAction*                           aMoveDownPtr,
                  QAction*                           aRemovePtr);
   void setModel(QAbstractItemModel* aModelPtr) override;

signals:
   void CurrentChanged(const QModelIndex& aCurrent);

protected:
   void currentChanged(const QModelIndex& aCurrent, const QModelIndex& aPrevious) override;
   void contextMenuEvent(QContextMenuEvent* aEvent) override;

private:
   QMenu* CreateContextMenu(const QModelIndex& aCurrent);
   void   CreateTransformSubmenu(const QModelIndex& aCurrent, QMenu* aMenu);

   QAction*                    mMoveUpActionPtr;
   QAction*                    mMoveDownActionPtr;
   QAction*                    mRemoveActionPtr;
   std::map<QString, QAction*> mTransformActions;
};

} // namespace spaceg

} // namespace wsfg

#endif // ORBITALMISSIONVIEW_HPP
