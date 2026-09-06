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

#ifndef ASTROLABEEVENTDETAILS_HPP
#define ASTROLABEEVENTDETAILS_HPP

#include <map>

class QLabel;
#include <QModelIndex>
class QToolButton;
class QStackedLayout;
#include "wsf_spaceg_export.h"

#include <QString>
#include <QWidget>

namespace wsfg
{

namespace spaceg
{

class AstrolabeConfig;
class ConfigWidget;
class OrbitalMissionModel;

//! This widget provides the means to display the details of the various event
//! types that Astrolabe is capable of editing.
class WSF_SPACEG_EXPORT AstrolabeEventDetails : public QWidget
{
   Q_OBJECT

public:
   explicit AstrolabeEventDetails(QWidget* aParentPtr = nullptr);
   ~AstrolabeEventDetails() override = default;

   void ConfigureEventTypes(const AstrolabeConfig& aConfiguration);

   void SetModel(OrbitalMissionModel* aModelPtr);

   QModelIndex GetCurrentItem() const { return mCurrentItem; }
   void        SetCurrentItem(const QModelIndex& aIndex, bool aSaveModification = true);

public slots:
   void Revert();
   void SaveCurrentModifications();
   void SetEventName(const QString& aName);

private:
   QString SetPanelValues(const QModelIndex& aIndex);

   struct PanelInfo
   {
      int           mPanelIndex;
      ConfigWidget* mConfigWidgetPtr;
      QString       mDisplayName;
   };

   std::map<QString, PanelInfo> mDetailPanels;
   std::map<int, QString>       mIndexToName;
   QStackedLayout*              mLayoutPtr;
   QLabel*                      mEventLabelPtr;
   QToolButton*                 mRevertButtonPtr;
   QWidget*                     mBlankWidgetPtr;
   OrbitalMissionModel*         mModelPtr;
   QModelIndex                  mCurrentItem;
};

} // namespace spaceg

} // namespace wsfg

#endif // ASTROLABEEVENTDETAILS_HPP
