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


#include "WkfCommVisNetworkNodeDialog.hpp"

#include "WkfCommVisNetworkGraphicsItem.hpp"

wkf::CommVisNetworkNodeDialog::CommVisNetworkNodeDialog(CommVisNetworkGraphicsItem* aNetworkPtr,
                                                        QWidget*                    aParent,
                                                        Qt::WindowFlags             aFlags)
   : QDialog(aParent, aFlags)
   , mNetworkObjPtr(aNetworkPtr)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);

   if (mNetworkObjPtr)
   {
      mUI.mNetworkNameLineEdit->setText(mNetworkObjPtr->GetName());
      mUI.mNetworkAddressLineEdit->setText(mNetworkObjPtr->GetAddress());
      mUI.mNetworkTypeLineEdit->setText(mNetworkObjPtr->GetNetworkType());

      connect(this, &QDialog::finished, mNetworkObjPtr, &CommVisNetworkGraphicsItem::DialogClosed);
   }
}
