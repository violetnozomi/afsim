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

#include "WkfCommVisRouterNodeDialog.hpp"

#include <QTextBlock>
#include <QTextDocumentFragment>

#include "WkfCommVisRouterGraphicsItem.hpp"

wkf::CommVisRouterNodeDialog::CommVisRouterNodeDialog(wkf::CommVisRouterGraphicsItem* aRouterObjPtr,
                                                      QWidget*                        aParent,
                                                      Qt::WindowFlags                 aFlags)
   : QDialog(aParent, aFlags)
   , mRouterObjPtr(aRouterObjPtr)
{
   setAttribute(Qt::WA_DeleteOnClose);
   mUI.setupUi(this);
   if (mRouterObjPtr)
   {
      mUI.mRouterNameLineEdit->setText(mRouterObjPtr->GetName());
      mUI.mRouterTypeLineEdit->setText(mRouterObjPtr->GetRouterType());
      mUI.mPlatformNameLineEdit->setText(mRouterObjPtr->GetPlatform());
      mUI.mGatewayLineEdit->setText(mRouterObjPtr->GetGateway());
      mUI.mGatewayAddressLineEdit->setText(mRouterObjPtr->GetAddress());
      mUI.mHopLimitLineEdit->setText(QString::number(mRouterObjPtr->GetHopLimit()));
      mUI.mDefaultProtocolCheckbox->setChecked(mRouterObjPtr->UseDefaultProtocol());
      mUI.mMulticastProtocolCheckbox->setChecked(mRouterObjPtr->UseMulticastProtocol());

      // Set the checkboxes to "read-only"
      mUI.mDefaultProtocolCheckbox->setAttribute(Qt::WA_TransparentForMouseEvents);
      mUI.mDefaultProtocolCheckbox->setFocusPolicy(Qt::NoFocus);
      mUI.mMulticastProtocolCheckbox->setAttribute(Qt::WA_TransparentForMouseEvents);
      mUI.mMulticastProtocolCheckbox->setFocusPolicy(Qt::NoFocus);

      // Hide the automated interface linking checkbox
      mUI.mAILCheckbox->hide();

      connect(this, &QDialog::finished, mRouterObjPtr, &CommVisRouterGraphicsItem::DialogClosed);
   }
}
