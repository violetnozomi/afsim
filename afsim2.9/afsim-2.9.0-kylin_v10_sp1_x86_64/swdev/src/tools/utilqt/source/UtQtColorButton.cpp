// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "UtQtColorButton.hpp"

#include <sstream>

#include <QColorDialog>
#include <QMessageBox>

#include "UtQt.hpp"


UtQtColorButton::UtQtColorButton(QWidget* aParent)
   : QPushButton(aParent)
   , mShowAlpha(false)
   , mWindowName("Choose Color")
{
   QObject::connect(this, &UtQtColorButton::clicked, this, &UtQtColorButton::HandleClick);
}

void UtQtColorButton::HandleClick()
{
   if (!mWarningStr.isEmpty())
   {
      QMessageBox::warning(this, "Unable to Change Color", mWarningStr, QMessageBox::Ok);
      return;
   }
   emit Clicked();

   QColorDialog* dialog = new QColorDialog(this);
   dialog->setModal(true);
   if (mShowAlpha)
   {
      dialog->setOptions(QColorDialog::ShowAlphaChannel);
   }
   dialog->setCurrentColor(mCurrentColor);

   QObject::connect(dialog,
                    &QDialog::accepted,
                    [=]()
                    {
                       SetColor(dialog->currentColor());
                       ColorSelected(mCurrentColor);
                       emit ColorChanged(mCurrentColor);
                    });
   QObject::connect(dialog, &QDialog::rejected, [=]() { emit DialogClosed(); });

   dialog->show();
}

void UtQtColorButton::SetColor(QColor aColor)
{
   mCurrentColor = aColor;
   QPixmap ico(20, 20);
   ico.fill(mCurrentColor);
   setIcon(ico);
   std::stringstream txt;
   txt << "R:" << mCurrentColor.redF() << " G:" << mCurrentColor.greenF() << " B:" << mCurrentColor.blueF()
       << " A:" << mCurrentColor.alphaF();
   setToolTip(txt.str().c_str());
}
