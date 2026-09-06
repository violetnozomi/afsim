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
#include "UtQtFontButton.hpp"

#include <QFontDialog>

UtQtFontButton::UtQtFontButton(QWidget* aParent)
{
   connect(this,
           &QPushButton::clicked,
           [this]
           {
              bool  ok      = true;
              QFont newFont = QFontDialog::getFont(&ok, mFont, this, QString(mWindowTitle));
              if (ok)
              {
                 SetFont(newFont);
              }
           });
}

void UtQtFontButton::SetFont(const QFont& aFont)
{
   mFont = aFont;
   setText(aFont.family() + ", " + QString::number(aFont.pointSize()));
}
