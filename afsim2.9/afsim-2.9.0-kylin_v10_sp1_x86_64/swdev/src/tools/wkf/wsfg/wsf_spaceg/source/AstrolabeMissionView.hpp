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

#ifndef ASTROLABEMISSIONVIEW_HPP
#define ASTROLABEMISSIONVIEW_HPP

#include "wsf_spaceg_export.h"

#include <QWidget>

#include "ui_AstrolabeMissionView.h"

namespace wsfg
{

namespace spaceg
{

class AstrolabeConfig;
class OrbitalMissionModel;

class WSF_SPACEG_EXPORT AstrolabeMissionView : public QWidget
{
   Q_OBJECT

public:
   explicit AstrolabeMissionView(QWidget* aParentPtr = nullptr);
   ~AstrolabeMissionView() override = default;

   void Configure(const AstrolabeConfig& aConfig);
   void SetModel(OrbitalMissionModel* aModelPtr);

   QModelIndex CurrentIndex() const;
   void        SetCurrentIndex(const QModelIndex& aIndex);


signals:
   void CurrentChanged(const QModelIndex& aCurrent, bool aSaveModifications);

private:
   void CreateActions(const AstrolabeConfig& aConfig);
   void CreateConnections();
   void ConfigureToolButtons(const AstrolabeConfig& aConfig);

   void AddEvent(const QString& aType);
   void RemoveEvent();
   void MoveEventUp();
   void MoveEventDown();
   void TransformEvent(const QString& aType);

   void UpdateToolbar(const QModelIndex& aIndex);
   void HandleCurrentChanged(const QModelIndex& aCurrent, bool aSaveModifications);

   Ui::AstrolabeMissionView    mUi;
   std::map<QString, QAction*> mAddEventActions{};
   std::map<QString, QAction*> mTransformEventActions{};
   QAction*                    mMoveEventUpActionPtr{nullptr};
   QAction*                    mMoveEventDownActionPtr{nullptr};
   QAction*                    mRemoveEventActionPtr{nullptr};
   OrbitalMissionModel*        mCurrentModelPtr{nullptr};
};

} // namespace spaceg

} // namespace wsfg

#endif // ASTROLABEMISSIONVIEW_HPP
