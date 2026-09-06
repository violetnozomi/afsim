// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef MAPTOOLBAR_HPP
#define MAPTOOLBAR_HPP

#include <QPixmap>
#include <QSettings>
#include <QToolBar>

class QSortFilterProxyModel;
class QStandardItemModel;
class QTreeView;

namespace wkf
{
class Action;
}

namespace Map
{
class ToolBar : public QToolBar
{
public:
   ToolBar();
   ~ToolBar() override = default;

   void         LoadSettings(QSettings& aSettings);
   void         SaveSettings(QSettings& aSettings);
   wkf::Action* GetCaptureAction() const { return mCaptureAction; }

private:
   static constexpr const char* cDYNAMIC_BUTTON_TEXT = "View";
   void                         Clear();
   void                         Capture();
   void                         ContextMenu(QPoint aPos);
   void                         AddCapture(const QString& aLink, const QPixmap& aPM);
   void                         GeoSearch();
   void                         BuildDatabase(const QString&      aFilename,
                                              const QString&      aType,
                                              const QString&      aIcon,
                                              QStandardItemModel& aModel,
                                              double              aTanFOVY);
   void                         MenuItemContextMenu(QTreeView*             aView,
                                                    QStandardItemModel*    aModel,
                                                    QSortFilterProxyModel* aFilterModel,
                                                    const QPoint&          aPoint) const;

   std::vector<std::pair<QString, QPixmap>> mDataMap;
   wkf::Action*                             mCaptureAction;
   QDialog*                                 mGeoSearchDlg{nullptr};
};
} // namespace Map

#endif
