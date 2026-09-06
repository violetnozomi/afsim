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

#ifndef PLATFORMSELECTOR_HPP
#define PLATFORMSELECTOR_HPP

#include <memory>

class QComboBox;
class QStringList;
class QStringListModel;
#include "wsf_spaceg_export.h"

#include <QString>
#include <QWidget>

namespace wkf
{

class Platform;
class PlatformSelector;

} // namespace wkf

namespace wsfg
{

namespace spaceg
{

//! User control for changing the platform which has its mission sequence displayed.
class WSF_SPACEG_EXPORT PlatformSelector : public QWidget
{
   Q_OBJECT

public:
   explicit PlatformSelector(QWidget* aParentPtr = nullptr);
   ~PlatformSelector() override = default;

   QString GetSelectedPlatform() const { return mSelectedPlatform; }
   QString GetRequestedPlatform() const;

   void SelectPlatform(const QString& aPlatformName);

public slots:
   void SetPlatformList(const QStringList& aPlatformList);

   void PlatformAdded(const QString& aAddedPlatformName);
   void PlatformRemoved(const QString& aRemovedPlatformName);

signals:
   void PlatformRequested(const QString& aPlatformName);

private:
   void OnSelectionAccepted(int aIndex);
   void OnPlatformSelectionTool(const wkf::Platform* aPlatformPtr);

private:
   QComboBox*             mComboBoxPtr;
   QStringListModel*      mModelPtr;
   wkf::PlatformSelector* mSelectorButtonPtr;
   QString                mSelectedPlatform{};
};

} // namespace spaceg

} // namespace wsfg

#endif // PLATFORMSELECTOR_HPP
