// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfCommVisViewLinksDialog.hpp"

#include <QPointer>
#include <QTableWidgetItem>

#include "UtCast.hpp"

wkf::CommVisViewLinksDialog::CommVisViewLinksDialog(QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);
   mUI.mAddLinkPushButton->hide();
}

wkf::CommVisViewLinksDialog::CommVisViewLinksDialog(const std::vector<wkf::CommEvent::LinkInfo>& aLinkInfo,
                                                    QWidget*                                     aParent,
                                                    Qt::WindowFlags                              aFlags)
   : QDialog(aParent, aFlags)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);

   const QStringList labels = {"Link Platform Name", "Link Comm Name", "Link Comm Address", ""};
   mUI.mLinksTable->setHorizontalHeaderLabels(labels);
   mUI.mLinksTable->setAlternatingRowColors(false);

   mUI.mLinksTable->setRowCount(ut::cast_to_int(aLinkInfo.size()));

   int row = 0;
   for (const auto& curLink : aLinkInfo)
   {
      QTableWidgetItem* destPlatformItem = new QTableWidgetItem(QString::fromStdString(curLink.mDestinationPlatformName));
      mUI.mLinksTable->setItem(row, 0, destPlatformItem);

      QTableWidgetItem* destCommItem = new QTableWidgetItem(QString::fromStdString(curLink.mDestinationCommName));
      mUI.mLinksTable->setItem(row, 1, destCommItem);

      QTableWidgetItem* destAddressItem = new QTableWidgetItem(QString::fromStdString(curLink.mDestinationAddress));
      mUI.mLinksTable->setItem(row, 2, destAddressItem);
      ++row;
   }
}
