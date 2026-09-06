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

#include "WkfCommVisGatewayNodeDialog.hpp"

#include "WkfCommVisGatewayGraphicsItem.hpp"

wkf::CommVisGatewayNodeDialog::CommVisGatewayNodeDialog(CommVisGatewayGraphicsItem* aGatewayObjPtr,
                                                        QWidget*                    aParent,
                                                        Qt::WindowFlags             aFlags)
   : QDialog(aParent, aFlags)
   , mGatewayObjPtr(aGatewayObjPtr)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);

   if (mGatewayObjPtr)
   {
      mUI.mPlatformNameLineEdit->setText(mGatewayObjPtr->GetPlatformName());
      mUI.mGatewayNameLineEdit->setText(mGatewayObjPtr->GetName());
      mUI.mGatewayAddressLineEdit->setText(mGatewayObjPtr->GetAddress());
      mUI.mGatewayRemotePlatformLineEdit->setText(mGatewayObjPtr->GetRemotePlatform());
      mUI.mGatewayRemoteCommLineEdit->setText(mGatewayObjPtr->GetRemoteComm());
      mUI.mGatewayRemoteAddressLineEdit->setText(mGatewayObjPtr->GetRemoteAddress());

      // Connections
      connect(this, &QDialog::finished, mGatewayObjPtr, &CommVisGatewayGraphicsItem::DialogClosed);
   }
}
