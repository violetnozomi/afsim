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

#include "RvBookmarkBrowser.hpp"

#include "WkfUnitsObject.hpp"
#include "WkfUtils.hpp"

namespace
{
const int cMAX_TIP_WIDTH = 40;

QString WordwrapToolTip(const QString& aTip)
{
   QStringList splitTip;
   int         size      = aTip.size();
   int         splitSize = std::min(size, cMAX_TIP_WIDTH);
   for (int i = 0; i < size; i += cMAX_TIP_WIDTH)
   {
      splitTip.push_back(aTip.mid(i, splitSize));
   }
   QString formattedTip = "<table>";
   for (const auto& it : splitTip)
   {
      formattedTip += "<tr><td>" + it + "</td></tr>";
   }
   formattedTip += "</table>";
   return formattedTip;
}
} // namespace

RvTimeController::BookmarkBrowser::BookmarkBrowser(QWidget*        parent /*= nullptr*/,
                                                   Qt::WindowFlags f /*= Qt::WindowFlags()*/)
{
   mUi.setupUi(this);
   mUi.treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
   mUi.treeWidget->setSortingEnabled(false);
   connect(mUi.treeWidget, &QTreeWidget::itemDoubleClicked, this, [&]() { SendBookmark(); });
   connect(&rvEnv, &rv::Environment::ClearScenario, [this]() { this->ResetBookmarks(); });
   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &wkf::UnitsObject::TimeFormatChanged,
           this,
           [&]() { SetBrowserItems(); });
}

void RvTimeController::BookmarkBrowser::AddBookmark(const rv::MsgBookmark& aBookmark)
{
   mBookmarks.emplace(aBookmark);
   SetBrowserItems();
}

void RvTimeController::BookmarkBrowser::ResetBookmarks()
{
   mBookmarks.clear();
   mUi.treeWidget->clear();
}

void RvTimeController::BookmarkBrowser::SendBookmark()
{
   int index = mUi.treeWidget->currentIndex().row();
   if (index < static_cast<int>(mBookmarks.size()) && index >= 0)
   {
      rv::MsgBookmark msg = *std::next(mBookmarks.begin(), index);
      emit            BookmarkSelected(msg);
   }
}

void RvTimeController::BookmarkBrowser::SetBrowserItems()
{
   mUi.treeWidget->clear();
   int longestType = 0;
   for (const auto& it : mBookmarks)
   {
      QTreeWidgetItem* bookmark = new QTreeWidgetItem(mUi.treeWidget);
      bookmark->setText(0,
                        QString::fromStdString(
                           UtTime::ToString(it.time(),
                                            wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeFormatting())));
      bookmark->setBackgroundColor(1, wkf::util::GetColor(QString::fromStdString(it.msgType())));
      bookmark->setText(2, QString::fromStdString(it.msgType()));
      longestType = std::max(longestType, QString::fromStdString(it.msgType()).size());
      bookmark->setText(3, QString::fromStdString(it.msgText()));
      bookmark->setToolTip(2, WordwrapToolTip(QString::fromStdString(it.msgType())));
      bookmark->setToolTip(3, WordwrapToolTip(QString::fromStdString(it.msgText())));
      mUi.treeWidget->addTopLevelItem(bookmark);
   }
   mUi.treeWidget->resizeColumnToContents(0);
   mUi.treeWidget->resizeColumnToContents(1);
   if (longestType < cMAX_TIP_WIDTH)
   {
      mUi.treeWidget->resizeColumnToContents(2);
   }
   else
   {
      mUi.treeWidget->setColumnWidth(2, mUi.treeWidget->fontMetrics().width(" ") * cMAX_TIP_WIDTH);
   }
   mUi.treeWidget->header()->setStretchLastSection(true);
}
