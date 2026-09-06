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

#include "WkfCommVisCommNodeDialog.hpp"

#include "UtQtMemory.hpp"
#include "WkfCommVisCommGraphicsItem.hpp"
#include "WkfCommVisViewLinksDialog.hpp"

wkf::CommVisCommNodeDialog::CommVisCommNodeDialog(CommVisCommGraphicsItem* aCommObjPtr, QWidget* aParent, Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mCommObjPtr(aCommObjPtr)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);

   if (mCommObjPtr)
   {
      mUI.mPlatformNameLineEdit->setText(mCommObjPtr->GetPlatformName());
      mUI.mCommNameLineEdit->setText(mCommObjPtr->GetName());
      mUI.mCommAddressLineEdit->setText(mCommObjPtr->GetAddress());
      mUI.mCommTypeLineEdit->setText(mCommObjPtr->GetType());
      mUI.mNetworkNameLineEdit->setText(mCommObjPtr->GetNetworkName());
      mUI.mNetworkAddressLineEdit->setText(mCommObjPtr->GetNetworkAddress());
      mUI.mRouterNameLineEdit->setText(mCommObjPtr->GetRouterName());
      mUI.mRouterAddressLineEdit->setText(mCommObjPtr->GetRouterAddress());
      mUI.mGatewayCommLineEdit->setText(mCommObjPtr->GetGatewayName());
      mUI.mGatewayAddressLineEdit->setText(mCommObjPtr->GetGatewayAddress());

      // Connections
      connect(this, &QDialog::finished, mCommObjPtr, &CommVisCommGraphicsItem::DialogClosed);

      mLinksDialog = ut::qt::make_qt_ptr<CommVisViewLinksDialog>(mCommObjPtr->GetLinks(), this, windowFlags());
      mLinksDialog->setWindowTitle(mCommObjPtr->GetPlatformName() + "'s Links");
      connect(mUI.mViewLinksPushButton, &QPushButton::clicked, mLinksDialog.data(), &QDialog::show);

      mUI.mDeleteCommPushButton->hide();
   }
}
